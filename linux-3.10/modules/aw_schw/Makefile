
################################################################################
#
# Common Variables that already set:
#     LICHEE_KDIR
#     LICHEE_MOD_DIR
#     CROSS_COMPILE
#     ARCH
#
#################################################################################
PWD=$(shell pwd)
obj-m+=sunxi_schw.o

sunxi_schw-objs := sunxi_schw_base.o libschw.a
#sunxi_schw-objs := sunxi_schw_base.o aw_schw_lib.o

install:build
	cp sunxi_schw.ko $(LICHEE_MOD_DIR)/
build:libschw.a
#build:
	@echo $(LICHEE_KDIR)
	$(MAKE) -C $(LICHEE_KDIR) M=$(PWD)
libschw.a:libschw
ifeq ($(LICHEE_ARCH),arm64)
	cp libschw libschw.a
else
	cp libschw_32 libschw.a
endif
clean:
	@rm -rf *.o *.ko .*.cmd *.mod.c *.order *.symvers .tmp_versions *~ *.a
