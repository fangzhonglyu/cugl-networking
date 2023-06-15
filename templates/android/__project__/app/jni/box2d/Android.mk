# WINDOWS FRIENDLY MAKEFILE (Long Paths are not okay)
# Need to invoke subprojects directly
LOCAL_PATH := $(call my-dir)
CUGL_PATH  := $(LOCAL_PATH)/__CUGL_PATH__
BOX2D_PATH := $(CUGL_PATH)/thirdparty/box2d

###########################
#
# Box2D static library
#
###########################
include $(CLEAR_VARS)

LOCAL_MODULE := box2d

LOCAL_C_INCLUDES := $(BOX2D_PATH)/include
LOCAL_C_INCLUDES += $(BOX2D_PATH)/src
LOCAL_EXPORT_C_INCLUDES += $(LOCAL_C_INCLUDES)

# To keep file mangling to a minimum
LOCAL_PATH = $(BOX2D_PATH)

# CODEC SOURCE CODE
LOCAL_SRC_FILES := \
	$(subst $(LOCAL_PATH)/,, \
	$(wildcard $(BOX2D_PATH)/src/collision/*.cpp) \
	$(wildcard $(BOX2D_PATH)/src/common/*.cpp) \
	$(wildcard $(BOX2D_PATH)/src/dynamics/*.cpp) \
	$(wildcard $(BOX2D_PATH)/src/rope/*.cpp) \
	)
LOCAL_STATIC_LIBRARIES :=

include $(BUILD_STATIC_LIBRARY)
