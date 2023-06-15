LOCAL_PATH := $(call my-dir)
POLY2_PATH := $(LOCAL_PATH)/../../../../poly2tri

###########################
#
# poly2Tri static library
#
###########################
include $(CLEAR_VARS)

LOCAL_MODULE := poly2tri

LOCAL_C_INCLUDES := $(POLY2_PATH)
LOCAL_EXPORT_C_INCLUDES += $(LOCAL_C_INCLUDES)

# To keep file mangling to a minimum
LOCAL_PATH = $(POLY2_PATH)

# CODEC SOURCE CODE
LOCAL_SRC_FILES := \
	$(subst $(LOCAL_PATH)/,, \
	$(wildcard $(POLY2_PATH)/poly2tri/common/*.cc) \
	$(wildcard $(POLY2_PATH)/poly2tri/sweep/*.cc) \
	)
LOCAL_STATIC_LIBRARIES :=

include $(BUILD_STATIC_LIBRARY)
