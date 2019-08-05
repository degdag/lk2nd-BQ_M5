LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += -I$(LK_TOP_DIR)/platform/msm_shared/include -I$(LK_TOP_DIR)/lib/zlib_inflate

MODULES += lib/zlib_inflate lib/fs lib/bio lib/partition app/reboot/aboot

OBJS += \
	$(LOCAL_DIR)/reboot.o \
	$(LOCAL_DIR)/boot.o \
	$(LOCAL_DIR)/config.o \
	$(LOCAL_DIR)/menu.o \
	$(LOCAL_DIR)/fs_util.o

