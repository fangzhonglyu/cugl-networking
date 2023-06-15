# WINDOWS FRIENDLY MAKEFILE (Long Paths are not okay)
# Need to invoke subprojects directly
LOCAL_PATH := $(call my-dir)
CUGL_PATH := $(LOCAL_PATH)/__CUGL_PATH__
WEBRTC_MAKE := $(CUGL_PATH)/thirdparty/libdatachannel/Android.mk
OPENSSL_MAKE := $(CUGL_PATH)/thirdparty/libdatachannel/deps/openssl/Android.mk
USRSCTP_MAKE := $(CUGL_PATH)/thirdparty/libdatachannel/deps/usrsctp/Android.mk
LIBSRTP_MAKE := $(CUGL_PATH)/thirdparty/libdatachannel/deps/libsrtp/Android.mk
LIBJUICE_MAKE := $(CUGL_PATH)/thirdparty/libdatachannel/deps/libjuice/Android.mk
include $(OPENSSL_MAKE)
include $(USRSCTP_MAKE)
include $(LIBSRTP_MAKE)
include $(LIBJUICE_MAKE)
include $(WEBRTC_MAKE)
