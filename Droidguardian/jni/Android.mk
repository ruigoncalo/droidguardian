 LOCAL_PATH := $(call my-dir)
 
include $(CLEAR_VARS)
       
LOCAL_CFLAGS    := -DHAVE_INTTYPES_H
LOCAL_LDLIBS :=  -llog
LOCAL_MODULE    := dgquery
LOCAL_SRC_FILES := sockets.c

include $(BUILD_SHARED_LIBRARY)