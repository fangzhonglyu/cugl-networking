########################
#
# Main Application Entry
#
########################
LOCAL_PATH := $(call my-dir)
CUGL_PATH  := $(LOCAL_PATH)/__CUGL_PATH__
PROJ_PATH  := $(LOCAL_PATH)/__SOURCE_PATH__
include $(CLEAR_VARS)

LOCAL_MODULE := main
LOCAL_C_INCLUDES := $(CUGL_PATH)/include
LOCAL_C_INCLUDES += $(CUGL_PATH)/sdl2/include
LOCAL_C_INCLUDES += $(CUGL_PATH)/thirdparty/box2d/include
LOCAL_C_INCLUDES += $(CUGL_PATH)/thirdparty/geomtools/include
LOCAL_C_INCLUDES += $(CUGL_PATH)/thirdparty/libdatachannel/include
__EXTRA_INCLUDES__

# Add your application source files here.
LOCAL_PATH = $(PROJ_PATH)
LOCAL_SRC_FILES := $(subst $(LOCAL_PATH)/,,__SOURCE_FILES__)

# Link in SDL2
LOCAL_SHARED_LIBRARIES := SDL2
LOCAL_SHARED_LIBRARIES += SDL2_image
LOCAL_SHARED_LIBRARIES += SDL2_ttf
LOCAL_SHARED_LIBRARIES += SDL2_codec
LOCAL_SHARED_LIBRARIES += datachannel

LOCAL_STATIC_LIBRARIES := CUGL
LOCAL_STATIC_LIBRARIES += box2d
LOCAL_STATIC_LIBRARIES += poly2tri
LOCAL_STATIC_LIBRARIES += polyclipping

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -lGLESv3 -lOpenSLES -llog -landroid
include $(BUILD_SHARED_LIBRARY)
