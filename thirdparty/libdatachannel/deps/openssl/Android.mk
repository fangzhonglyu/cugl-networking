LOCAL_PATH := $(call my-dir)
PROJ_PATH := $(LOCAL_PATH)

########################
#
# crypto static library
#
########################
include $(CLEAR_VARS)

LOCAL_MODULE := crypto
LOCAL_C_INCLUDES := $(PROJ_PATH)
LOCAL_C_INCLUDES := $(PROJ_PATH)/include
LOCAL_C_INCLUDES += $(PROJ_PATH)/android/$(TARGET_ARCH_ABI)/crypto
LOCAL_C_INCLUDES += $(PROJ_PATH)/android/$(TARGET_ARCH_ABI)/include
LOCAL_C_INCLUDES += $(PROJ_PATH)/crypto/ec/curve448/arch_32
LOCAL_C_INCLUDES += $(PROJ_PATH)/crypto/ec/curve448
LOCAL_C_INCLUDES += $(PROJ_PATH)/crypto/modes

# The defines
LOCAL_CFLAGS += -DENGINESDIR=\"/usr/local/engines-1.1\" 
LOCAL_CFLAGS += -DOPENSSLDIR=\"/usr/local/ssl\" 
LOCAL_CFLAGS += -DOPENSSL_NO_ASM 
LOCAL_CFLAGS += -DOPENSSL_NO_STATIC_ENGINE 

# Compilation flags
LOCAL_CFLAGS += -fPIC 
LOCAL_CFLAGS += -fno-limit-debug-info 

