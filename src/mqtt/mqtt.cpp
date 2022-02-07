
#include <Arduino.h>

#include <AsyncMqttClient.h>
#include <WiFi.h>

#include "mqtt.h"

namespace creatures
{

    void MQTT::configure(IPAddress _mqtt_broker_address, uint16_t _mqtt_broker_port)
    {
        mqtt_broker_address = _mqtt_broker_address;
        mqtt_broker_port = _mqtt_broker_port;

        ESP_LOGI(TAG, "The IP of the broker is %s, port: %d", mqtt_broker_address.toString(), mqtt_broker_port);
    }

    void MQTT::connectToMqtt()
    {

        mqttClient.setServer(mqtt_broker_address, mqtt_broker_port);
        mqttClient.onConnect(onMqttConnect);
        mqttClient.onDisconnect(onMqttDisconnect);
        mqttClient.onSubscribe(onMqttSubscribe);
        mqttClient.onUnsubscribe(onMqttUnsubscribe);
        mqttClient.onPublish(onMqttPublish);

        ESP_LOGI(TAG, "Connecting to MQTT...");
        mqttClient.connect();
        ESP_LOGI(TAG, "connected!");
    }

    void MQTT::onMqttConnect(bool sessionPresent)
    {
        ESP_LOGI(TAG, "Connected to MQTT.");
        ESP_LOGV(TAG, "Session present: %b", sessionPresent);

        // Listen for the magic heartbeat
        mqttClient.subscribe(HEARTBEAT_TOPIC, 0);
    }

    void MQTT::onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
    {
        ESP_LOGI(TAG, "Disconnected from MQTT.");

        if (WiFi.isConnected())
        {
            xTimerStart(mqttReconnectTimer, 0);
        }
    }

    void MQTT::onMqttSubscribe(uint16_t packetId, uint8_t qos)
    {
        ESP_LOGD(TAG, "Subscribe acknowledged.");
        ESP_LOGV(TAG, "  packetId: %d, qos: %d", packetId, qos);
    }

    void MQTT::onMqttUnsubscribe(uint16_t packetId)
    {
        ESP_LOGI(TAG, "Unsubscribe acknowledged.");
        ESP_LOGV(TAG, "  packetId: %d", packetId);
    }

    void MQTT::onMqttPublish(uint16_t packetId)
    {
        ESP_LOGI(TAG, "Publish acknowledged.");
        ESP_LOGV(TAG, " packetId: %d", packetId);
    }

    void MQTT::onWifiDisconnect()
    {
    }

}