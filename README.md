AWS IoT Core Integration with NVIDIA DeepStream
------------
This project implements protocol adaptor between NVIDIA DeepStream SDK and AWS (edge to cloud messaging).
The adaptor implements the API for client applications to publish inference results or metadata using MQTT messages to AWS IoT Core.

Installation Guide:
------------
**Using pre-compiled share library (.so) file**
1. git clone this repo to Jetson device. We recommend you putting this into `<DeepStreamSDK>/sources/libs/`
2. Create a thing on aws console following: https://docs.aws.amazon.com/iot/latest/developerguide/device-certs-create.html
3. Download certificates (inluding root certificate) in step 4 in the link above to Jetson device.
4. Edit cfg_aws.txt with updated absolute paths to the certificates downloaded in step above, and replace `<YOUR IOT HOST ADDRESS>` with your AWS IoT end-point URL, which can be found in AWS IoT console → setting, in the box showing endpoint.
5. Compile test app 4 and run it with the following command:
```
./deepstream-test4-app -i ../../../../samples/streams/sample_720p.h264 -t test -p <\THIS REPO>/aws_protocol_adaptor/device_client/libnvds_aws_proto.so --conn-str=hello -c <\THIS REPO>/aws_protocol_adaptor/device_client/cfg_aws.txt
```
6. Compile and test app 5 by modifying test config file `<DeepStreamSDK>/sources/apps/sample_apps/deepstream-test5/configs/test5_config_file_src_infer.txt` on the following lines:
```
msg-broker-proto-lib=<\THIS REPO>/aws_protocol_adaptor/device_client/libnvds_aws_proto.so
msg-broker-config=<\THIS REPO>/aws_protocol_adaptor/device_client/cfg_aws.txt
```

**Optional: Only if you want to re-compile and modify .so file**
1. Installing all the necessary dependencies for Deepstream App following NVIDIA installation guide https://docs.nvidia.com/metropolis/deepstream/4.0/dev-guide/index.html. And verify that deepstream-app builds and runs successfully.
2. Download AWS iot embedded C SDK into aws-iot-sdk folder from https://github.com/aws/aws-iot-device-sdk-embedded-C
   Current SDK version is 3, support for AWS IoT SDK version 4 is in our pipeline. (Version 4 is currently in Beta testing.)
3. Download mbedTLS (before version 2.16) into `aws-iot-sdk/external_libs/mbedTLS` folder from https://github.com/ARMmbed/mbedtls
   Current AWS IoT SDK does not support 2.21 (development branch). They have an issue tracker for this: https://github.com/aws/aws-iot-device-sdk-embedded-C/issues/814
4. Run 'make' in `aws_protocol_adaptor/device_client`. (Or make clean to clean any compiled files.)

AWS cfg:
----------
Please set up your cfg files after you register your device on AWS cloud and download your certs.
NOTE: DO NOT delete the line [message-broker] inside the cfg file. Its the section identifier used for parsing.


Set message properties:
-----------------------
Some additional message properties like buffer size, can be set in `aws_iot_config.h`. If you want to change any of these settings, you will have to compile your own .so file.

For we used QoS 0 for all the send functions. However, you can adjust to QoS 1 in message broker source code: `aws_nvmsgbroker.c`.

To adjust the level of IOT logging, please go to Makefile and comment or un-comment the following lines.

LOG_FLAGS += -DENABLE_IOT_DEBUG
LOG_FLAGS += -DENABLE_IOT_INFO
LOG_FLAGS += -DENABLE_IOT_WARN
LOG_FLAGS += -DENABLE_IOT_ERROR

## License

This project is licensed under the Apache-2.0 License.

