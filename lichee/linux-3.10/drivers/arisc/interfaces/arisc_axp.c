/*
 *  drivers/arisc/interfaces/arisc_axp.c
 *
 * Copyright (c) 2012 Allwinner.
 * 2012-05-01 Written by sunny (sunny@allwinnertech.com).
 * 2012-10-01 Written by superm (superm@allwinnertech.com).
 *
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "../arisc_i.h"

/* nmi isr node, record current nmi interrupt handler and argument */
nmi_isr_t nmi_isr_node[2];
EXPORT_SYMBOL(nmi_isr_node);

/**
 * register call-back function, call-back function is for arisc notify some event to ac327,
 * axp/rtc interrupt for external interrupt NMI.
 * @type:  nmi type, pmu/rtc;
 * @func:  call-back function;
 * @para:  parameter for call-back function;
 *
 * @return: result, 0 - register call-back function successed;
 *                 !0 - register call-back function failed;
 * NOTE: the function is like "int callback(void *para)";
 *       this function will execute in system ISR.
 */
int arisc_nmi_cb_register(u32 type, arisc_cb_t func, void *para)
{
	if (nmi_isr_node[type].handler) {
		if(func == nmi_isr_node[type].handler) {
			ARISC_WRN("nmi interrupt handler register already\n");
			return 0;
		}
		/* just output warning message, overlay handler */
		ARISC_WRN("nmi interrupt handler register already\n");
		return -EINVAL;
	}
	nmi_isr_node[type].handler = func;
	nmi_isr_node[type].arg     = para;

	return 0;
}
EXPORT_SYMBOL(arisc_nmi_cb_register);


/**
 * unregister call-back function.
 * @type:  nmi type, pmu/rtc;
 * @func:  call-back function which need be unregister;
 */
void arisc_nmi_cb_unregister(u32 type, arisc_cb_t func)
{
	if ((nmi_isr_node[type].handler) != (func)) {
		/* invalid handler */
		ARISC_WRN("invalid handler for unreg\n\n");
		return ;
	}
	nmi_isr_node[type].handler = NULL;
	nmi_isr_node[type].arg     = NULL;
}
EXPORT_SYMBOL(arisc_nmi_cb_unregister);

#if defined CONFIG_SUNXI_ARISC_COM_DIRECTLY
int arisc_config_pmu_paras(void)
{
	struct device_node *np;
	u32 pmu_discharge_ltf = 0;
	u32 pmu_discharge_htf = 0;

	int result = 0;
	struct arisc_message *pmessage;

#if defined CONFIG_ARCH_SUN8IW5
	np = of_find_node_by_type(NULL, "charger0");
#else
	np = of_find_compatible_node(NULL, NULL, "allwinner,pmu0");
#endif
	if (!np) {
		ARISC_ERR("No allwinner,pmu0 node found\n");
		return -ENODEV;
	}

	if (of_property_read_u32(np, "pmu_discharge_ltf", &pmu_discharge_ltf)) {
		ARISC_ERR("parse pmu discharge ltf fail\n");
		return -EINVAL;
	}

	if (of_property_read_u32(np, "pmu_discharge_htf", &pmu_discharge_htf)) {
		ARISC_ERR("parse pmu discharge ltf fail\n");
		return -EINVAL;
	}

	/* allocate a message frame */
	pmessage = arisc_message_allocate(ARISC_MESSAGE_ATTR_HARDSYN);
	if (pmessage == NULL) {
		ARISC_WRN("allocate message failed\n");
		return -ENOMEM;
	}

	/* initialize message */
	pmessage->type       = ARISC_AXP_SET_PARAS;
	pmessage->private    = (void *)0x00; /* init pmu paras flag */
	pmessage->paras[0]   = pmu_discharge_ltf;
	pmessage->paras[1]   = pmu_discharge_htf;
	pmessage->paras[2]   = 0;

	pmessage->state      = ARISC_MESSAGE_INITIALIZED;
	pmessage->cb.handler = NULL;
	pmessage->cb.arg     = NULL;

	ARISC_INF("pmu_discharge_ltf:0x%x\n", pmessage->paras[0]);
	ARISC_INF("pmu_discharge_htf:0x%x\n", pmessage->paras[1]);

	/* send request message */
	arisc_hwmsgbox_send_message(pmessage, ARISC_SEND_MSG_TIMEOUT);

	/*check config fail or not*/
	if (pmessage->result) {
		ARISC_WRN("config pmu paras [%d] [%d] fail\n", pmessage->paras[0], pmessage->paras[1]);
		result = -EINVAL;
	}

	/* free allocated message */
	arisc_message_free(pmessage);

	return result;
}

