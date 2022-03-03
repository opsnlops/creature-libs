
#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

extern "C"
{
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/queue.h"
}
#include <AsyncMqttClient.h>

#define MQTT_ON "on"
#define MQTT_OFF "off"

#define HEARTBEAT_TOPIC "creatures/heartbeat"

// Configure the message passing system
#define MQTT_QUEUE_LENGTH 16
#define MQTT_MAX_TOPIC_LENGTH 128
#define MQTT_MAX_PAYLOAD_LENGTH 1024

namespace creatures
{
    /**
     * @brief A message received from MQTT
     *
     * This is a bunch of fixed-length arrays so they can go into a queue
     * without a lot of headaches
     */
    struct MqttMessage
    {
        char topic[MQTT_MAX_TOPIC_LENGTH + 1];
        char topicGlobalNamespace[MQTT_MAX_TOPIC_LENGTH + 1];
        char payload[MQTT_MAX_PAYLOAD_LENGTH + 1];
    } __attribute__((packed));

    class MQTT
    {

    public:
        MQTT(String ourName);
        static void subscribe(String topic, uint8_t qos);
        static void subscribeGlobalNamespace(String topic, uint8_t qos);
        static uint16_t publish(String topic, String message, uint8_t qos, boolean retain);
        static uint16_t publishGlobalNamespace(String topic, String message, uint8_t qos, boolean retain);
        static void connect(IPAddress mqtt_broker_address, uint16_t mqtt_broker_port);

        static void startHeartbeat();

        static QueueHandle_t getIncomingMessageQueue();

        // Handlers
        static void onConnect(bool sessionPresent);
        static void onDisconnect(AsyncMqttClientDisconnectReason reason);
        static void onSubscribe(uint16_t packetId, uint8_t qos);
        static void onUnsubscribe(uint16_t packetId);

        static void onPublish(uint16_t packetId);

        static void onMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);

    protected:
        static String ourName;
    };

    portTASK_FUNCTION_PROTO(creatureHeartBeatTask, pvParameters);

}