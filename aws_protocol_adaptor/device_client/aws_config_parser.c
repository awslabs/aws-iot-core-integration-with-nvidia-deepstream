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
#include <string.h>
#include <gst/gst.h>
#include "aws_config_parser.h"
#include "aws_iot_config.h"
#include "aws_iot_mqtt_client_interface.h"

#define CONFIG_MQTT_INFO "message-broker"
#define CONFIG_MQTT_HOSTADD "HostAddress"
#define CONFIG_MQTT_PORT "Port"
#define CONFIG_MQTT_CLIENT_ID "ClientID"
#define CONFIG_MQTT_THINGNAME "ThingName"
#define CONFIG_MQTT_ROOTCALOCATION "RootCALocation"
#define CONFIG_MQTT_CERTLOCATION "DeepstreamCertLocation"
#define CONFIG_MQTT_PRIVKEYLOCATION "DeepstreamPrivateKeyLocation"
#define CONFIG_MQTT_COMMAND_TIMEOUT "MQTTCommandTimeoutMS"
#define CONFIG_MQTT_TLS_TIMEOUT "TLSHandshakeTimeoutMS"
#define CONFIG_MQTT_SSL_HOST_NAME_VERIFY "SSLHostnameVerify"
#define CHECK_ERROR(error)                           \
  if (error)                                         \
  {                                                  \
    g_print("ERROR in parsing: %s", error->message); \
    goto done;                                       \
  }
static gchar g_pHostURL[AWS_IOT_MAX_HOST_URL_LEN];
static gchar g_pClientID[AWS_IOT_MAX_CLIENT_ID_LEN];
static gchar g_pRootCALocation[AWS_IOT_MAX_ROOT_CA_PATH_LEN];
static gchar g_pDeviceCertLocation[AWS_IOT_MAX_CERT_PATH_LEN];
static gchar g_pDevicePrivateKeyLocation[AWS_IOT_MAX_PRIVATE_KEY_PATH_LEN];

GST_DEBUG_CATEGORY(APP_CFG_PARSER_CAT);

