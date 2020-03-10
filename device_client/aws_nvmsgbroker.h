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

#ifndef __NVGSTDS_APP_H__
#define __NVGSTDS_APP_H__

#ifdef __cplusplus
extern "C"
{
#endif
    // #include "deepstream_common.h"
    // #include "aws_iot_mqtt_client_interface.h"

#define NVGSTDS_ERR_MSG_V(msg, ...) \
    g_print("** ERROR: <%s:%d>: " msg "\n", __func__, __LINE__, ##__VA_ARGS__)

#define NVGSTDS_INFO_MSG_V(msg, ...) \
    g_print("** INFO: <%s:%d>: " msg "\n", __func__, __LINE__, ##__VA_ARGS__)

#define NVGSTDS_WARN_MSG_V(msg, ...) \
    g_print("** WARN: <%s:%d>: " msg "\n", __func__, __LINE__, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif
NvDsMsgApiHandle nvds_msgapi_connect(char *connection_str, nvds_msgapi_connect_cb_t connect_cb, char *config_path);
NvDsMsgApiErrorType nvds_msgapi_disconnect(NvDsMsgApiHandle h_ptr);
NvDsMsgApiErrorType nvds_msgapi_send(NvDsMsgApiHandle conn, char *topic, const uint8_t *payload, size_t nbuf);
NvDsMsgApiErrorType nvds_msgapi_send_async(NvDsMsgApiHandle h_ptr, char *topic, const uint8_t *payload, size_t nbuf, nvds_msgapi_send_cb_t send_callback, void *user_ptr);
void nvds_msgapi_do_work(NvDsMsgApiHandle h_ptr);
