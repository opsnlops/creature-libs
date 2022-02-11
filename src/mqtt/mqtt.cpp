
#include <Arduino.h>

#include <AsyncMqttClient.h>
#include <WiFi.h>

#include "mqtt.h"

namespace creatures
{

    static TimerHandle_t mqttReconnectTimer;
    static AsyncMqttClient mqttClient;
    static IPAddress mqtt_broker_address;
    static uint16_t mqtt_broker_port;

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
        ESP_LOGI(TAG, "Subcribing to %s (%d)", topic, qos);
        mqttClient.subscribe(topic.c_str(), qos);
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

}