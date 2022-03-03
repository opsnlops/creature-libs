
#include <Arduino.h>
#include <ArduinoJson.h>

#include <AsyncMqttClient.h>
#include <WiFi.h>

#include "mqtt.h"

#include "time/time.h"
#include "logging/logging.h"

namespace creatures
{
    static Logger l;
    static QueueHandle_t incomingMessageQueue;
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
        l.verbose("set mqtt_base_name to %s", mqtt_base_name);

        mqtt_base_topic = String("creatures/") + mqtt_base_name;
        l.verbose("set mqtt_base_topic to %s", mqtt_base_topic.c_str());

        incomingMessageQueue = xQueueCreate(MQTT_QUEUE_LENGTH, sizeof(MqttMessage));
        l.debug("incoming message queue made");
    }

    /**
     * @brief Returns a pointer to the queue of `MqttMessage` structs with our incoming message
     *
     * @return QueueHandle_t* a pointer to the internal message queue
     */
    QueueHandle_t MQTT::getIncomingMessageQueue()
    {
        l.debug("returning the incomingMessageQueue");
        return incomingMessageQueue;
    }

    void MQTT::connect(IPAddress _mqtt_broker_address, uint16_t _mqtt_broker_port)
    {

        mqtt_broker_address = _mqtt_broker_address;
        mqtt_broker_port = _mqtt_broker_port;

        l.debug("Setting up the MQTT client");

        mqttClient.setServer(mqtt_broker_address, mqtt_broker_port);
        mqttClient.onConnect(onConnect);
        mqttClient.onDisconnect(onDisconnect);
        mqttClient.onSubscribe(onSubscribe);
        mqttClient.onUnsubscribe(onUnsubscribe);
        mqttClient.onPublish(onPublish);
        mqttClient.onMessage(onMessage);

        l.info("Connecting to MQTT (%s:%d)...", mqtt_broker_address.toString(), mqtt_broker_port);
        mqttClient.connect();

        /*
            Bit of a weird thing here. connect() will return before it's actually connected. (I guess that's
            okay, it does say it's async!) Soooo if we return right away, any attempts to subscribe to topics
            will fail.

            So let's spin for a bit and wait for the client to know it's up and running.
        */

        l.debug("connect() returned, waiting for client to admit it");

        // wait for the client to know it's connecting
        int waitCount = 0;
        while (!mqttClient.connected() || waitCount++ > 100)
        {
            l.debug("waiting...");
            vTaskDelay(pdMS_TO_TICKS(50));
        }

        // Leave a log message if we can't connect
        if (mqttClient.connected())
        {
            l.info("connected!");
        }
        else
        {
            l.error("Unable to connect to MQTT!");
        }
    }

    /**
     * @brief Subscribed to a topic that's localized to this creature
     *
     * Whatever `topic` is passed in will have `mqtt_base_topic` added to it before
     * it's passed to the MQTT client.
     *
     * It's ideal to limit MQTT subscriptions to things localized for each creature so
     * I can replicate creatures easily. If there is a need to subscribe to a topic in
     * the global namespace, see `subscribeGlobalNamespace()`.
     *
     * @param topic The topic to subscribe to
     * @param qos QOS passed to MQTT
     */
    void MQTT::subscribe(String topic, uint8_t qos)
    {
        String fullTopic = mqtt_base_topic + String("/") + topic;
        l.info("Mapping %s to %s", topic, fullTopic.c_str());

        subscribeGlobalNamespace(fullTopic, qos);
    }

    /**
     * @brief Subscribes to a topic in the global namespace
     *
     * The subscription will be placed with the exact `topic` that's passed in to
     * this method.
     *
     * Use subscriptions to the global namespace with care, it's better to localize thing
     * to each creature.
     *
     * @param topic The topic in the global namespace to subscribe to
     * @param qos QOS passed to QMTT
     */
    void MQTT::subscribeGlobalNamespace(String topic, uint8_t qos)
    {
        l.info("Subscribing to %s (%d)", topic.c_str(), qos);
        mqttClient.subscribe(topic.c_str(), qos);
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

       l.verbose("publish: topic: %s, message: %s", topic, message);


        String fullTopic = mqtt_base_topic + String("/") + topic;
        l.debug("Mapping %s to %s", topic, fullTopic.c_str());

        uint16_t returnCode = MQTT::publishRaw(fullTopic, message, qos, retain);
        l.info("Published message to %s (%d)", fullTopic.c_str(), qos);

        return returnCode;
    }

    uint16_t MQTT::publishRaw(String topic, String message, uint8_t qos, boolean retain)
    {

        uint16_t returnCode = mqttClient.publish(topic.c_str(), qos, retain, message.c_str(), message.length());
        l.debug("Published message to %s (%d)", topic.c_str(), qos);

        return returnCode;
    }

    void MQTT::onConnect(bool sessionPresent)
    {
        l.debug("in onConnect()");
        l.verbose("Session present: %b", sessionPresent);
    }

    void MQTT::onDisconnect(AsyncMqttClientDisconnectReason reason)
    {
        l.info("Disconnected from MQTT.");

        // if (WiFi.isConnected())
        //{
        //     xTimerStart(mqttReconnectTimer, 0);
        // }
    }

    /**
     * @brief Tosses a message from MQTT into the message queue
     *
     * @param topic
     * @param payload
     * @param properties
     * @param len
     * @param index
     * @param total
     */
    void MQTT::onMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
    {
        // Blink the LED while working
        digitalWrite(LED_BUILTIN, HIGH);

        l.info("in onMessage()");
        l.debug("MQTT message received: topic: %s", topic);
        l.verbose("Incoming message: %s", payload);

        // Create some buffers and make sure we've got \0 on the end
        char topicBuffer[MQTT_MAX_TOPIC_LENGTH + 1];
        memset(topicBuffer, '\0', MQTT_MAX_TOPIC_LENGTH + 1);

        char payloadBuffer[MQTT_MAX_PAYLOAD_LENGTH + 1];
        memset(payloadBuffer, '\0', MQTT_MAX_PAYLOAD_LENGTH + 1);

        // Make sure we're not going to over-run a buffer
        int topicLength = strlen(topic);
        if (topicLength > MQTT_MAX_TOPIC_LENGTH)
        {
            l.info("topic is %d length, truncating to %d", topicLength, MQTT_MAX_TOPIC_LENGTH);
            topicLength = MQTT_MAX_TOPIC_LENGTH;
        }
        else
        {
            l.debug("topic length: %d", topicLength);
        }

        int payloadLength = len;
        if(payloadLength > MQTT_MAX_PAYLOAD_LENGTH)
        {
            l.info("payload is %d length, truncating to %d", payloadLength, MQTT_MAX_PAYLOAD_LENGTH);
            payloadLength = MQTT_MAX_PAYLOAD_LENGTH;
        }
        else
        {
            l.debug("payload length: %d", payloadLength);
        }

        struct MqttMessage message;
        memcpy(message.topic, topic, topicLength + 1);
        memcpy(message.payload, payload, payloadLength + 1);

        // Bye message! have fun in the queue!
        xQueueSendToBackFromISR(incomingMessageQueue, &message, NULL);

        l.debug("enqueued incoming message");
        digitalWrite(LED_BUILTIN, LOW);
    }

    void MQTT::onSubscribe(uint16_t packetId, uint8_t qos)
    {
        l.debug("Subscribe acknowledged.");
        l.verbose("  packetId: %d, qos: %d", packetId, qos);
    }

    void MQTT::onUnsubscribe(uint16_t packetId)
    {
        l.info("Unsubscribe acknowledged.");
        l.verbose("  packetId: %d", packetId);
    }

    void MQTT::onPublish(uint16_t packetId)
    {
        l.info("Publish acknowledged.");
        l.verbose(" packetId: %d", packetId);
    }

    void MQTT::onWifiDisconnect()
    {
    }

    void MQTT::startHeartbeat()
    {

        xTaskCreate(creatureHeartBeatTask,
                    "creatureHeartBeatTask",
                    5120,
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
            l.debug("publishing heartbeat");

            StaticJsonDocument<130> message;
            message["name"] = mqtt_base_name;
            message["time"] = Time::getCurrentTime();

            // Show the uptime in seconds
            message["uptime"] = esp_timer_get_time() / 1000000;

            String json;
            serializeJson(message, json);

            MQTT::publishRaw(HEARTBEAT_TOPIC, json, 0, false);

            vTaskDelay(pdMS_TO_TICKS(15000));
        }
    }

}