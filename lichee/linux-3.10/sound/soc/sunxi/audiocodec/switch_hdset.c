/*
 * drivers\staging\android\switch\switch_headset.c
 * (C) Copyright 2010-2016
 * reuuimllatech Co., Ltd. <www.reuuimllatech.com>
 * huangxin <huangxin@reuuimllatech.com>
 *
 * some simple description for this code
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/switch.h>
#include <linux/irq.h>
#include <linux/input.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/input.h>
#include <linux/workqueue.h>
#include <linux/gpio.h>
#include <linux/power/scenelock.h>
#include <linux/pm.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>

#include "switch_hdset.h"

#undef SWITCH_DBG

#if (0)
    #define SWITCH_DBG(format,args...)  printk(KERN_ERR "[SWITCH] "format,##args)
#else
    #define SWITCH_DBG(...)
#endif

#define FUNCTION_NAME "h2w"

//static int headphone_direct_used = 0;
/*1=headphone in slot, else 0*/
static int headphone_state = 0;
/* key define */
#define KEY_HEADSETHOOK         226

/*
* 	CIRCLE_COUNT == 0, check the earphone state three times(audio_hmic_irq:one time and earphone_switch_timer_poll:two times)
*	CIRCLE_COUNT == 1, check the earphone state four times(audio_hmic_irq:one time and earphone_switch_timer_poll:three times)
*	CIRCLE_COUNT == 2, check the earphone state five times(audio_hmic_irq:one time and earphone_switch_timer_poll:four times)
*/
#define CIRCLE_COUNT			12

#define HEADSET_DEBOUNCE 		3
//static int debounce_val[HEADSET_DEBOUNCE+1];

static void switch_resume_events(struct work_struct *work);
static struct workqueue_struct *resume_switch_work_queue = NULL;

static void codec_init_events(struct work_struct *work);
static struct workqueue_struct *init_work_queue;
static DECLARE_WORK(codec_init_work, codec_init_events);

static int req_mute_status;
static unsigned int mute_gpio;

enum headphone_mode_u {
	HEADPHONE_IDLE,
	FOUR_HEADPHONE_PLUGIN,
	THREE_HEADPHONE_PLUGIN,
	HOOK_CHCCK_DOWN
};

struct gpio_switch_data {

	int hookdown;
	int volumeup;
	int volumedown;
	struct switch_dev sdev;
	int state;
	int reset_flag;
	int check_three_count;
	int check_four_count;
	int check_debounce_count;
	int check_plugout_count;
	int check_hook_count;
	int volume_down_count;
	int volume_up_count;
	enum headphone_mode_u mode;		/* mode for three/four sector headphone */
	struct work_struct work;
	struct semaphore sem;
	struct timer_list timer;
	struct timer_list mute_timer;
	struct timer_list volume_timer;
	spinlock_t 	 lock;

	struct work_struct hook_work;
	struct work_struct resume_work;
	struct input_dev *key;
};

static void earphone_open_mute(unsigned long data)
{
	SWITCH_DBG("%s,line:%d\n", __func__, __LINE__);
	hmic_wr_prcm_control(DAC_PA_SRC, 0x1, 2, 0x1);
	hmic_wr_prcm_control(DAC_PA_SRC, 0x1, 3, 0x1);
}

