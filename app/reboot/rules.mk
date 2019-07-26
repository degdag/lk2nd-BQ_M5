LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += -I$(LK_TOP_DIR)/platform/msm_shared/include -I$(LK_TOP_DIR)/lib/zlib_inflate

MODULES += lib/zlib_inflate lib/fs lib/bio lib/partition app/reboot/aboot

OBJS += \
	$(LOCAL_DIR)/reboot.o

