
#pragma once

#include <Arduino.h>

extern "C"
{
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
}
#include <AsyncMqttClient.h>

#define MQTT_ON "on"
#define MQTT_OFF "off"

namespace creatures
{

    class MQTT
    {

    public:
        static void subscribe(String topic, uint8_t qos);
        static void connect(IPAddress mqtt_broker_address, uint16_t mqtt_broker_port);

        // Handlers
        static void onConnect(bool sessionPresent);
        static void onDisconnect(AsyncMqttClientDisconnectReason reason);
        static void onSubscribe(uint16_t packetId, uint8_t qos);
        static void onUnsubscribe(uint16_t packetId);

        static void onMessage(AsyncMqttClientInternals::OnMessageUserCallback callback);
        static void onPublish(uint16_t packetId);
        static void onWifiDisconnect();

    protected:
        static void defaultOnMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);
    };

}