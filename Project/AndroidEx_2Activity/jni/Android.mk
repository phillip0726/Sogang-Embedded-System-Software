LOCAL_PATH:=$(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE:=fnd_driver
LOCAL_SRC_FILES:=driver.c
LOCAL_LDLIBS := -llog

include $(BUILD_SHARED_LIBRARY)