static void earphone_switch_timer_poll(unsigned long data)
{
	int tmp = 0;
	struct gpio_switch_data	*switch_data =(struct gpio_switch_data *)data;
	tmp = hmic_rdreg(SUNXI_HMIC_DATA);
	tmp &= 0x1f;
	SWITCH_DBG("%s,line:%d,tmp:%x\n",__func__,__LINE__,tmp);
	if ((tmp >= 0xe) && (switch_data->state == 1) && (switch_data->reset_flag==0)) {
		if (tmp >= 0x11) {
			input_report_key(switch_data->key, KEY_HEADSETHOOK, 1);
			input_sync(switch_data->key);
			switch_data->mode = HOOK_CHCCK_DOWN;
			switch_data->hookdown = 1;
			switch_data->volumeup = 0;
			switch_data->volumedown = 0;
			SWITCH_DBG("%s,KEY_HEADSETHOOK-1---,line:%d\n",__func__,__LINE__);
		} else if (tmp == 0x10) {
			input_report_key(switch_data->key, KEY_VOLUMEUP, 1);
			input_sync(switch_data->key);
			switch_data->volumeup = 1;
			switch_data->hookdown = 0;
			//switch_data->volumeup = 0;
			switch_data->volumedown = 0;
			SWITCH_DBG("KEY_VOLUME+++1111111++++,line:%d\n",__LINE__);

		} else if ((tmp == 0xf)&&(switch_data->reset_flag==0)) {
			input_report_key(switch_data->key, KEY_VOLUMEDOWN, 1);
			input_sync(switch_data->key);
			switch_data->volumedown = 1;
			switch_data->hookdown = 0;
			switch_data->volumeup = 0;
			//switch_data->volumedown = 0;
			SWITCH_DBG("KEY_VOLUME----3333333----,line:%d\n",__LINE__);
		}


	}else if(tmp>=0x1 && tmp<0xe && (switch_data->state == 1) && (switch_data->reset_flag==0)) {
		if (switch_data->hookdown) {
			input_report_key(switch_data->key, KEY_HEADSETHOOK, 0);
			input_sync(switch_data->key);
			switch_data->hookdown = 0;
			SWITCH_DBG("%s,KEY_HEADSETHOOK-0---,line:%d\n",__func__,__LINE__);
		}else if (switch_data->volumeup) {
			input_report_key(switch_data->key, KEY_VOLUMEUP, 0);
			input_sync(switch_data->key);
			switch_data->volumeup = 0;
			SWITCH_DBG("%s,KEY_VOLUME+++222222+++,line:%d\n",__func__,__LINE__);
		} else if (switch_data->volumedown) {
			input_report_key(switch_data->key, KEY_VOLUMEDOWN, 0);
			input_sync(switch_data->key);
			switch_data->volumedown = 0;
			SWITCH_DBG("%s,KEY_VOLUME--44444444----,line:%d\n",__func__,__LINE__);
		}
	} else {
		if (tmp >= 0x11) {
			if (switch_data->check_three_count > CIRCLE_COUNT) {
				/*it means the three sections earphone has plun in*/
				switch_data->mode = THREE_HEADPHONE_PLUGIN;
				switch_data->state 		= 2;
				schedule_work(&switch_data->work);
				switch_data->check_three_count = 0;
				schedule_work(&switch_data->work);
				switch_data->reset_flag = 0;
			}else{
				switch_data->check_three_count++;
				if (((&switch_data->timer) != NULL)) {
					mod_timer(&switch_data->timer, jiffies +  HZ/30);
				}
			}
			switch_data->check_four_count 		= 0;
			//switch_data->check_three_count 		= 0;
			switch_data->check_plugout_count =0;

		} else if (tmp>=0x1 && tmp<0xe) {
			if (switch_data->check_four_count > CIRCLE_COUNT) {
				/*it means the four sections earphone has plun in*/
				switch_data->mode = FOUR_HEADPHONE_PLUGIN;
				switch_data->state 		= 1;
				schedule_work(&switch_data->work);
				switch_data->check_four_count = 0;
				switch_data->reset_flag = 0;
			}else{
				switch_data->check_four_count++;
				if (((&switch_data->timer) != NULL)) {
					mod_timer(&switch_data->timer, jiffies +  HZ/30);
				}
			}
			//switch_data->check_four_count 		= 0;
			switch_data->check_three_count 		= 0;
			switch_data->check_plugout_count =0;
		} else {
			if (switch_data->check_plugout_count > CIRCLE_COUNT) {
				switch_data->reset_flag = 0;
				switch_data->state 		= 0;
				switch_data->mode = HEADPHONE_IDLE;
				switch_data->check_plugout_count = 0;
				schedule_work(&switch_data->work);
				switch_data->reset_flag = 0;
			}else{
				switch_data->check_plugout_count++;
				if (((&switch_data->timer) != NULL)) {
					mod_timer(&switch_data->timer, jiffies +  HZ/30);
				}
			}

			switch_data->check_four_count 		= 0;
			switch_data->check_three_count 		= 0;
		}
		switch_data->volumedown = 0;
		switch_data->volumeup = 0;
		switch_data->hookdown = 0;
	}


}

