# Indirect makefile for SDL2
LOCAL_PATH := $(call my-dir)
SDL2_PATH := $(LOCAL_PATH)/__SDL2_PATH__
SDL2_MAKE := $(SDL2_PATH)/Android.mk
include $(SDL2_MAKE)
