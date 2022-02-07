
#pragma once

extern "C"
{
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
}
#include <AsyncMqttClient.h>

#define HEARTBEAT_TOPIC "heartbeat"

#define MQTT_ON "on"
#define MQTT_OFF "off"

namespace creatures
{

    class MQTT
    {
        static const char *broker_role;
        static const char *broker_service;
        static const char *broker_protocol;

        static IPAddress mqtt_broker_address;
        static uint16_t mqtt_broker_port;

    public:
        static TimerHandle_t mqttReconnectTimer;
        static AsyncMqttClient mqttClient;

        static void configure(IPAddress mqtt_broker_address, uint16_t mqtt_broker_port);
        static void connectToMqtt();
        static void onMqttConnect(bool sessionPresent);
        static void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
        static void onMqttSubscribe(uint16_t packetId, uint8_t qos);
        static void onMqttUnsubscribe(uint16_t packetId);
        static void onMqttMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);
        static void onMqttPublish(uint16_t packetId);
        static void onWifiDisconnect();
    };

}