static void earphone_switch_work(struct work_struct *work)
{
	struct gpio_switch_data	*switch_data =
		container_of(work, struct gpio_switch_data, work);
	down(&switch_data->sem);
	if (switch_data->sdev.state == 1 && switch_data->state == 2) {
		if ((&switch_data->timer) != NULL) {
			mod_timer(&switch_data->timer, jiffies +  HZ/10);
		}
		switch_data->state = 1;
	} else
		switch_set_state(&switch_data->sdev, switch_data->state);
	up(&switch_data->sem);
	SWITCH_DBG("%s,line:%d,switch_data->state:%d\n",__func__,__LINE__,switch_data->state);
}

static irqreturn_t audio_hmic_irq(int irq, void *dev_id)
{
	int tmp = 0;
	struct gpio_switch_data *switch_data = (struct gpio_switch_data *)dev_id;

	if (switch_data == NULL) {
		return IRQ_NONE;
	}

	if (switch_data->sdev.state == 0) {
		/*mute headphone pa*/
		hmic_wr_prcm_control(DAC_PA_SRC, 0x1, 2, 0x0);
		hmic_wr_prcm_control(DAC_PA_SRC, 0x1, 3, 0x0);
	}
	tmp = hmic_rdreg(SUNXI_HMIC_DATA);
	SWITCH_DBG("%s,line:%d,tmp:%x\n",__func__,__LINE__,tmp);
	if ((0x1<<HMIC_EARPHONE_OUT_IRQ_PEND)&tmp) {
		switch_data->reset_flag = 1;
		SWITCH_DBG("%s,line:%d,tmp:%x,switch_data->reset_flag:%d\n",__func__,__LINE__,tmp,switch_data->reset_flag);
	}
	tmp = tmp&0x1f;
	if (((&switch_data->mute_timer) != NULL)) {
		del_timer(&switch_data->mute_timer);
	}
	init_timer(&switch_data->mute_timer);
	switch_data->mute_timer.function = earphone_open_mute;
	switch_data->mute_timer.data = (unsigned long)switch_data;
	mod_timer(&switch_data->mute_timer, jiffies +  (HZ+HZ/2));

	if (((&switch_data->timer) != NULL)) {
		del_timer(&switch_data->timer);
	}
	init_timer(&switch_data->timer);
	switch_data->timer.function = earphone_switch_timer_poll;
	switch_data->timer.data = (unsigned long)switch_data;
	if (switch_data->sdev.state == 1)
		mod_timer(&switch_data->timer, jiffies +  HZ/20 );
	else
		mod_timer(&switch_data->timer, jiffies +  HZ/5 );

	hmic_wr_control(SUNXI_HMIC_DATA, 0x1, HMIC_KEY_DOWN_IRQ_PEND, 0x1);
	hmic_wr_control(SUNXI_HMIC_DATA, 0x1, HMIC_EARPHONE_IN_IRQ_PEND, 0x1);
	hmic_wr_control(SUNXI_HMIC_DATA, 0x1, HMIC_KEY_UP_IRQ_PEND, 0x1);
	hmic_wr_control(SUNXI_HMIC_DATA, 0x1, HMIC_EARPHONE_OUT_IRQ_PEND, 0x1);
	hmic_wr_control(SUNXI_HMIC_DATA, 0x1, HMIC_DATA_IRQ_PEND, 0x1);

	return IRQ_HANDLED;
}

static void sunxi_hppa_enable(void) {
    /*fix the resume blaze blaze noise*/
	hmic_wr_prcm_control(ADDA_APT2, 0x1, PA_SLOPE_SELECT, 0x0);
	hmic_wr_prcm_control(PAEN_HP_CTRL, 0x3, PA_ANTI_POP_CTRL, 0x1);
	hmic_wr_prcm_control(PA_ANTI_POP_REG_CTRL, 0x7, PA_ANTI_POP_EN, 0x2);
	usleep_range(100,200);
	/*enable pa*/
	hmic_wr_prcm_control(PAEN_HP_CTRL, 0x1, HPPAEN, 0x1);
}

