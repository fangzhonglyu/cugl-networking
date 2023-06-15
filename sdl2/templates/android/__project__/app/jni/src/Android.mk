########################
#
# Main Application Entry
#
########################
LOCAL_PATH := $(call my-dir)
SDL2_PATH := $(LOCAL_PATH)/__SDL2_PATH__
PROJ_PATH := $(LOCAL_PATH)/__SOURCE_PATH__
include $(CLEAR_VARS)

LOCAL_MODULE := main
LOCAL_C_INCLUDES := $(SDL2_PATH)/include
__EXTRA_INCLUDES__

# Add your application source files here.
LOCAL_PATH = $(PROJ_PATH)
LOCAL_SRC_FILES := $(subst $(LOCAL_PATH)/,,__SOURCE_FILES__)

# Link in SDL2
LOCAL_SHARED_LIBRARIES := SDL2
LOCAL_SHARED_LIBRARIES += SDL2_image
LOCAL_SHARED_LIBRARIES += SDL2_ttf
LOCAL_SHARED_LIBRARIES += SDL2_codec

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -lGLESv3 -lOpenSLES -llog -landroid
include $(BUILD_SHARED_LIBRARY)
