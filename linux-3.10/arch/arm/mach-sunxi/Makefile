obj-$(CONFIG_ARCH_SUN8I)  += sun8i.o
obj-$(CONFIG_ARCH_SUN50I) += sun50i.o
obj-$(CONFIG_SUNXI_BOOTUP_EXTEND) += sunxi_bootup_extend.o

ifeq ($(CONFIG_ARCH_SUN8IW6),y)
obj-$(CONFIG_SMP)         += sun8i-platsmp.o sunxi-headsmp.o sunxi-platsmp.o
obj-$(CONFIG_HOTPLUG_CPU) += sunxi-hotplug.o
obj-$(CONFIG_SUN8I_CCI)   += sun8i-cci.o
obj-$(CONFIG_MCPM)        += sun8i-setup.o
ifeq ($(CONFIG_SUNXI_TRUSTZONE), y)
obj-$(CONFIG_SUNXI_TRUSTZONE) += firmware.o
obj-$(CONFIG_SUNXI_TRUSTZONE) += sunxi-smc.o
endif

ifdef CONFIG_SUNXI_GIC_ACCESS_SS
obj-$(CONFIG_MCPM)        += sun8i-mcpm.o
obj-y                     += sun8i-gic.o
obj-y                     += sun8i-gicss.o
else
obj-$(CONFIG_MCPM)        += sun8iw6-mcpm.o
obj-y                     += sun8iw6-gicss.o
endif

else
ifeq ($(CONFIG_SMP), y)
ifneq ($(CONFIG_ARM_PSCI), y)
obj-y += headsmp.o platsmp.o
obj-$(CONFIG_HOTPLUG_CPU) += hotplug.o
endif
endif
endif