static void sunxi_hbias_enable(void) {
	/*audio codec hardware bug. the HBIASADCEN bit must be enable in init*/
	#ifdef CONFIG_ARCH_SUN8IW5
	hmic_wr_prcm_control(MIC1G_MICBIAS_CTRL, 0x1, HMICBIAS_MODE, 0x1);
	#else/*CONFIG_ARCH_SUN8IW8*/
	hmic_wr_prcm_control(MIC1G_MICBIAS_CTRL, 0x1, HMICADCEN, 0x1);
	#endif
	//hmic_wr_prcm_control(MIC1G_MICBIAS_CTRL, 0x1, HMICBIAS_MODE, 0x1);
	hmic_wr_prcm_control(MIC1G_MICBIAS_CTRL, 0x1, HMICBIASEN, 0x1);

}

static void codec_init_events(struct work_struct *work)
{
	/*fix the resume blaze blaze noise*/
	sunxi_hppa_enable();
	msleep(650);
	/*audio codec hardware bug. the HBIASADCEN bit must be enable in init*/
	sunxi_hbias_enable();

	pr_debug("====codec_init_events===\n");
}

static void switch_resume_events(struct work_struct *work)
{
	int tmp = 0,tmp1 = 0;
	//script_item_u val;
	//script_item_value_type_e  type;

	struct gpio_switch_data *switch_data = container_of(work,
				struct gpio_switch_data, resume_work);

	if (switch_data == NULL) {
		SWITCH_DBG("%s, %d, switch_data is NULL\n", __func__, __LINE__);
		return;
	}
	/*fix the resume blaze blaze noise*/
	hmic_wr_prcm_control(ADDA_APT2, 0x1, PA_SLOPE_SELECT, 0x1);
	hmic_wr_prcm_control(PAEN_HP_CTRL, 0x1, HPPAEN, 0x1);
	msleep(650);

	tmp = hmic_rdreg(SUNXI_HMIC_DATA);
	tmp1 =(tmp&0x1f);
	switch_data->mode = HEADPHONE_IDLE;
	switch_data->state = -1;
	switch_data->sdev.state 		= -1;
	switch_data->check_three_count = 0;
	switch_data->check_four_count = 0;
	switch_data->check_plugout_count 	= 0;
	switch_data->check_debounce_count 	= 0;
	switch_data->check_hook_count 		= 0;
	switch_data->volumedown = 0;
	switch_data->volumeup = 0;
	switch_data->hookdown = 0;

	hmic_wr_control(SUNXI_HMIC_CTL, 0xf, HMIC_M, 0x0);						/*0xf should be get from hw_debug 28*/
	hmic_wr_control(SUNXI_HMIC_CTL, 0xf, HMIC_N, 0x0);						/*0xf should be get from hw_debug 24 0xf*/
	hmic_wr_control(SUNXI_HMIC_CTL, 0x3, HMIC_DS_SAMP, 0x0); 				/*14 */
	hmic_wr_control(SUNXI_HMIC_CTL, 0x1f, HMIC_TH2_KEY, 0xe);				/*0xf should be get from hw_debug 8*/
	hmic_wr_control(SUNXI_HMIC_CTL, 0x1f, HMIC_TH1_EARPHONE, 0x1);			/*0x1 should be get from hw_debug 0*/
	/*enable irq keyup/keydown phone in/out*/
	hmic_wr_control(SUNXI_HMIC_CTL, 0x1, HMIC_KEY_DOWN_IRQ_EN, 0x1); 		/*17*/
	hmic_wr_control(SUNXI_HMIC_CTL, 0x1, HMIC_KEY_UP_IRQ_EN, 0x1); 			/*16*/
	hmic_wr_control(SUNXI_HMIC_CTL, 0x1, HMIC_EARPHONE_OUT_IRQ_EN, 0x1); 	/*20*/
	hmic_wr_control(SUNXI_HMIC_CTL, 0x1, HMIC_EARPHONE_IN_IRQ_EN, 0x1); 	/*19*/
	hmic_wr_control(SUNXI_HMIC_ENABLE, 0x1, HMIC_DIG_EN, 0x1);						/*0xf should be get from hw_debug 28*/
	msleep(200);
		/*audio codec hardware bug. the HBIASADCEN bit must be enable in init*/
	hmic_wr_prcm_control(MIC1G_MICBIAS_CTRL, 0x1, HMICBIAS_MODE, 0x1);

	hmic_wr_prcm_control(MIC1G_MICBIAS_CTRL, 0x1, HMICBIASEN, 0x1);
	SWITCH_DBG("%s,line:%d\n",__func__,__LINE__);
	if (((&switch_data->timer) != NULL)) {
		del_timer(&switch_data->timer);
	}
	init_timer(&switch_data->timer);
	switch_data->timer.function = earphone_switch_timer_poll;
	switch_data->timer.data = (unsigned long)switch_data;
	mod_timer(&switch_data->timer, jiffies +  HZ/5 );

}