# Add your application source files here.
LOCAL_PATH = $(PROJ_PATH)
LOCAL_SRC_FILES := $(subst $(LOCAL_PATH)/,, \
	$(LOCAL_PATH)/crypto/aes/aes_cbc.c \
	$(LOCAL_PATH)/crypto/aes/aes_cfb.c \
	$(LOCAL_PATH)/crypto/aes/aes_core.c \
	$(LOCAL_PATH)/crypto/aes/aes_ecb.c \
	$(LOCAL_PATH)/crypto/aes/aes_ige.c \
	$(LOCAL_PATH)/crypto/aes/aes_misc.c \
	$(LOCAL_PATH)/crypto/aes/aes_ofb.c \
	$(LOCAL_PATH)/crypto/aes/aes_wrap.c \
	$(wildcard $(LOCAL_PATH)/crypto/aria/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/asn1/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/async/arch/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/async/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/bf/*.c) \
	$(LOCAL_PATH)/crypto/bio/b_addr.c \
	$(LOCAL_PATH)/crypto/bio/b_dump.c \
	$(LOCAL_PATH)/crypto/bio/b_print.c \
	$(LOCAL_PATH)/crypto/bio/b_sock.c \
	$(LOCAL_PATH)/crypto/bio/b_sock2.c \
	$(LOCAL_PATH)/crypto/bio/bf_buff.c \
	$(LOCAL_PATH)/crypto/bio/bf_nbio.c \
	$(LOCAL_PATH)/crypto/bio/bf_null.c \
	$(LOCAL_PATH)/crypto/bio/bio_cb.c \
	$(LOCAL_PATH)/crypto/bio/bio_err.c \
	$(LOCAL_PATH)/crypto/bio/bio_lib.c \
	$(LOCAL_PATH)/crypto/bio/bio_meth.c \
	$(LOCAL_PATH)/crypto/bio/bss_acpt.c \
	$(LOCAL_PATH)/crypto/bio/bss_bio.c \
	$(LOCAL_PATH)/crypto/bio/bss_conn.c \
	$(LOCAL_PATH)/crypto/bio/bss_dgram.c \
	$(LOCAL_PATH)/crypto/bio/bss_fd.c \
	$(LOCAL_PATH)/crypto/bio/bss_file.c \
	$(LOCAL_PATH)/crypto/bio/bss_log.c \
	$(LOCAL_PATH)/crypto/bio/bss_mem.c \
	$(LOCAL_PATH)/crypto/bio/bss_null.c \
	$(LOCAL_PATH)/crypto/bio/bss_sock.c \
	$(wildcard $(LOCAL_PATH)/crypto/blake2/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/bn/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/buffer/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/camellia/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/cast/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/chacha/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/cmac/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/cms/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/comp/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/conf/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/ct/*.c) \
	$(LOCAL_PATH)/crypto/des/cbc_cksm.c \
	$(LOCAL_PATH)/crypto/des/cbc_enc.c \
	$(LOCAL_PATH)/crypto/des/cfb64ede.c \
	$(LOCAL_PATH)/crypto/des/cfb64enc.c \
	$(LOCAL_PATH)/crypto/des/cfb_enc.c \
	$(LOCAL_PATH)/crypto/des/des_enc.c \
	$(LOCAL_PATH)/crypto/des/ecb3_enc.c \
	$(LOCAL_PATH)/crypto/des/ecb_enc.c \
	$(LOCAL_PATH)/crypto/des/fcrypt.c \
	$(LOCAL_PATH)/crypto/des/fcrypt_b.c \
	$(LOCAL_PATH)/crypto/des/ofb64ede.c \
	$(LOCAL_PATH)/crypto/des/ofb64enc.c \
	$(LOCAL_PATH)/crypto/des/ofb_enc.c \
	$(LOCAL_PATH)/crypto/des/pcbc_enc.c \
	$(LOCAL_PATH)/crypto/des/qud_cksm.c \
	$(LOCAL_PATH)/crypto/des/rand_key.c \
	$(LOCAL_PATH)/crypto/des/set_key.c \
	$(LOCAL_PATH)/crypto/des/str2key.c \
	$(LOCAL_PATH)/crypto/des/xcbc_enc.c \
	$(wildcard $(LOCAL_PATH)/crypto/dh/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/dsa/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/dso/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/ec/curve448/arch_32/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/ec/curve448/*.c) \
	$(LOCAL_PATH)/crypto/ec/curve25519.c \
	$(LOCAL_PATH)/crypto/ec/ec2_oct.c \
	$(LOCAL_PATH)/crypto/ec/ec2_smpl.c \
	$(LOCAL_PATH)/crypto/ec/ec_ameth.c \
	$(LOCAL_PATH)/crypto/ec/ec_asn1.c \
	$(LOCAL_PATH)/crypto/ec/ec_check.c \
	$(LOCAL_PATH)/crypto/ec/ec_curve.c \
	$(LOCAL_PATH)/crypto/ec/ec_cvt.c \
	$(LOCAL_PATH)/crypto/ec/ec_err.c \
	$(LOCAL_PATH)/crypto/ec/ec_key.c \
	$(LOCAL_PATH)/crypto/ec/ec_kmeth.c \
	$(LOCAL_PATH)/crypto/ec/ec_lib.c \
	$(LOCAL_PATH)/crypto/ec/ec_mult.c \
	$(LOCAL_PATH)/crypto/ec/ec_oct.c \
	$(LOCAL_PATH)/crypto/ec/ec_pmeth.c \
	$(LOCAL_PATH)/crypto/ec/ec_print.c \
	$(LOCAL_PATH)/crypto/ec/ecdh_kdf.c \
	$(LOCAL_PATH)/crypto/ec/ecdh_ossl.c \
	$(LOCAL_PATH)/crypto/ec/ecdsa_ossl.c \
	$(LOCAL_PATH)/crypto/ec/ecdsa_sign.c \
	$(LOCAL_PATH)/crypto/ec/ecdsa_vrf.c \
	$(LOCAL_PATH)/crypto/ec/eck_prn.c \
	$(LOCAL_PATH)/crypto/ec/ecp_mont.c \
	$(LOCAL_PATH)/crypto/ec/ecp_nist.c \
	$(LOCAL_PATH)/crypto/ec/ecp_nistp224.c \
	$(LOCAL_PATH)/crypto/ec/ecp_nistp256.c \
	$(LOCAL_PATH)/crypto/ec/ecp_nistp521.c \
	$(LOCAL_PATH)/crypto/ec/ecp_nistputil.c \
	$(LOCAL_PATH)/crypto/ec/ecp_oct.c \
	$(LOCAL_PATH)/crypto/ec/ecp_smpl.c \
	$(LOCAL_PATH)/crypto/ec/ecx_meth.c \
	$(LOCAL_PATH)/crypto/engine/eng_all.c \
	$(LOCAL_PATH)/crypto/engine/eng_cnf.c \
	$(LOCAL_PATH)/crypto/engine/eng_ctrl.c \
	$(LOCAL_PATH)/crypto/engine/eng_dyn.c \
	$(LOCAL_PATH)/crypto/engine/eng_err.c \
	$(LOCAL_PATH)/crypto/engine/eng_fat.c \
	$(LOCAL_PATH)/crypto/engine/eng_init.c \
	$(LOCAL_PATH)/crypto/engine/eng_lib.c \
	$(LOCAL_PATH)/crypto/engine/eng_list.c \
	$(LOCAL_PATH)/crypto/engine/eng_openssl.c \
	$(LOCAL_PATH)/crypto/engine/eng_pkey.c \
	$(LOCAL_PATH)/crypto/engine/eng_rdrand.c \
	$(LOCAL_PATH)/crypto/engine/eng_table.c \
	$(LOCAL_PATH)/crypto/engine/tb_asnmth.c \
	$(LOCAL_PATH)/crypto/engine/tb_cipher.c \
	$(LOCAL_PATH)/crypto/engine/tb_dh.c \
	$(LOCAL_PATH)/crypto/engine/tb_digest.c \
	$(LOCAL_PATH)/crypto/engine/tb_dsa.c \
	$(LOCAL_PATH)/crypto/engine/tb_eckey.c \
	$(LOCAL_PATH)/crypto/engine/tb_pkmeth.c \
	$(LOCAL_PATH)/crypto/engine/tb_rand.c \
	$(LOCAL_PATH)/crypto/engine/tb_rsa.c \
	$(wildcard $(LOCAL_PATH)/crypto/err/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/evp/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/hmac/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/idea/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/kdf/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/lhash/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/md4/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/md5/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/mdc2/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/modes/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/objects/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/ocsp/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/pem/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/pkcs12/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/pkcs7/*.c) \
	$(LOCAL_PATH)/crypto/poly1305/poly1305.c \
	$(LOCAL_PATH)/crypto/poly1305/poly1305_ameth.c \
	$(LOCAL_PATH)/crypto/poly1305/poly1305_pmeth.c \
	$(wildcard $(LOCAL_PATH)/crypto/rand/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/rc2/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/rc4/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/ripemd/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/rsa/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/seed/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/sha/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/siphash/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/sm2/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/sm3/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/sm4/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/srp/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/stack/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/store/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/ts/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/txt_db/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/ui/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/whrlpool/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/x509/*.c) \
	$(wildcard $(LOCAL_PATH)/crypto/x509v3/*.c) \
	$(LOCAL_PATH)/crypto/cpt_err.c \
	$(LOCAL_PATH)/crypto/cryptlib.c \
	$(LOCAL_PATH)/crypto/ctype.c \
	$(LOCAL_PATH)/crypto/cversion.c \
	$(LOCAL_PATH)/crypto/ebcdic.c \
	$(LOCAL_PATH)/crypto/ex_data.c \
	$(LOCAL_PATH)/crypto/init.c \
	$(LOCAL_PATH)/crypto/mem.c \
	$(LOCAL_PATH)/crypto/mem_clr.c \
	$(LOCAL_PATH)/crypto/mem_dbg.c \
	$(LOCAL_PATH)/crypto/mem_sec.c \
	$(LOCAL_PATH)/crypto/o_dir.c \
	$(LOCAL_PATH)/crypto/o_fips.c \
	$(LOCAL_PATH)/crypto/o_fopen.c \
	$(LOCAL_PATH)/crypto/o_init.c \
	$(LOCAL_PATH)/crypto/o_str.c \
	$(LOCAL_PATH)/crypto/o_time.c \
	$(LOCAL_PATH)/crypto/uid.c \
	$(LOCAL_PATH)/crypto/getenv.c \
	$(LOCAL_PATH)/crypto/threads_pthread.c)

include $(BUILD_STATIC_LIBRARY)

########################
#
# ssl static library
#
########################
include $(CLEAR_VARS)

LOCAL_MODULE := ssl
LOCAL_C_INCLUDES := $(PROJ_PATH)
LOCAL_C_INCLUDES += $(PROJ_PATH)/android/$(TARGET_ARCH_ABI)/include

# The defines
LOCAL_CFLAGS += -DENGINESDIR=\"/usr/local/engines-1.1\" 
LOCAL_CFLAGS += -DOPENSSLDIR=\"/usr/local/ssl\" 
LOCAL_CFLAGS += -DOPENSSL_NO_ASM 
LOCAL_CFLAGS += -DOPENSSL_NO_STATIC_ENGINE 

# Compilation flags
LOCAL_CFLAGS += -fPIC 
LOCAL_CFLAGS += -fno-limit-debug-info 

LOCAL_PATH = $(PROJ_PATH)
LOCAL_SRC_FILES := $(subst $(LOCAL_PATH)/,, \
	$(wildcard $(LOCAL_PATH)/ssl/record/*.c) \
	$(wildcard $(LOCAL_PATH)/ssl/statem/*.c) \
	$(wildcard $(LOCAL_PATH)/ssl/*.c) )

include $(BUILD_STATIC_LIBRARY)