int arisc_disable_nmi_irq(void)
{
	int                   result;
	struct arisc_message *pmessage;

	/* allocate a message frame */
	pmessage = arisc_message_allocate(ARISC_MESSAGE_ATTR_HARDSYN);
	if (pmessage == NULL) {
		ARISC_WRN("allocate message failed\n");
		return -ENOMEM;
	}

	/* initialize message */
	pmessage->type       = ARISC_AXP_DISABLE_IRQ;
	pmessage->state      = ARISC_MESSAGE_INITIALIZED;
	pmessage->cb.handler = NULL;
	pmessage->cb.arg     = NULL;

	/* send message use hwmsgbox */
	arisc_hwmsgbox_send_message(pmessage, ARISC_SEND_MSG_TIMEOUT);

	/* free message */
	result = pmessage->result;
	arisc_message_free(pmessage);

	return result;
}
EXPORT_SYMBOL(arisc_disable_nmi_irq);

int arisc_enable_nmi_irq(void)
{
	int                   result;
	struct arisc_message *pmessage;

	/* allocate a message frame */
	pmessage = arisc_message_allocate(ARISC_MESSAGE_ATTR_HARDSYN);
	if (pmessage == NULL) {
		ARISC_WRN("allocate message failed\n");
		return -ENOMEM;
	}

	/* initialize message */
	pmessage->type       = ARISC_AXP_ENABLE_IRQ;
	pmessage->state      = ARISC_MESSAGE_INITIALIZED;
	pmessage->cb.handler = NULL;
	pmessage->cb.arg     = NULL;

	/* send message use hwmsgbox */
	arisc_hwmsgbox_send_message(pmessage, ARISC_SEND_MSG_TIMEOUT);

	/* free message */
	result = pmessage->result;
	arisc_message_free(pmessage);

	return result;
}
EXPORT_SYMBOL(arisc_enable_nmi_irq);


int arisc_axp_get_chip_id(unsigned char *chip_id)
{
	int                   i;
	int                   result;
	struct arisc_message *pmessage;

	/* allocate a message frame */
	pmessage = arisc_message_allocate(ARISC_MESSAGE_ATTR_HARDSYN);
	if (pmessage == NULL) {
		ARISC_WRN("allocate message failed\n");
		return -ENOMEM;
	}

	/* initialize message */
	pmessage->type       = ARISC_AXP_GET_CHIP_ID;
	pmessage->state      = ARISC_MESSAGE_INITIALIZED;
	pmessage->cb.handler = NULL;
	pmessage->cb.arg     = NULL;

	memset((void *)pmessage->paras, 0, 16);

	/* send message use hwmsgbox */
	arisc_hwmsgbox_send_message(pmessage, ARISC_SEND_MSG_TIMEOUT);

	/* |paras[0]    |paras[1]    |paras[2]     |paras[3]      |
	 * |chip_id[0~3]|chip_id[4~7]|chip_id[8~11]|chip_id[12~15]|
	 */
	/* copy message readout data to user data buffer */
	for (i = 0; i < 4; i++) {
			chip_id[i] = (pmessage->paras[0] >> (i * 8)) & 0xff;
			chip_id[4 + i] = (pmessage->paras[1] >> (i * 8)) & 0xff;
			chip_id[8 + i] = (pmessage->paras[2] >> (i * 8)) & 0xff;
			chip_id[12 + i] = (pmessage->paras[3] >> (i * 8)) & 0xff;
	}

	/* free message */
	result = pmessage->result;
	arisc_message_free(pmessage);

	return result;
}
EXPORT_SYMBOL(arisc_axp_get_chip_id);