static ssize_t switch_gpio_print_state(struct switch_dev *sdev, char *buf)
{
	struct gpio_switch_data	*switch_data =
		container_of(sdev, struct gpio_switch_data, sdev);

	return sprintf(buf, "%d\n", switch_data->state);
}

static ssize_t print_headset_name(struct switch_dev *sdev, char *buf)
{
	struct gpio_switch_data	*switch_data =
		container_of(sdev, struct gpio_switch_data, sdev);

	return sprintf(buf, "%s\n", switch_data->sdev.name);
}

static int gpio_switch_probe(struct platform_device *pdev)
{
//	int headphone_mute_used = 0;
	struct gpio_switch_platform_data *pdata = pdev->dev.platform_data;
	struct gpio_switch_data *switch_data;
	int ret = 0;
//	script_item_u val;
//	script_item_value_type_e  type;

	if (!pdata) {
		return -EBUSY;
	}

	hmic_wr_control(SUNXI_HMIC_CTL, 0xf, HMIC_M, 0x0);						/*0xf should be get from hw_debug 28*/
	hmic_wr_control(SUNXI_HMIC_CTL, 0xf, HMIC_N, 0x0);						/*0xf should be get from hw_debug 24 0xf*/
	hmic_wr_control(SUNXI_HMIC_CTL, 0x3, HMIC_DS_SAMP, 0x0); 				/*14 */
	hmic_wr_control(SUNXI_HMIC_CTL, 0x1f, HMIC_TH2_KEY, 0xe);				/*0xf should be get from hw_debug 8*/
	hmic_wr_control(SUNXI_HMIC_CTL, 0x1f, HMIC_TH1_EARPHONE, 0x1);			/*0x1 should be get from hw_debug 0*/
	/*enable irq keyup/keydown phone in/out*/
	hmic_wr_control(SUNXI_HMIC_CTL, 0x1, HMIC_KEY_DOWN_IRQ_EN, 0x1); 		/*17*/
	hmic_wr_control(SUNXI_HMIC_CTL, 0x1, HMIC_KEY_UP_IRQ_EN, 0x1); 			/*16*/
	hmic_wr_control(SUNXI_HMIC_CTL, 0x1, HMIC_EARPHONE_OUT_IRQ_EN, 0x1); 	/*20*/
	hmic_wr_control(SUNXI_HMIC_CTL, 0x1, HMIC_EARPHONE_IN_IRQ_EN, 0x1); 	/*19*/

	switch_data = kzalloc(sizeof(struct gpio_switch_data), GFP_KERNEL);
	if (!switch_data) {
		pr_err("%s,line:%d\n", __func__, __LINE__);
		return -ENOMEM;
	}

	platform_set_drvdata(pdev, (void *)switch_data);

	switch_data->check_three_count 		= 0;
	switch_data->check_plugout_count 	= 0;
	switch_data->check_debounce_count 	= 0;
	switch_data->check_hook_count		= 0;
	switch_data->check_four_count 		= 0;
	switch_data->sdev.state 		= 0;
	switch_data->reset_flag 		= 0;
	switch_data->state				= -1;
	switch_data->sdev.name 			= pdata->name;
	switch_data->sdev.print_name 	= print_headset_name;
	switch_data->sdev.print_state 	= switch_gpio_print_state;
	INIT_WORK(&switch_data->work, earphone_switch_work);
	INIT_WORK(&switch_data->resume_work, switch_resume_events);
	/* create input device */
	switch_data->key = input_allocate_device();
	if (!switch_data->key) {
		pr_err(KERN_ERR "gpio_switch_probe: not enough memory for input device\n");
		ret = -ENOMEM;
		goto err_input_allocate_device;
	}

	switch_data->key->name          = "headset";
	switch_data->key->phys          = "headset/input0";
	switch_data->key->id.bustype    = BUS_HOST;
	switch_data->key->id.vendor     = 0x0001;
	switch_data->key->id.product    = 0xffff;
	switch_data->key->id.version    = 0x0100;

	switch_data->key->evbit[0] = BIT_MASK(EV_KEY);

	set_bit(KEY_HEADSETHOOK, switch_data->key->keybit);
	set_bit(KEY_VOLUMEUP, switch_data->key->keybit);
	set_bit(KEY_VOLUMEDOWN, switch_data->key->keybit);

	ret = input_register_device(switch_data->key);
	if (ret) {
		pr_err(KERN_ERR "gpio_switch_probe: input_register_device failed\n");
		goto err_input_register_device;
	}

	headphone_state = 0;
	sema_init(&switch_data->sem, 1);
	spin_lock_init(&switch_data->lock);

	ret = switch_dev_register(&switch_data->sdev);
	if (ret < 0) {
		goto err_switch_dev_register;
	}

	ret = request_irq(78, audio_hmic_irq, 0, "audio_hmic_irq", switch_data);
	if (ret < 0) {
		pr_err("request irq err\n");
		ret = -EINVAL;
		goto err_request_irq;
	}

	resume_switch_work_queue = create_singlethread_workqueue("switch_resume");
	if (resume_switch_work_queue == NULL) {
		pr_err("[switch_headset] try to create workqueue for codec failed!\n");
		ret = -ENOMEM;
		goto err_switch_work_queue;
	}

	init_work_queue = create_singlethread_workqueue("codec_init");
	if (init_work_queue == NULL) {
		pr_err("[codec] try to create workqueue for codec failed!\n");
		ret = -ENOMEM;
		goto err_switch_work_queue;
	}
	queue_work(init_work_queue, &codec_init_work);
	return 0;

err_switch_work_queue:
	free_irq(78, switch_data);
err_request_irq:
	switch_dev_unregister(&switch_data->sdev);
err_input_register_device:
    if(switch_data->key){
        input_free_device(switch_data->key);
    }

err_input_allocate_device:
    switch_dev_unregister(&switch_data->sdev);

err_switch_dev_register:
	kfree(switch_data);

	return ret;
}

