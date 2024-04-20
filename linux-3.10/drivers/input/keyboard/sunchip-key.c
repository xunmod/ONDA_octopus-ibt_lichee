/*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
*
* Copyright (c) 2011
*
* ChangeLog
*
*
*/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/irq.h>
#include <linux/input.h>
#include <linux/ioctl.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/interrupt.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <linux/time.h>
#include <linux/sys_config.h>
#include <linux/input.h>
#include <linux/clk.h>
#include <linux/ktime.h>
#include "sun8i-keyboard.h"
static int key_left=0;
static int key_right=0;
static int key_down=0;
static irq_num1=0;
static irq_num2=0;
static irq_num3=0;
//static int flag_record_down = 0;
//static int flag_cam_down = 0;
static struct input_dev *gpiokbd_dev;
//struct timer_list click_touch_timer ;
//bool key_down = false;
//static void click_touch_timer_handle(unsigned long data)
//{
	//send_key = true;
	//pr_err("aw====\n");
//}
static irqreturn_t  down_interrupt(int irq,void *dev_id)
{

      disable_irq_nosync(irq);
	mdelay(5);
	input_report_key(gpiokbd_dev, KEY_DOWN, gpio_get_value(key_down)==0?1:0);
    input_sync(gpiokbd_dev);

		mdelay(10);
		enable_irq(irq);
	   return IRQ_HANDLED;
}
static irqreturn_t  left_interrupt(int irq,void *dev_id)
{

      disable_irq_nosync(irq);
	mdelay(5);
    input_report_key(gpiokbd_dev, KEY_LEFT, gpio_get_value(key_left)==0?1:0);
    input_sync(gpiokbd_dev);
	mdelay(10);
	enable_irq(irq);
	return IRQ_HANDLED;
}
static irqreturn_t  right_interrupt(int irq,void *dev_id)
{


      disable_irq_nosync(irq);

     mdelay(5);
    input_report_key(gpiokbd_dev, KEY_RIGHT, gpio_get_value(key_right)==0?1:0);
    input_sync(gpiokbd_dev);
	mdelay(10);
	enable_irq(irq);
	return IRQ_HANDLED;
}
static int sysconfig_init_key() {
	int i;
	int ret;
	struct gpio_config val;
	struct device_node *np = NULL;

	np = of_find_node_by_name(NULL,"V3_KEY");
	if (!np) {
	 pr_err("ERROR! get ctp_para failed, func:%s, line:%d\n",__FUNCTION__, __LINE__);
	 return -1;
	}

	if (!of_device_is_available(np)) {
	 pr_err("%s: ctp is not used\n", __func__);
	 return -1;
	}
	val.gpio = of_get_named_gpio_flags(np, "KEY_DOWN", 0, (enum of_gpio_flags *)(&(val)));
	if (!gpio_is_valid(val.gpio))
		pr_err("%s: KEY_DOWN is invalid. \n",__func__ );
	key_left = val.gpio;
	if(0 != gpio_request(key_down, NULL)){
		printk("gpio sensor_int request failed.\n");
		goto script_get_err;
	}
	gpio_direction_input(key_down);

	val.gpio = of_get_named_gpio_flags(np, "KEY_LEFT", 0, (enum of_gpio_flags *)(&(val)));
	if (!gpio_is_valid(val.gpio))
		pr_err("%s: KEY_LEFT is invalid. \n",__func__ );
	key_down = val.gpio;
	if(0 != gpio_request(key_left, NULL)){
		printk("gpio sensor_int request failed.\n");
		goto script_get_err;
	}
	gpio_direction_input(key_left);

	val.gpio = of_get_named_gpio_flags(np, "KEY_RIGHT", 0, (enum of_gpio_flags *)(&(val)));
	if (!gpio_is_valid(val.gpio))
		pr_err("%s: KEY_RIGHT is invalid. \n",__func__ );
	key_right = val.gpio;
	if(0 != gpio_request(key_right, NULL)){
		printk("gpio sensor_int request failed.\n");
		goto script_get_err;
	}
	gpio_direction_input(key_right);

	gpiokbd_dev = input_allocate_device();
	if(!gpiokbd_dev){
		printk("gpiokbd: not enough memory for input device\n");
		goto script_get_err1;
	}
    gpiokbd_dev->name = "gpio-keyboard";
    gpiokbd_dev->phys = "gpiokbd/input0";
    gpiokbd_dev->id.bustype = BUS_HOST;
    gpiokbd_dev->id.vendor = 0x0001;
    gpiokbd_dev->id.product = 0x0001;
    gpiokbd_dev->id.version = 0x0100;
    gpiokbd_dev->evbit[0] = BIT_MASK(EV_KEY);

      set_bit(KEY_DOWN, gpiokbd_dev->keybit);
	  set_bit(KEY_LEFT, gpiokbd_dev->keybit);
	  set_bit(KEY_RIGHT, gpiokbd_dev->keybit);
      ret = input_register_device(gpiokbd_dev);

	  irq_num1=__gpio_to_irq(key_down);
        if (request_irq(irq_num1,down_interrupt,IRQF_TRIGGER_FALLING|IRQF_TRIGGER_RISING,"key-down",NULL)) {
                printk("Unable to request pulley lkey.\n");
				goto script_get_err4;
        }
		//printk("%s: gpio-int1 is %d. \n", __func__, irq_num1);
      irq_num2=__gpio_to_irq(key_left);
	   if (request_irq(irq_num2,left_interrupt,IRQF_TRIGGER_FALLING|IRQF_TRIGGER_RISING,"key-left",NULL)) {
                printk("Unable to request pulley lkey.\n");
				goto script_get_err3;
        }
		//printk("%s: gpio-int2 is %d. \n", __func__, irq_num2);
		irq_num3=__gpio_to_irq(key_right);
			if (request_irq(irq_num3,right_interrupt,IRQF_TRIGGER_FALLING|IRQF_TRIGGER_RISING,"key-right",NULL)) {
                printk("Unable to request pulley lkey.\n");
				goto script_get_err5;
        }
		//printk("%s: gpio-int3 is %d. \n", __func__, irq_num3);
	//init_timer(&click_touch_timer);
	//timer.function=&click_touch_timer_handle;
	//timer.expires=jiffies+2HZ;
	//add_timer(&click_touch_timer);



		//printk("ro_key success\n");
        return 0;
script_get_err5:
       free_irq(irq_num3, NULL);
script_get_err4:
     free_irq(irq_num1, NULL);
script_get_err3:
	free_irq(irq_num2,NULL);
script_get_err2:
     gpio_free(key_right);
script_get_err1:
    gpio_free(key_left);
script_get_err:
gpio_free(key_down);
	pr_notice("=========script_get_err============\n");

   return -1;

}

static int __init  gpio_key_init(void)
{
   int err=0;

	sysconfig_init_key();
   return err;
}
static void __exit  gpio_key_exit(void)
{
  // printk("gpio_key_exit\n");
	free_irq(irq_num2, NULL);
    free_irq(irq_num1, NULL);
	free_irq(irq_num3, NULL);
	//del_timer(&click_touch_timer);
    gpio_free(key_right);
    gpio_free(key_left);
	gpio_free(key_down);
    input_unregister_device(gpiokbd_dev);
}
module_init(gpio_key_init);
module_exit(gpio_key_exit);

MODULE_DESCRIPTION("a simple ithink gpio-key driver");
MODULE_AUTHOR("majian@sunchip-tech.com");
MODULE_LICENSE("GPL");