int arisc_set_led_bln(u32 led_rgb, u32 led_onms,
			u32 led_offms, u32 led_darkms)
{
	int result;
	struct arisc_message *pmessage;

	/* allocate a message frame */
	pmessage = arisc_message_allocate(ARISC_MESSAGE_ATTR_HARDSYN);
	if (pmessage == NULL) {
		ARISC_WRN("allocate message failed\n");
		return -ENOMEM;
	}

	/* initialize message */
	pmessage->type       = ARISC_SET_LED_BLN;
	pmessage->private    = (void *)0; /* set charge magic flag */
	pmessage->paras[0]   = led_rgb;
	pmessage->paras[1]   = led_onms;
	pmessage->paras[2]   = led_offms;
	pmessage->paras[3]   = led_darkms;

	/* send message use hwmsgbox */
	arisc_hwmsgbox_send_message(pmessage, ARISC_SEND_MSG_TIMEOUT);

	/* free message */
	result = pmessage->result;
	arisc_message_free(pmessage);

	return result;

}
EXPORT_SYMBOL(arisc_set_led_bln);

#if (defined CONFIG_ARCH_SUN8IW5P1)
int arisc_adjust_pmu_chgcur(unsigned int max_chgcur, unsigned int chg_ic_temp)
{
	int                   result;
	struct arisc_message *pmessage;

	/* allocate a message frame */
	pmessage = arisc_message_allocate(ARISC_MESSAGE_ATTR_HARDSYN);
	if (pmessage == NULL) {
		ARISC_WRN("allocate message failed\n");
		return -ENOMEM;
	}

	/* initialize message */
	pmessage->type       = ARISC_AXP_SET_PARAS;
	pmessage->private    = (void *)0x62; /* set charge current flag */
	pmessage->paras[0]   = chg_ic_temp;
	pmessage->paras[1]   = max_chgcur;
	pmessage->paras[2]   = 0;

	/* send message use hwmsgbox */
	arisc_hwmsgbox_send_message(pmessage, ARISC_SEND_MSG_TIMEOUT);

	/* free message */
	result = pmessage->result;
	arisc_message_free(pmessage);

	return result;
}
EXPORT_SYMBOL(arisc_adjust_pmu_chgcur);

int arisc_clear_nmi_status(void)
{
	int                   result;
	struct arisc_message *pmessage;

	/* allocate a message frame */
	pmessage = arisc_message_allocate(ARISC_MESSAGE_ATTR_HARDSYN);
	if (pmessage == NULL) {
		ARISC_WRN("allocate message failed\n");
		return -ENOMEM;
	}

	/* initialize message */
	pmessage->type       = ARISC_CLR_NMI_STATUS;
	pmessage->state      = ARISC_MESSAGE_INITIALIZED;
	pmessage->cb.handler = NULL;
	pmessage->cb.arg     = NULL;

	/* send message use hwmsgbox */
	arisc_hwmsgbox_send_message(pmessage, ARISC_SEND_MSG_TIMEOUT);

	/* free message */
	result = pmessage->result;
	arisc_message_free(pmessage);

	return result;
}
EXPORT_SYMBOL(arisc_clear_nmi_status);

int arisc_set_nmi_trigger(u32 type)
{
	int result;

	struct arisc_message *pmessage;

	/* allocate a message frame */
	pmessage = arisc_message_allocate(ARISC_MESSAGE_ATTR_HARDSYN);
	if (pmessage == NULL) {
		ARISC_WRN("allocate message failed\n");
		return -ENOMEM;
	}

	/* initialize message */
	pmessage->type       = ARISC_SET_NMI_TRIGGER;
	pmessage->state      = ARISC_MESSAGE_INITIALIZED;
	pmessage->cb.handler = NULL;
	pmessage->cb.arg     = NULL;
	pmessage->paras[0]   = type;
	/* send message use hwmsgbox */
	arisc_hwmsgbox_send_message(pmessage, ARISC_SEND_MSG_TIMEOUT);

	/* free message */
	result = pmessage->result;
	arisc_message_free(pmessage);

	return result;
}
#endif

int arisc_axp_int_notify(struct arisc_message *pmessage)
{
	u32 type = pmessage->paras[0];
	u32 ret = 0;

	if (type & NMI_INT_TYPE_PMU_OFFSET) {
		/* call pmu interrupt handler */
		if (nmi_isr_node[NMI_INT_TYPE_PMU].handler == NULL) {
			ARISC_WRN("pmu irq handler not install\n");
			return 1;
		}

		ARISC_INF("call pmu interrupt handler\n");
		ret |= (*(nmi_isr_node[NMI_INT_TYPE_PMU].handler))(nmi_isr_node[NMI_INT_TYPE_PMU].arg);
	}
	if (type & NMI_INT_TYPE_RTC_OFFSET) {
		/* call rtc interrupt handler */
		if (nmi_isr_node[NMI_INT_TYPE_RTC].handler == NULL) {
			ARISC_WRN("rtc irq handler not install\n");
			return 1;
		}

		ARISC_INF("call rtc interrupt handler\n");
		ret |= (*(nmi_isr_node[NMI_INT_TYPE_RTC].handler))(nmi_isr_node[NMI_INT_TYPE_RTC].arg);
	}

	return ret;
}

