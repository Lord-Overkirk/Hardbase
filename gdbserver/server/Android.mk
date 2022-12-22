LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := gdbserver
LOCAL_C_INCLUDES += \
		../tools/
LOCAL_SRC_FILES := \
		gdbserver.c \
		../tools/process.c
LOCAL_CFLAGS := -Wall -Wextra
include $(BUILD_EXECUTABLE)