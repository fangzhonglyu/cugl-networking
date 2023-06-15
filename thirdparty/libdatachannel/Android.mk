########################
#
# libdatachannel shared library
#
########################
LOCAL_PATH := $(call my-dir)
PROJ_PATH := $(LOCAL_PATH)
OSSL_PATH := $(LOCAL_PATH)/deps/openssl
include $(CLEAR_VARS)

LOCAL_MODULE := datachannel
LOCAL_C_INCLUDES := $(PROJ_PATH)/include
LOCAL_C_INCLUDES += $(PROJ_PATH)/include/rtc
LOCAL_C_INCLUDES += $(PROJ_PATH)/src
LOCAL_C_INCLUDES += $(PROJ_PATH)/deps/usrsctp/usrsctplib
LOCAL_C_INCLUDES += $(PROJ_PATH)/deps/plog/include
LOCAL_C_INCLUDES += $(PROJ_PATH)/deps/libsrtp/crypto/include
LOCAL_C_INCLUDES += $(PROJ_PATH)/deps/libsrtp/include
LOCAL_C_INCLUDES += $(PROJ_PATH)/deps/libjuice/include
LOCAL_C_INCLUDES += $(OSSL_PATH)
LOCAL_C_INCLUDES += $(OSSL_PATH)/include
LOCAL_C_INCLUDES += $(OSSL_PATH)/android/$(TARGET_ARCH_ABI)/include

# The defines
LOCAL_CFLAGS += -DRTC_EXPORTS
LOCAL_CFLAGS += -DRTC_STATIC
LOCAL_CFLAGS += -DRTC_ENABLE_WEBSOCKET=1
LOCAL_CFLAGS += -DRTC_ENABLE_MEDIA=1
LOCAL_CFLAGS += -DRTC_SYSTEM_SRTP=0
LOCAL_CFLAGS += -DUSE_GNUTLS=0
LOCAL_CFLAGS += -DUSE_NICE=0
LOCAL_CFLAGS += -DRTC_SYSTEM_JUICE=0
LOCAL_CFLAGS += -DJUICE_STATIC

# Compilation flags
LOCAL_CFLAGS += -fPIC
LOCAL_CFLAGS += -fno-limit-debug-info
LOCAL_CFLAGS += -fvisibility=default
LOCAL_CFLAGS += -Wall
LOCAL_CFLAGS += -Wextra
LOCAL_CFLAGS += -pthread
LOCAL_CFLAGS += -std=gnu++17

# Add your application source files here.
LOCAL_PATH = $(PROJ_PATH)
LOCAL_SRC_FILES := $(subst $(LOCAL_PATH)/,, \
	$(LOCAL_PATH)/src/candidate.cpp \
	$(LOCAL_PATH)/src/channel.cpp \
	$(LOCAL_PATH)/src/configuration.cpp \
	$(LOCAL_PATH)/src/datachannel.cpp \
	$(LOCAL_PATH)/src/description.cpp \
	$(LOCAL_PATH)/src/global.cpp \
	$(LOCAL_PATH)/src/message.cpp \
	$(LOCAL_PATH)/src/peerconnection.cpp \
	$(LOCAL_PATH)/src/rtcpreceivingsession.cpp \
	$(LOCAL_PATH)/src/track.cpp \
	$(LOCAL_PATH)/src/websocket.cpp \
	$(LOCAL_PATH)/src/websocketserver.cpp \
	$(LOCAL_PATH)/src/rtppacketizationconfig.cpp \
	$(LOCAL_PATH)/src/rtcpsrreporter.cpp \
	$(LOCAL_PATH)/src/rtppacketizer.cpp \
	$(LOCAL_PATH)/src/opusrtppacketizer.cpp \
	$(LOCAL_PATH)/src/opuspacketizationhandler.cpp \
	$(LOCAL_PATH)/src/h264rtppacketizer.cpp \
	$(LOCAL_PATH)/src/nalunit.cpp \
	$(LOCAL_PATH)/src/h264packetizationhandler.cpp \
	$(LOCAL_PATH)/src/mediachainablehandler.cpp \
	$(LOCAL_PATH)/src/mediahandlerelement.cpp \
	$(LOCAL_PATH)/src/mediahandlerrootelement.cpp \
	$(LOCAL_PATH)/src/rtcpnackresponder.cpp \
	$(LOCAL_PATH)/src/rtp.cpp \
	$(LOCAL_PATH)/src/capi.cpp \
	$(LOCAL_PATH)/src/impl/certificate.cpp \
	$(LOCAL_PATH)/src/impl/channel.cpp \
	$(LOCAL_PATH)/src/impl/datachannel.cpp \
	$(LOCAL_PATH)/src/impl/dtlssrtptransport.cpp \
	$(LOCAL_PATH)/src/impl/dtlstransport.cpp \
	$(LOCAL_PATH)/src/impl/icetransport.cpp \
	$(LOCAL_PATH)/src/impl/init.cpp \
	$(LOCAL_PATH)/src/impl/peerconnection.cpp \
	$(LOCAL_PATH)/src/impl/logcounter.cpp \
	$(LOCAL_PATH)/src/impl/sctptransport.cpp \
	$(LOCAL_PATH)/src/impl/threadpool.cpp \
	$(LOCAL_PATH)/src/impl/tls.cpp \
	$(LOCAL_PATH)/src/impl/track.cpp \
	$(LOCAL_PATH)/src/impl/utils.cpp \
	$(LOCAL_PATH)/src/impl/processor.cpp \
	$(LOCAL_PATH)/src/impl/sha.cpp \
	$(LOCAL_PATH)/src/impl/pollinterrupter.cpp \
	$(LOCAL_PATH)/src/impl/pollservice.cpp \
	$(LOCAL_PATH)/src/impl/tcpserver.cpp \
	$(LOCAL_PATH)/src/impl/tcptransport.cpp \
	$(LOCAL_PATH)/src/impl/tlstransport.cpp \
	$(LOCAL_PATH)/src/impl/transport.cpp \
	$(LOCAL_PATH)/src/impl/verifiedtlstransport.cpp \
	$(LOCAL_PATH)/src/impl/websocket.cpp \
	$(LOCAL_PATH)/src/impl/websocketserver.cpp \
	$(LOCAL_PATH)/src/impl/wstransport.cpp \
	$(LOCAL_PATH)/src/impl/wshandshake.cpp)

# Pull in all the dependencies
LOCAL_STATIC_LIBRARIES := crypto
LOCAL_STATIC_LIBRARIES += ssl
LOCAL_STATIC_LIBRARIES += usrsctp
LOCAL_STATIC_LIBRARIES += srtp2
LOCAL_STATIC_LIBRARIES += juice

include $(BUILD_SHARED_LIBRARY)
