LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE:= UninstallMonitor
LOCAL_SRC_FILES:= Monitor.cc jni_main.cc
LOCAL_C_INCLUDES:= $(LOCAL_PATH)/includes
LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib -llog
# include $(BUILD_SHARED_LIBRARY)
include $(BUILD_EXECUTABLE) 

ifeq ($(VAR_MODE), debug)
	LOCAL_CFLAGS += -DDEBUG
endif
	LOCAL_CFLAGS += -DRELEASE