int aw_display_suspended = 0;
EXPORT_SYMBOL(aw_display_suspended);

static int switch_suspend(struct platform_device *pdev,pm_message_t state)
{
	//int headphone_mute_used = 0;
	//script_item_u val;
//	script_item_value_type_e  type;
	struct gpio_switch_data *switch_data;
	switch_data = (struct gpio_switch_data *)platform_get_drvdata(pdev);
	pr_debug("[headset]:suspend start\n");
	/* check if called in talking standby */
	if (check_scene_locked(SCENE_TALKING_STANDBY) == 0) {
		pr_debug("In talking standby, do not suspend!!\n");
		return 0;
	}
	msleep(150);
	/*fix the resume blaze blaze noise*/
	hmic_wr_prcm_control(ADDA_APT2, 0x1, PA_SLOPE_SELECT, 0x1);

	hmic_wr_control(SUNXI_HMIC_CTL, 0xf, HMIC_M, 0x0);						/*0xf should be get from hw_debug 28*/
	hmic_wr_control(SUNXI_HMIC_CTL, 0xf, HMIC_N, 0x0);						/*0xf should be get from hw_debug 24 0xf*/
	hmic_wr_control(SUNXI_HMIC_CTL, 0x3, HMIC_DS_SAMP, 0x0); 				/*14 */
	hmic_wr_control(SUNXI_HMIC_CTL, 0x1f, HMIC_TH2_KEY, 0x0);				/*0xf should be get from hw_debug 8*/
	hmic_wr_control(SUNXI_HMIC_CTL, 0x1f, HMIC_TH1_EARPHONE, 0x0);			/*0x1 should be get from hw_debug 0*/
	/*disable irq keyup/keydown phone in/out*/
	hmic_wr_control(SUNXI_HMIC_CTL, 0x1, HMIC_KEY_DOWN_IRQ_EN, 0x0); 		/*17*/
	hmic_wr_control(SUNXI_HMIC_CTL, 0x1, HMIC_KEY_UP_IRQ_EN, 0x0); 			/*16*/
	hmic_wr_control(SUNXI_HMIC_CTL, 0x1, HMIC_EARPHONE_OUT_IRQ_EN, 0x0); 	/*20*/
	hmic_wr_control(SUNXI_HMIC_CTL, 0x1, HMIC_EARPHONE_IN_IRQ_EN, 0x0); 	/*19*/
	hmic_wr_control(SUNXI_HMIC_ENABLE, 0x1, HMIC_DIG_EN, 0x0);						/*0xf should be get from hw_debug 28*/

	/*disable pa*/
	hmic_wr_prcm_control(PAEN_HP_CTRL, 0x1, HPPAEN, 0x0);
	hmic_wr_prcm_control(MIC1G_MICBIAS_CTRL, 0x1, HMICBIASEN, 0x0);
	switch_set_state(&switch_data->sdev, 0);
	SWITCH_DBG("%s,line:%d\n", __func__, __LINE__);
	msleep(350);
	aw_display_suspended = 1;
	return 0;
}

