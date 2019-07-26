LOCAL_DIR := $(GET_LOCAL_DIR)

MODULES += \
	lib/bio \
	lib/fs \
	lib/partition \
	app/aboot

OBJS += \
	$(LOCAL_DIR)/gfx.o \
	$(LOCAL_DIR)/reboot.o
