LOCAL_PATH := $(call my-dir)
CLIPR_PATH := $(LOCAL_PATH)

###########################
#
# clipper static library
#
###########################
include $(CLEAR_VARS)

LOCAL_MODULE := polyclipping

LOCAL_C_INCLUDES := $(CLIPR_PATH)
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)

# To keep file mangling to a minimum
LOCAL_PATH := $(CLIPR_PATH)

LOCAL_SRC_FILES := clipper.cpp

LOCAL_CFLAGS += -Wall -Wextra -Wpedantic -Werror -Wno-deprecated-copy

include $(BUILD_STATIC_LIBRARY)