int arisc_pmu_set_voltage(u32 type, u32 voltage)
{
	int                   result;
	struct arisc_message *pmessage;

	/* allocate a message frame */
	pmessage = arisc_message_allocate(ARISC_MESSAGE_ATTR_HARDSYN);
	if (pmessage == NULL) {
		ARISC_WRN("allocate message failed\n");
		return -ENOMEM;
	}

	/* initialize message */
	pmessage->type       = ARISC_SET_PMU_VOLT;
	pmessage->state      = ARISC_MESSAGE_INITIALIZED;
	pmessage->cb.handler = NULL;
	pmessage->cb.arg     = NULL;
	pmessage->paras[0]   = type;
	pmessage->paras[1]   = voltage;

	/* send message use hwmsgbox */
	arisc_hwmsgbox_send_message(pmessage, ARISC_SEND_MSG_TIMEOUT);

	/* free message */
	result = pmessage->result;
	arisc_message_free(pmessage);

	return result;
}
EXPORT_SYMBOL(arisc_pmu_set_voltage);

unsigned int arisc_pmu_get_voltage(u32 type)
{
	u32                   voltage;
	struct arisc_message *pmessage;

	/* allocate a message frame */
	pmessage = arisc_message_allocate(ARISC_MESSAGE_ATTR_HARDSYN);
	if (pmessage == NULL) {
		ARISC_WRN("allocate message failed\n");
		return -ENOMEM;
	}

	/* initialize message */
	pmessage->type       = ARISC_GET_PMU_VOLT;
	pmessage->state      = ARISC_MESSAGE_INITIALIZED;
	pmessage->cb.handler = NULL;
	pmessage->cb.arg     = NULL;
	pmessage->paras[0]   = type;

	/* send message use hwmsgbox */
	arisc_hwmsgbox_send_message(pmessage, ARISC_SEND_MSG_TIMEOUT);
	voltage = pmessage->paras[1];

	/* free message */
	arisc_message_free(pmessage);

	return voltage;
}
EXPORT_SYMBOL(arisc_pmu_get_voltage);

#else
int arisc_disable_nmi_irq(void)
{
	int                   result;

	/* send message use hwmsgbox */
	result = invoke_scp_fn_smc(ARM_SVC_ARISC_AXP_DISABLE_IRQ, 0, 0, 0);

	return result;
}
EXPORT_SYMBOL(arisc_disable_nmi_irq);

int arisc_enable_nmi_irq(void)
{
	int result;

	/* send message use hwmsgbox */
	result = invoke_scp_fn_smc(ARM_SVC_ARISC_AXP_ENABLE_IRQ, 0, 0, 0);

	return result;
}
EXPORT_SYMBOL(arisc_enable_nmi_irq);

int arisc_clear_nmi_status(void)
{
	int result;

	/* send message use hwmsgbox */
	result = invoke_scp_fn_smc(ARM_SVC_ARISC_CLR_NMI_STATUS, 0, 0, 0);

	return result;
}
EXPORT_SYMBOL(arisc_clear_nmi_status);

int arisc_set_nmi_trigger(u32 type)
{
	int result;

	/* send message use hwmsgbox */
	result = invoke_scp_fn_smc(ARM_SVC_ARISC_SET_NMI_TRIGGER, type, 0, 0);

	return result;
}
EXPORT_SYMBOL(arisc_set_nmi_trigger);