static int switch_resume(struct platform_device *pdev)
{
	struct gpio_switch_data *switch_data;
	pr_debug("[headset]:resume start\n");
	if(aw_display_suspended == 1)
	{
		SWITCH_DBG("%s,line:%d\n", __func__, __LINE__);
		return 0;
	}

	SWITCH_DBG("%s,line:%d\n", __func__, __LINE__);

	switch_data = (struct gpio_switch_data *)platform_get_drvdata(pdev);

	if (switch_data != NULL) {
		queue_work(resume_switch_work_queue, &switch_data->resume_work);
	}
	return 0;
}

static void switch_shutdown(struct platform_device *devptr)
{
	//if (headphone_mute_used) {
		/*config gpio info of headphone_mute_used,
		 *the default pa config is close(check sys_config.fex).
		 */
	//	gpio_set_value(mute_gpio, 0);
	//}
	msleep(150);
	/*fix the resume blaze blaze noise*/
	hmic_wr_prcm_control(ADDA_APT2, 0x1, PA_SLOPE_SELECT, 0x1);
	/*disable pa*/
	hmic_wr_prcm_control(PAEN_HP_CTRL, 0x1, HPPAEN, 0x0);
	msleep(450);
}

static int gpio_switch_remove(struct platform_device *pdev)
{
	struct gpio_switch_data *switch_data = platform_get_drvdata(pdev);

	if (resume_switch_work_queue != NULL) {
		flush_workqueue(resume_switch_work_queue);
		destroy_workqueue(resume_switch_work_queue);
	}

    if (switch_data->key) {
        input_unregister_device(switch_data->key);
        input_free_device(switch_data->key);
    }

    switch_dev_unregister(&switch_data->sdev);

	kfree(switch_data);

	return 0;
}

static struct platform_driver gpio_switch_driver = {
	.probe		= gpio_switch_probe,
	.remove		= gpio_switch_remove,
	.driver		= {
		.name	= "sun8iw5-switch-gpio",
		.owner	= THIS_MODULE,
	},
	.suspend	= switch_suspend,
	.resume		= switch_resume,
	.shutdown   = switch_shutdown,
};

static struct gpio_switch_platform_data headset_switch_data = {
    .name = "h2w",
};

static struct platform_device gpio_switch_device = {
	.name = "sun8iw5-switch-gpio",
	.dev = {
		.platform_data = &headset_switch_data,
	}
};

static int __init gpio_switch_init(void)
{
	int ret = 0;

	ret = platform_device_register(&gpio_switch_device);
	if (ret == 0) {
		ret = platform_driver_register(&gpio_switch_driver);
	}

	return ret;
}

static void __exit gpio_switch_exit(void)
{
	platform_driver_unregister(&gpio_switch_driver);
	platform_device_unregister(&gpio_switch_device);

}
module_init(gpio_switch_init);
module_exit(gpio_switch_exit);

MODULE_AUTHOR("huanxin<huanxin@reuuimllatech.com>");
MODULE_DESCRIPTION("GPIO Switch driver");
MODULE_LICENSE("GPL");
