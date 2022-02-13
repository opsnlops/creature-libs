
#include <Arduino.h>
#include <ArduinoJson.h>

#include <AsyncMqttClient.h>
#include <WiFi.h>

#include "mqtt.h"

#include "time/time.h"

namespace creatures
{

    static TimerHandle_t mqttReconnectTimer;
    static AsyncMqttClient mqttClient;
    static IPAddress mqtt_broker_address;
    static uint16_t mqtt_broker_port;
    static String mqtt_base_name;
    static String mqtt_base_topic;

    TaskHandle_t creatureHeartBeatTaskHandler;

    MQTT::MQTT(String ourName)
    {
        ourName.toLowerCase();
        mqtt_base_name = ourName;
        ESP_LOGV(TAG, "Set mqtt_base_name to %s", mqtt_base_name);

        mqtt_base_topic = String("creatures/") + mqtt_base_name;
        ESP_LOGV(TAG, "Set mqtt_base_topic to %s", mqtt_base_topic.c_str());
    }

    void MQTT::connect(IPAddress _mqtt_broker_address, uint16_t _mqtt_broker_port)
    {
        mqtt_broker_address = _mqtt_broker_address;
        mqtt_broker_port = _mqtt_broker_port;

        ESP_LOGI(TAG, "The IP of the broker is %s, port: %d", mqtt_broker_address.toString(), mqtt_broker_port);

        mqttClient.setServer(mqtt_broker_address, mqtt_broker_port);
        mqttClient.onConnect(onConnect);
        mqttClient.onDisconnect(onDisconnect);
        mqttClient.onSubscribe(onSubscribe);
        mqttClient.onUnsubscribe(onUnsubscribe);
        mqttClient.onPublish(onPublish);
        mqttClient.onMessage(defaultOnMessage);

        ESP_LOGI(TAG, "Connecting to MQTT...");
        mqttClient.connect();
        ESP_LOGI(TAG, "connected!");
    }

    void MQTT::subscribe(String topic, uint8_t qos)
    {
        String fullTopic = mqtt_base_topic + String("/") + topic;
        ESP_LOGI(TAG, "Mapping %s to %s", topic, fullTopic.c_str());

        ESP_LOGI(TAG, "Subscribing to %s (%d)", fullTopic.c_str(), qos);
        mqttClient.subscribe(fullTopic.c_str(), qos);
    }

    /**
     * @brief Publish a message to a topic owned by the creature
     *
     * @param topic topic to publish to. Will be prepended with "creatures/$name"
     * @param message The message to send
     * @param qos QOS of message
     * @param retain Retain this message in the broker?
     * @return uint16_t Whatever the client return code was
     */
    uint16_t MQTT::publish(String topic, String message, uint8_t qos, boolean retain)
    {

        /*
        Client signature:
        uint16_t publish(const char* topic, uint8_t qos, bool retain, const char* payload = nullptr,
                         size_t length = 0, bool dup = false, uint16_t message_id = 0);
        */

        String fullTopic = mqtt_base_topic + String("/") + topic;
        ESP_LOGD(TAG, "Mapping %s to %s", topic, fullTopic.c_str());

        uint16_t returnCode = MQTT::publishRaw(fullTopic, message, qos, retain);
        ESP_LOGI(TAG, "Published message to %s (%d)", fullTopic.c_str(), qos);

        return returnCode;
    }

    uint16_t MQTT::publishRaw(String topic, String message, uint8_t qos, boolean retain)
    {

        uint16_t returnCode = mqttClient.publish(topic.c_str(), qos, retain, message.c_str(), message.length());
        ESP_LOGD(TAG, "Published message to %s (%d)", topic.c_str(), qos);

        return returnCode;
    }

    void MQTT::onConnect(bool sessionPresent)
    {
        ESP_LOGI(TAG, "Connected to MQTT.");
        ESP_LOGV(TAG, "Session present: %b", sessionPresent);
    }

    void MQTT::onDisconnect(AsyncMqttClientDisconnectReason reason)
    {
        ESP_LOGI(TAG, "Disconnected from MQTT.");

        if (WiFi.isConnected())
        {
            xTimerStart(mqttReconnectTimer, 0);
        }
    }

    // Allow others to set their own callback if they want
    void MQTT::onMessage(AsyncMqttClientInternals::OnMessageUserCallback callback)
    {
        mqttClient.onMessage(callback);
    }

    // Provide a logging setup if there's no onMessage()
    void MQTT::defaultOnMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
    {
        ESP_LOGD(TAG, "MQTT message received: topic: %s", topic);
    }

    void MQTT::onSubscribe(uint16_t packetId, uint8_t qos)
    {
        ESP_LOGD(TAG, "Subscribe acknowledged.");
        ESP_LOGV(TAG, "  packetId: %d, qos: %d", packetId, qos);
    }

    void MQTT::onUnsubscribe(uint16_t packetId)
    {
        ESP_LOGI(TAG, "Unsubscribe acknowledged.");
        ESP_LOGV(TAG, "  packetId: %d", packetId);
    }

    void MQTT::onPublish(uint16_t packetId)
    {
        ESP_LOGI(TAG, "Publish acknowledged.");
        ESP_LOGV(TAG, " packetId: %d", packetId);
    }

    void MQTT::onWifiDisconnect()
    {
    }

    void MQTT::startHeartbeat()
    {

        xTaskCreate(creatureHeartBeatTask,
                    "creatureHeartBeatTask",
                    4096,
                    NULL,
                    1,
                    &creatureHeartBeatTaskHandler);
    }

    /**
     * @brief Task to send a pulse to the heartbeat
     *
     */
    portTASK_FUNCTION(creatureHeartBeatTask, pvParameters)
    {

        for (;;)
        {
            ESP_LOGD(TAG, "publishing heartbeat");

            StaticJsonDocument<100> message;
            message["name"] = mqtt_base_name;
            message["time"] = Time::getCurrentTime();

            String json;
            serializeJson(message, json);

            MQTT::publishRaw(HEARTBEAT_TOPIC, json, 0, false);

            vTaskDelay(pdMS_TO_TICKS(15000));
        }
    }

}