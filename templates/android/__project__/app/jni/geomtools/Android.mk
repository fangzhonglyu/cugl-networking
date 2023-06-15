# WINDOWS FRIENDLY MAKEFILE (Long Paths are not okay)
# Need to invoke subprojects directly
LOCAL_PATH := $(call my-dir)
CUGL_PATH := $(LOCAL_PATH)/__CUGL_PATH__
POLYTRI_MAKE := $(CUGL_PATH)/thirdparty/geomtools/poly2tri/Android.mk
CLIPPER_MAKE := $(CUGL_PATH)/thirdparty/geomtools/clipper/Android.mk
include $(POLYTRI_MAKE)
include $(CLIPPER_MAKE)