/*
 * sunxi-nmi.c NMI driver
 *
 * Copyright (C) 2014-2015 allwinner.
 *	Ming Li<liming@allwinnertech.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/slab.h>
#include <linux/arisc/arisc.h>
#include <linux/io.h>
#include "sunxi-nmi.h"

static u32 debug_mask = 0x0;
static nmi_struct *nmi_data;

void clear_nmi_status(void)
{
	arisc_clear_nmi_status();

	return;
}
EXPORT_SYMBOL(clear_nmi_status);


void enable_nmi(void)
{
	arisc_enable_nmi_irq();

	return;
}
EXPORT_SYMBOL(enable_nmi);

void disable_nmi(void)
{
	arisc_disable_nmi_irq();

	return;
}
EXPORT_SYMBOL(disable_nmi);

void set_nmi_trigger(u32 trigger)
{
	u32 tmp = 0;

	if (IRQF_TRIGGER_LOW == trigger)
		tmp = NMI_IRQ_LOW_LEVEL;
	else if (IRQF_TRIGGER_FALLING == trigger)
		tmp = NMI_IRQ_NE_EDGE;
	else if (IRQF_TRIGGER_HIGH == trigger)
		tmp = NMI_IRQ_HIGH_LEVEL;
	else if (IRQF_TRIGGER_RISING == trigger)
		tmp = NMI_IRQ_PO_EDGE;

	arisc_set_nmi_trigger(tmp);

	return;
}
EXPORT_SYMBOL(set_nmi_trigger);

static int sunxi_nmi_probe(struct platform_device *pdev)
{
	struct device_node *node = pdev->dev.of_node;
	struct resource *mem_res = NULL;
	s32 ret;

	if (!of_device_is_available(node)) {
		printk("%s: nmi status disable!!\n", __func__);
		return -EPERM;
	}

	nmi_data = kzalloc(sizeof(*nmi_data), GFP_KERNEL);
	if (nmi_data == NULL) {
		ret = -ENOMEM;
		return ret;
	}

	mem_res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (mem_res == NULL) {
		printk(KERN_ERR "%s: failed to get MEM res\n", __func__);
		ret = -ENXIO;
		goto mem_io_err;
	}

	if (!request_mem_region(mem_res->start, resource_size(mem_res), mem_res->name)) {
		printk(KERN_ERR  "%s: failed to request mem region\n", __func__);
		ret = -EINVAL;
		goto mem_io_err;
	}

	nmi_data->base_addr = ioremap(mem_res->start, resource_size(mem_res));
	if (!nmi_data->base_addr) {
		printk(KERN_ERR  "%s: failed to io remap\n", __func__);
		ret = -EIO;
		goto mem_io_err;
	}

	if (of_property_read_u32(node, "nmi_irq_ctrl", &nmi_data->nmi_irq_ctrl))
		nmi_data->nmi_irq_ctrl = 0xffffffff;

	if (of_property_read_u32(node, "nmi_irq_en", &nmi_data->nmi_irq_en))
		nmi_data->nmi_irq_en = 0xffffffff;

	if (of_property_read_u32(node, "nmi_irq_status", &nmi_data->nmi_irq_status))
		nmi_data->nmi_irq_status = 0xffffffff;

	if (of_property_read_u32(node, "nmi_irq_mask", &nmi_data->nmi_irq_mask))
		nmi_data->nmi_irq_mask = 0xffffffff;

	return 0;

mem_io_err:
	kfree(nmi_data);

	return ret;
}

static int sunxi_nmi_remove(struct platform_device *pdev)
{
	printk(KERN_INFO "%s: module unloaded\n", __func__);

	return 0;
}


static const struct of_device_id sunxi_nmi_match[] = {
	 { .compatible = "allwinner,sunxi-nmi", },
	 {},
};
MODULE_DEVICE_TABLE(of, sunxi_nmi_match);

static struct platform_driver nmi_platform_driver = {
	.probe  = sunxi_nmi_probe,
	.remove = sunxi_nmi_remove,
	.driver = {
		.name	= NMI_MODULE_NAME,
		.owner = THIS_MODULE,
		.of_match_table = sunxi_nmi_match,
	},
};

static int __init sunxi_nmi_init(void)
{
	return platform_driver_register(&nmi_platform_driver);
}

static void __exit sunxi_nmi_exit(void)
{
	platform_driver_unregister(&nmi_platform_driver);
}

arch_initcall(sunxi_nmi_init);
module_exit(sunxi_nmi_exit);
module_param_named(debug_mask, debug_mask, int, 0644);
MODULE_DESCRIPTION("sunxi nmi driver");
MODULE_AUTHOR("Ming Li<liming@allwinnertech.com>");
MODULE_LICENSE("GPL");

int arisc_axp_get_chip_id(unsigned char *chip_id)
{
	int result;

	/* FIXME: if the runtime sever enable the mmu & dcache,
	 * should not use flush cache here.
	 */
	result = invoke_scp_fn_smc(ARM_SVC_ARISC_AXP_GET_CHIP_ID,
			virt_to_phys(chip_id), 0, 0);

	return result;
}
EXPORT_SYMBOL(arisc_axp_get_chip_id);

