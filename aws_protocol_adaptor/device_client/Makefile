###############################################################################
# Copyright 2010-2015 Amazon.com, Inc. or its affiliates. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License").
# You may not use this file except in compliance with the License.
# A copy of the License is located at
#
#  http://aws.amazon.com/apache2.0
#
# or in the "license" file accompanying this file. This file is distributed
# on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
# express or implied. See the License for the specific language governing
# permissions and limitations under the License.
###############################################################################


#This target is to ensure accidental execution of Makefile as a bash script will not execute commands like rm in unexpected directories and exit gracefully.
.prevent_execution:
	exit 0

CC = gcc

#remove @ for no make command prints
DEBUG = @

APP_DIR = .
APP_INCLUDE_DIRS += -I $(APP_DIR)
APP_NAME = aws_nvmsgbroker_test
APP_SRC_FILES = $(APP_NAME).c

#IoT client directory
IOT_CLIENT_DIR = ../aws-iot-sdk

PLATFORM_DIR = $(IOT_CLIENT_DIR)/platform/linux/mbedtls
PLATFORM_COMMON_DIR = $(IOT_CLIENT_DIR)/platform/linux/common

IOT_INCLUDE_DIRS += -I $(IOT_CLIENT_DIR)/include
IOT_INCLUDE_DIRS += -I $(IOT_CLIENT_DIR)/external_libs/jsmn
IOT_INCLUDE_DIRS += -I $(PLATFORM_COMMON_DIR)
IOT_INCLUDE_DIRS += -I $(PLATFORM_DIR)

IOT_SRC_FILES += $(IOT_CLIENT_DIR)/src/aws_iot_mqtt_client.c
IOT_SRC_FILES += $(IOT_CLIENT_DIR)/src/aws_iot_mqtt_client_common_internal.c
IOT_SRC_FILES += $(IOT_CLIENT_DIR)/src/aws_iot_mqtt_client_connect.c
IOT_SRC_FILES += $(IOT_CLIENT_DIR)/src/aws_iot_mqtt_client_publish.c
IOT_SRC_FILES += $(IOT_CLIENT_DIR)/src/aws_iot_mqtt_client_subscribe.c
IOT_SRC_FILES += $(IOT_CLIENT_DIR)/src/aws_iot_mqtt_client_unsubscribe.c
IOT_SRC_FILES += $(IOT_CLIENT_DIR)/src/aws_iot_mqtt_client_yield.c


IOT_SRC_FILES += $(shell find $(IOT_CLIENT_DIR)/external_libs/jsmn -name '*.c')
IOT_SRC_FILES += $(shell find $(PLATFORM_DIR)/ -name '*.c')
IOT_SRC_FILES += $(shell find $(PLATFORM_COMMON_DIR)/ -name '*.c')

#TLS - mbedtls
MBEDTLS_DIR = $(IOT_CLIENT_DIR)/external_libs/mbedTLS/mbedtls
TLS_LIB_DIR = $(MBEDTLS_DIR)/library
CRYPTO_LIB_DIR = $(MBEDTLS_DIR)/library
TLS_INCLUDE_DIR = -I $(MBEDTLS_DIR)/include
EXTERNAL_LIBS += -L$(TLS_LIB_DIR)
LD_FLAG += -Wl,-rpath,$(TLS_LIB_DIR)
LD_FLAG += -ldl $(TLS_LIB_DIR)/libmbedtls.a $(TLS_LIB_DIR)/libmbedcrypto.a $(TLS_LIB_DIR)/libmbedx509.a -lpthread

# GLibs
DP_SOURCE_DIR = ../../..
NVDS_VERSION:=4.0
LIB_INSTALL_DIR?=/opt/nvidia/deepstream/deepstream-$(NVDS_VERSION)/lib/
PKGS:= gstreamer-1.0 gstreamer-video-1.0 x11
GLIBS_INCLUDE+= -I $(DP_SOURCE_DIR)/includes
GLIBS_INCLUDE+= `pkg-config --cflags $(PKGS)`
LIBS+= `pkg-config --libs $(PKGS)`
GLIB_SRC_FILES+= $(APP_DIR)/aws_config_parser.c
GLIB_SRC_FILES+= $(APP_DIR)/aws_nvmsgbroker.c

#Aggregate all include and src directories
INCLUDE_ALL_DIRS += $(IOT_INCLUDE_DIRS)
INCLUDE_ALL_DIRS += $(TLS_INCLUDE_DIR)
INCLUDE_ALL_DIRS += $(APP_INCLUDE_DIRS)
INCLUDE_ALL_DIRS += $(GLIBS_INCLUDE)

SRC_FILES += $(IOT_SRC_FILES)
SRC_FILES += $(GLIB_SRC_FILES)

# Logging level control
# LOG_FLAGS += -DENABLE_IOT_DEBUG
# LOG_FLAGS += -DENABLE_IOT_INFO
LOG_FLAGS += -DENABLE_IOT_WARN
LOG_FLAGS += -DENABLE_IOT_ERROR

COMPILER_FLAGS += $(LOG_FLAGS)
#If the processor is big endian uncomment the compiler flag
#COMPILER_FLAGS += -DREVERSED

MBED_TLS_MAKE_CMD = $(MAKE) -C $(MBEDTLS_DIR)

PRE_MAKE_CMD = $(MBED_TLS_MAKE_CMD)
MAKE_OBJ_CMD = $(CC) -c -fPIC $(SRC_FILES) $(COMPILER_FLAGS) $(LD_FLAG) $(EXTERNAL_LIBS) $(INCLUDE_ALL_DIRS) $(LIBS)

OBJS:=./*.o
TLS_OBJS=$(TLS_LIB_DIR)/*.o
MAKE_LIB_CMD = $(CC) -shared -fPIC -o libnvds_aws_proto.so $(OBJS) $(TLS_OBJS) ${LIBS}
MAKE_TEST_CMD = $(CC) $(SRC_FILES) $(APP_SRC_FILES) $(COMPILER_FLAGS) $(LD_FLAG) $(EXTERNAL_LIBS) $(INCLUDE_ALL_DIRS) $(LIBS) -o $(APP_NAME)

all:
	$(PRE_MAKE_CMD)
	$(DEBUG)$(MAKE_OBJ_CMD)
	$(DEBUG)$(MAKE_LIB_CMD)
	$(DEBUG)$(MAKE_TEST_CMD)
	$(POST_MAKE_CMD)

clean:
	rm -f $(APP_DIR)/$(APP_NAME)
	rm -f *.o
	rm -f *.so
	$(MBED_TLS_MAKE_CMD) clean
