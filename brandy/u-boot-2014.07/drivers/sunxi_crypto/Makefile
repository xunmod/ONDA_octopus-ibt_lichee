#
# (C) Copyright 2013-2016
# Allwinner Technology Co., Ltd. <www.allwinnertech.com>
#
# SPDX-License-Identifier:     GPL-2.0+
#
ifdef CONFIG_ARCH_SUN8IW6P1
	obj-$(CONFIG_SUNXI_SECURE_SYSTEM)	+= sunxi_crypto_legacy.o
else
	obj-$(CONFIG_SUNXI_SECURE_SYSTEM)	+= sunxi_crypto.o
endif
obj-$(CONFIG_SUNXI_SECURE_SYSTEM)	+= ss_op.o
obj-$(CONFIG_SUNXI_SECURE_SYSTEM)	+= sunxi_efuse.o
obj-$(CONFIG_BURN_NORMAL_EFUSE)	+= efuse_normal.o
