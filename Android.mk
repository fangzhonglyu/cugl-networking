###########################
#
# CUGL static library
#
###########################
LOCAL_PATH := $(call my-dir)
CUGL_PATH  := $(LOCAL_PATH)
include $(CLEAR_VARS)

LOCAL_MODULE := CUGL

LOCAL_C_INCLUDES := $(CUGL_PATH)/include
LOCAL_C_INCLUDES += $(CUGL_PATH)/sdl2/include
LOCAL_C_INCLUDES += $(CUGL_PATH)/thirdparty/geomtools/include

# Add your application source files here...
LOCAL_PATH := $(CUGL_PATH)

LOCAL_SRC_FILES := \
	$(subst $(LOCAL_PATH)/,, \
	$(wildcard $(CUGL_PATH)/source/base/*.cpp) \
	$(wildcard $(CUGL_PATH)/source/util/*.cpp) \
	$(wildcard $(CUGL_PATH)/source/math/*.cpp) \
	$(wildcard $(CUGL_PATH)/source/math/*.c) \
	$(wildcard $(CUGL_PATH)/source/math/polygon/*.cpp) \
	$(wildcard $(CUGL_PATH)/source/input/*.cpp) \
	$(wildcard $(CUGL_PATH)/source/io/*.cpp) \
	$(wildcard $(CUGL_PATH)/source/render/*.cpp) \
	$(wildcard $(CUGL_PATH)/source/assets/*.cpp) \
	$(wildcard $(CUGL_PATH)/source/scene2/*.cpp) \
	$(wildcard $(CUGL_PATH)/source/scene2/graph/*.cpp) \
	$(wildcard $(CUGL_PATH)/source/scene2/ui/*.cpp) \
	$(wildcard $(CUGL_PATH)/source/scene2/layout/*.cpp) \
	$(wildcard $(CUGL_PATH)/source/external/cJSON/*.c))

LOCAL_CFLAGS += -DGL_GLEXT_PROTOTYPES
LOCAL_CFLAGS += \
	-Wdocumentation \
	-Wdocumentation-unknown-command \
	-Wmissing-prototypes \
	-Wunreachable-code-break \
	-Wunneeded-internal-declaration \
	-Wno-implicit-const-int-float-conversion \

LOCAL_EXPORT_LDLIBS := ldl -lGLESv1_CM -lGLESv2 -lGLESv3 -lOpenSLES -llog -landroid -latomic

LOCAL_STATIC_LIBRARIES := cpufeatures

include $(BUILD_STATIC_LIBRARY)

$(call import-module,android/cpufeatures)
