
#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

extern "C"
{
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
}
#include <AsyncMqttClient.h>

#define MQTT_ON "on"
#define MQTT_OFF "off"

#define HEARTBEAT_TOPIC "creatures/heartbeat"

namespace creatures
{

    class MQTT
    {

    public:
        MQTT(String ourName);
        static void subscribe(String topic, uint8_t qos);
        static uint16_t publish(String topic, String message, uint8_t qos, boolean retain);
        static void connect(IPAddress mqtt_broker_address, uint16_t mqtt_broker_port);

        static void startHeartbeat();

        // Handlers
        static void onConnect(bool sessionPresent);
        static void onDisconnect(AsyncMqttClientDisconnectReason reason);
        static void onSubscribe(uint16_t packetId, uint8_t qos);
        static void onUnsubscribe(uint16_t packetId);

        static void onMessage(AsyncMqttClientInternals::OnMessageUserCallback callback);
        static void onPublish(uint16_t packetId);
        static void onWifiDisconnect();

        static uint16_t publishRaw(String topic, String message, uint8_t qos, boolean retain);

    protected:
        static void defaultOnMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);
        static String ourName;
    };

    portTASK_FUNCTION_PROTO(creatureHeartBeatTask, pvParameters);

}