int arisc_set_led_bln(u32 led_rgb, u32 led_onms, u32 led_offms, u32 led_darkms)
{
	int result;
	u32 paras[22];

	paras[0] = led_rgb;
	paras[1] = led_onms;
	paras[2] = led_offms;
	paras[3] = led_darkms;

	/* FIXME: if the runtime sever enable the mmu & dcache,
	 * should not use flush cache here.
	 */
	result = invoke_scp_fn_smc(ARM_SVC_ARISC_SET_LED_BLN,
			virt_to_phys(paras), 0, 0);

	return result;

}
EXPORT_SYMBOL(arisc_set_led_bln);

#if (defined CONFIG_ARCH_SUN50IW1P1) || \
	(defined CONFIG_ARCH_SUN50IW2P1)
int arisc_adjust_pmu_chgcur(unsigned int max_chgcur, unsigned int chg_ic_temp)
{
	int result;

	result = invoke_scp_fn_smc(ARM_SVC_ARISC_AXP_SET_PARAS, max_chgcur,
			chg_ic_temp, 1);

	return result;
}
EXPORT_SYMBOL(arisc_adjust_pmu_chgcur);
#endif

int arisc_set_pwr_tree(u32 *pwr_tree)
{
	int result;

	result = invoke_scp_fn_smc(ARM_SVC_ARISC_SET_PWR_TREE,
			virt_to_phys(pwr_tree), 0, 0);

	return result;
}
EXPORT_SYMBOL(arisc_set_pwr_tree);


int arisc_axp_int_notify(struct arisc_message *pmessage)
{
	u32 type = pmessage->paras[0];
	u32 ret = 0;

	if (type & NMI_INT_TYPE_PMU_OFFSET) {
		/* call pmu interrupt handler */
		if (nmi_isr_node[NMI_INT_TYPE_PMU].handler == NULL) {
			ARISC_WRN("pmu irq handler not install\n");
			return 1;
		}

		ARISC_INF("call pmu interrupt handler\n");
		ret |= (*(nmi_isr_node[NMI_INT_TYPE_PMU].handler))(nmi_isr_node[NMI_INT_TYPE_PMU].arg);
	}
	if (type & NMI_INT_TYPE_RTC_OFFSET)
	{
		/* call rtc interrupt handler */
		if (nmi_isr_node[NMI_INT_TYPE_RTC].handler == NULL) {
			ARISC_WRN("rtc irq handler not install\n");
			return 1;
		}

		ARISC_INF("call rtc interrupt handler\n");
		ret |= (*(nmi_isr_node[NMI_INT_TYPE_RTC].handler))(nmi_isr_node[NMI_INT_TYPE_RTC].arg);
	}

	return ret;
}

int arisc_pmu_set_voltage(u32 type, u32 voltage)
{
	int result;

	result = invoke_scp_fn_smc(ARM_SVC_ARISC_SET_PMU_VOLT, type, voltage, 0);

	return result;
}
EXPORT_SYMBOL(arisc_pmu_set_voltage);

unsigned int arisc_pmu_get_voltage(u32 type)
{
	u32 voltage;

	invoke_scp_fn_smc(ARM_SVC_ARISC_GET_PMU_VOLT, type,
			virt_to_phys(&voltage), 0);

	return voltage;
}
EXPORT_SYMBOL(arisc_pmu_get_voltage);

int arisc_pmu_set_voltage_state(u32 type, u32 state)
{
	int result;

	result = invoke_scp_fn_smc(ARM_SVC_ARISC_SET_PMU_VOLT_STA, type, state, 0);

	return result;
}
EXPORT_SYMBOL(arisc_pmu_set_voltage_state);

unsigned int arisc_pmu_get_voltage_state(u32 type)
{
	u32 state;

	invoke_scp_fn_smc(ARM_SVC_ARISC_GET_PMU_VOLT_STA, type,
			virt_to_phys(&state), 0);

	return state;
}
EXPORT_SYMBOL(arisc_pmu_get_voltage_state);


#endif
