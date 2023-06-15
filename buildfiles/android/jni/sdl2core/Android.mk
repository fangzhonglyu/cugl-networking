# WINDOWS FRIENDLY MAKEFILE (Long Paths are not okay)
# Need to invoke subprojects directly
LOCAL_PATH := $(call my-dir)
CUGL_PATH  := $(LOCAL_PATH)/../../../..
SDL2_MAKE  := $(CUGL_PATH)/sdl2/Android.mk
IMAGE_MAKE := $(CUGL_PATH)/sdl2/extensions/image/Android.mk
SDTTF_MAKE := $(CUGL_PATH)/sdl2/extensions/ttf/Android.mk
CODEC_MAKE := $(CUGL_PATH)/sdl2/extensions/codec/Android.mk
include $(SDL2_MAKE)
include $(IMAGE_MAKE)
include $(SDTTF_MAKE)
include $(CODEC_MAKE)