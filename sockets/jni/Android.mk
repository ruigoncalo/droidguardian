LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := dg_client
LOCAL_SRC_FILES := dg_client.c

# executable insted of library
include $(BUILD_EXECUTABLE)
