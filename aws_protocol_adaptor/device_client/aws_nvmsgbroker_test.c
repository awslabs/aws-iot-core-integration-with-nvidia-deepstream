/*
 * Copyright 2010-2015 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>

#include "aws_iot_config.h"
#include "aws_iot_log.h"
#include "aws_iot_version.h"
#include "aws_iot_mqtt_client_interface.h"

#include <gst/gst.h>
#include "aws_config_parser.h"
#include "nvds_msgapi.h"
#include <sys/syscall.h>
#include <glib.h>
#include <dlfcn.h>

extern const IoT_Client_Init_Params iotClientInitParamsDefault;
extern const IoT_Client_Connect_Params iotClientConnectParamsDefault;

NvDsMsgApiHandle (*nvds_msgapi_connect_ptr)(char *connection_str, nvds_msgapi_connect_cb_t connect_cb, char *config_path);
NvDsMsgApiErrorType (*nvds_msgapi_send_ptr)(NvDsMsgApiHandle conn, char *topic, const uint8_t *payload, size_t nbuf);
NvDsMsgApiErrorType (*nvds_msgapi_disconnect_ptr)(NvDsMsgApiHandle h_ptr);
NvDsMsgApiErrorType (*nvds_msgapi_send_async_ptr)(NvDsMsgApiHandle conn, char *topic, const uint8_t *payload, size_t nbuf, nvds_msgapi_send_cb_t send_callback, void *user_ptr);
NvDsMsgApiErrorType (*nvds_msgapi_do_work_ptr)(NvDsMsgApiHandle conn);

const char *AWS_PROTO_SO = "./libnvds_aws_proto.so";
/* ************************************************************************* */
// Connect function def
/* ************************************************************************* */

void connect_cb(NvDsMsgApiHandle h_ptr, NvDsMsgApiEventType evt)
{
    AWS_IoT_Client *client = (AWS_IoT_Client *)h_ptr;
    if (evt == NVDS_MSGAPI_EVT_DISCONNECT)
    {
        g_print("In sample prog: connect failed. \n");
    }
    else
        g_print("In sample prog: connect success \n");
}

/* ************************************************************************* */
// Async: Send function def
/* ************************************************************************* */

struct send_info_t
{
    pid_t tid;
    int num;
};

void send_callback(void *user_ptr, NvDsMsgApiErrorType completion_flag)
{
    struct send_info_t *info = (struct send_info_t *)user_ptr;
    if (completion_flag == NVDS_MSGAPI_OK)
        g_print("Thread [%d] , Message num %d : send success\n", info->tid, info->num);
    else
        g_print("Thread [%d] , Message num %d : send failed\n", info->tid, info->num);
    g_free(info);
}

void *func(void *ptr)
{
    NvDsMsgApiHandle conn = (NvDsMsgApiHandle)ptr;
    char cPayload_send_async[100] = {0};
    sprintf(cPayload_send_async, "%s", "hello from SDK nvds_msgapi_send_async");
    pid_t myid = syscall(SYS_gettid);
    char cTopic_send_async[20] = {0};
    sprintf(cTopic_send_async, "%s", "sdkTest/sub");
    for (int i = 0; i < 10; i++)
    {
        struct send_info_t *myinfo = g_malloc(sizeof(struct send_info_t));
        myinfo->tid = myid;
        myinfo->num = i;
        nvds_msgapi_send_async_ptr(conn, cTopic_send_async, (const uint8_t *)cPayload_send_async, strlen(cPayload_send_async), send_callback, &myinfo);
        sleep(0.5);
    }
}

int main(int argc, char **argv)
{
    void *so_handle;
    so_handle = dlopen(AWS_PROTO_SO, RTLD_NOW);
    if (!so_handle)
    {
        g_print("Failed to load lib, exiting: %s\n", dlerror());
        return 0;
    }
    *(void **)(&nvds_msgapi_connect_ptr) = dlsym(so_handle, "nvds_msgapi_connect");
    *(void **)(&nvds_msgapi_send_ptr) = dlsym(so_handle, "nvds_msgapi_send");
    *(void **)(&nvds_msgapi_disconnect_ptr) = dlsym(so_handle, "nvds_msgapi_disconnect");
    *(void **)(&nvds_msgapi_send_async_ptr) = dlsym(so_handle, "nvds_msgapi_send_async");
    *(void **)(&nvds_msgapi_do_work_ptr) = dlsym(so_handle, "nvds_msgapi_do_work");
    gchar config[20] = "cfg_aws.txt";

    /* ************************************************************************* */
    // Connect call
    /* ************************************************************************* */
    NvDsMsgApiHandle conn = nvds_msgapi_connect_ptr(NULL, connect_cb, config);

    /* ************************************************************************* */
    // Send call
    /* ************************************************************************* */
    char cPayload_send[100];
    sprintf(cPayload_send, "%s", "hello from SDK nvds_msgapi_send");
    char cTopic_send[20];
    sprintf(cTopic_send, "%s", "sdkTest/sub");
    nvds_msgapi_send_ptr(conn, cTopic_send, cPayload_send, strlen(cPayload_send));
    /* ************************************************************************* */
    // Async: Send call (current AWS IoT client does not support multi-thread.
    /* ************************************************************************* */

    int NUM_THREADS = 2;
    pthread_t tid[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++)
        pthread_create(&tid[i], NULL, &func, (void *)conn);
    for (int i = 0; i < NUM_THREADS; i++)
        pthread_join(tid[i], NULL);
    /* ************************************************************************* */
    // Do work call
    /* ************************************************************************* */
    for (int i = 0; i < NUM_THREADS * 15; i++)
    {
        g_print("doing work.\n");
        nvds_msgapi_do_work_ptr(conn);
    }
    /* ************************************************************************* */
    // Disconnect call
    /* ************************************************************************* */
    g_print("Disconnecting.\n");
    nvds_msgapi_disconnect_ptr(conn);

    return 0;
}