gboolean
parse_server(IoT_Client_Init_Params *iot_params, IoT_Client_Connect_Params *con_params, GKeyFile *key_file)
{
  gboolean ret = FALSE;
  gchar **keys = NULL;
  gchar **key = NULL;
  GError *error = NULL;
  keys = g_key_file_get_keys(key_file, CONFIG_MQTT_INFO, NULL, &error);
  CHECK_ERROR(error);

  for (key = keys; *key; key++)
  {

    if (!g_strcmp0(*key, CONFIG_MQTT_HOSTADD))
    {
      gchar *pHostURL_temp = g_key_file_get_string(key_file, CONFIG_MQTT_INFO,
                                                   CONFIG_MQTT_HOSTADD, &error);
      iot_params->pHostURL = g_pHostURL;
      g_strlcpy(g_pHostURL, pHostURL_temp, AWS_IOT_MAX_HOST_URL_LEN);
      g_free(pHostURL_temp);
      CHECK_ERROR(error);
    }
    else if (!g_strcmp0(*key, CONFIG_MQTT_PORT))
    {
      iot_params->port =
          g_key_file_get_integer(key_file, CONFIG_MQTT_INFO,
                                 CONFIG_MQTT_PORT, &error);
      CHECK_ERROR(error);
    }
    else if (!g_strcmp0(*key, CONFIG_MQTT_CLIENT_ID))
    {
      gchar *pClientID_temp = g_key_file_get_string(key_file, CONFIG_MQTT_INFO,
                                                    CONFIG_MQTT_CLIENT_ID, &error);
      con_params->pClientID = g_pClientID;
      g_strlcpy(g_pClientID, pClientID_temp, AWS_IOT_MAX_CLIENT_ID_LEN);
      g_free(pClientID_temp);
      CHECK_ERROR(error);
    }
    else if (!g_strcmp0(*key, CONFIG_MQTT_ROOTCALOCATION))
    {
      gchar *pRootCALocation_temp = g_key_file_get_string(key_file, CONFIG_MQTT_INFO,
                                                          CONFIG_MQTT_ROOTCALOCATION, &error);
      iot_params->pRootCALocation = g_pRootCALocation;
      g_strlcpy(g_pRootCALocation, pRootCALocation_temp, AWS_IOT_MAX_ROOT_CA_PATH_LEN);
      g_free(pRootCALocation_temp);
      CHECK_ERROR(error);
    }
    else if (!g_strcmp0(*key, CONFIG_MQTT_CERTLOCATION))
    {
      gchar *pDeviceCertLocation_temp = g_key_file_get_string(key_file, CONFIG_MQTT_INFO,
                                                              CONFIG_MQTT_CERTLOCATION, &error);
      iot_params->pDeviceCertLocation = g_pDeviceCertLocation;
      g_strlcpy(g_pDeviceCertLocation, pDeviceCertLocation_temp, AWS_IOT_MAX_CERT_PATH_LEN);
      g_free(pDeviceCertLocation_temp);
      CHECK_ERROR(error);
    }
    else if (!g_strcmp0(*key, CONFIG_MQTT_PRIVKEYLOCATION))
    {
      gchar *pDevicePrivateKeyLocation_temp = g_key_file_get_string(key_file, CONFIG_MQTT_INFO,
                                                                    CONFIG_MQTT_PRIVKEYLOCATION, &error);
      iot_params->pDevicePrivateKeyLocation = g_pDevicePrivateKeyLocation;
      g_strlcpy(g_pDevicePrivateKeyLocation, pDevicePrivateKeyLocation_temp, AWS_IOT_MAX_PRIVATE_KEY_PATH_LEN);
      g_free(pDevicePrivateKeyLocation_temp);
      CHECK_ERROR(error);
    }
    else if (!g_strcmp0(*key, CONFIG_MQTT_COMMAND_TIMEOUT))
    {
      iot_params->mqttCommandTimeout_ms =
          g_key_file_get_integer(key_file, CONFIG_MQTT_INFO,
                                 CONFIG_MQTT_COMMAND_TIMEOUT, &error);
      CHECK_ERROR(error);
    }
    else if (!g_strcmp0(*key, CONFIG_MQTT_TLS_TIMEOUT))
    {
      iot_params->tlsHandshakeTimeout_ms =
          g_key_file_get_integer(key_file, CONFIG_MQTT_INFO,
                                 CONFIG_MQTT_TLS_TIMEOUT, &error);
      CHECK_ERROR(error);
    }
    else if (!g_strcmp0(*key, CONFIG_MQTT_SSL_HOST_NAME_VERIFY))
    {
      gchar *isSSLHostnameVerify_str = g_key_file_get_string(key_file, CONFIG_MQTT_INFO,
                                                             CONFIG_MQTT_SSL_HOST_NAME_VERIFY, &error);
      if (g_strcmp0(isSSLHostnameVerify_str, "false") && g_strcmp0(isSSLHostnameVerify_str, "true"))
      {
        NVGSTDS_WARN_MSG_V("SSLHostnameVerify has to be true or false. Current input is not valid, defaulting to true\n");
      }
      iot_params->isSSLHostnameVerify = g_strcmp0(isSSLHostnameVerify_str, "false");
      g_free(isSSLHostnameVerify_str);
      CHECK_ERROR(error);
    }
    else if (!g_strcmp0(*key, CONFIG_MQTT_THINGNAME))
    {
      // No need for thing name in this implementation.
      continue;
    }
    else
    {
      NVGSTDS_WARN_MSG_V("Unknown key '%s' for group [%s]", *key,
                         CONFIG_MQTT_INFO);
    }
  }

  ret = TRUE;
done:
  if (error)
  {
    g_error_free(error);
  }
  if (keys)
  {
    g_strfreev(keys);
  }
  if (!ret)
  {
    NVGSTDS_ERR_MSG_V("%s failed", __func__);
  }
  return ret;
}

gboolean
parse_config_file(IoT_Client_Init_Params *iot_params, IoT_Client_Connect_Params *con_params, gchar *cfg_file_path)
{
  GKeyFile *cfg_file = g_key_file_new();
  GError *error = NULL;
  gboolean ret = FALSE;
  gchar **groups = NULL;
  gchar **group;
  guint i, j;

  if (!APP_CFG_PARSER_CAT)
  {
    GST_DEBUG_CATEGORY_INIT(APP_CFG_PARSER_CAT, "NVDS_CFG_PARSER", 0, NULL);
  }
  if (!g_key_file_load_from_file(cfg_file, cfg_file_path, G_KEY_FILE_NONE,
                                 &error))
  {
    GST_CAT_ERROR(APP_CFG_PARSER_CAT, "Failed to load uri file: %s",
                  error->message);
    goto done;
  }
  groups = g_key_file_get_groups(cfg_file, NULL);
  for (group = groups; *group; group++)
  {
    gboolean parse_err = FALSE;
    GST_CAT_DEBUG(APP_CFG_PARSER_CAT, "Parsing group: %s", *group);
    if (!g_strcmp0(*group, CONFIG_MQTT_INFO))
    {
      parse_err = !parse_server(iot_params, con_params, cfg_file);
      if (parse_err)
      {
        GST_CAT_ERROR(APP_CFG_PARSER_CAT, "%d", parse_err);
        goto done;
      }
    }
  }
  ret = true;
done:
  if (cfg_file)
  {
    g_key_file_free(cfg_file);
  }

  if (groups)
  {
    g_strfreev(groups);
  }

  if (error)
  {
    g_error_free(error);
  }
  if (!ret)
  {
    NVGSTDS_ERR_MSG_V("%s failed", __func__);
  }
  return ret;
}
