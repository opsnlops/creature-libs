

// This isn't gonna work on anything but an ESP32
#if !defined(ESP32)
#error This code is intended to run only on the ESP32 board
#endif

#include <WiFi.h>
extern "C"
{
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
}

#include "esp_log.h"

#include "connection.h"
#include "secrets.h"

namespace creatures
{

    static const char *TAG = "network";

    WiFiClass NetworkConnection::WiFi;
    TimerHandle_t NetworkConnection::wifiReconnectTimer;

    NetworkConnection::NetworkConnection()
    {
        wifi_init();
    }

    void NetworkConnection::wifi_init()
    {
        ESP_LOGV(TAG, "enter NetworkConnection::wifi_init()");
        WiFi.mode(WIFI_STA);
        WiFi.onEvent(onWifiReady, SYSTEM_EVENT_WIFI_READY);
        WiFi.onEvent(onStart, SYSTEM_EVENT_STA_START);
        WiFi.onEvent(onConnected, SYSTEM_EVENT_STA_CONNECTED);
        WiFi.onEvent(onGotIP, SYSTEM_EVENT_STA_GOT_IP);
        WiFi.onEvent(onDisconnected, SYSTEM_EVENT_STA_DISCONNECTED);
        wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void *)0, reinterpret_cast<TimerCallbackFunction_t>(connectToWiFi));

        ESP_LOGV(TAG, "leave NetworkConnection::wifi_init()");
    }

    void NetworkConnection::connectToWiFi()
    {
        ESP_LOGI(TAG, "Connecting to WiFi network: %s", WIFI_NETWORK);

        int attempt = 0;

        WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
            ESP_LOGD(TAG, " (not yet - %d)", attempt);

            // Only wait so long
            if (attempt++ == 50)
            {
                signal_sos();
            }
        }
        ESP_LOGI(TAG, "connected to wifi");
    }

    //
    // Event Handlers
    //
    void NetworkConnection::onWifiReady(WiFiEvent_t event, WiFiEventInfo_t info)
    {
        ESP_LOGD(TAG, "Wifi Ready");
    }

    void NetworkConnection::onStart(WiFiEvent_t event, WiFiEventInfo_t info)
    {
        ESP_LOGD(TAG, "WiFi started");
    }

    void NetworkConnection::onConnected(WiFiEvent_t event, WiFiEventInfo_t info)
    {
        ESP_LOGI(TAG, "WiFi connected");
    }

    void NetworkConnection::onGotIP(WiFiEvent_t event, WiFiEventInfo_t info)
    {
        ESP_LOGI(TAG, "IP address: %s", WiFi.localIP().toString().c_str());
    }

    void NetworkConnection::onDisconnected(WiFiEvent_t event, WiFiEventInfo_t info)
    {
        ESP_LOGE(TAG, "WiFi lost connection");
        xTimerStart(wifiReconnectTimer, 0);
    }

    /**
     * This tosses SOS onto the internal LED. It's a dead loop, and means we couldn't connect.
     */
    void NetworkConnection::signal_sos()
    {

        const int LED_PIN = LED_BUILTIN;

        while (true)
        {
            digitalWrite(LED_PIN, HIGH);
            delay(100);
            digitalWrite(LED_PIN, LOW);
            delay(100);
            digitalWrite(LED_PIN, HIGH);
            delay(100);
            digitalWrite(LED_PIN, LOW);
            delay(100);
            digitalWrite(LED_PIN, HIGH);
            delay(100);
            digitalWrite(LED_PIN, LOW);
            delay(400);

            digitalWrite(LED_PIN, HIGH);
            delay(500);
            digitalWrite(LED_PIN, LOW);
            delay(200);
            digitalWrite(LED_PIN, HIGH);
            delay(500);
            digitalWrite(LED_PIN, LOW);
            delay(200);
            digitalWrite(LED_PIN, HIGH);
            delay(500);
            digitalWrite(LED_PIN, LOW);
            delay(400);

            digitalWrite(LED_PIN, HIGH);
            delay(100);
            digitalWrite(LED_PIN, LOW);
            delay(100);
            digitalWrite(LED_PIN, HIGH);
            delay(100);
            digitalWrite(LED_PIN, LOW);
            delay(100);
            digitalWrite(LED_PIN, HIGH);
            delay(100);
            digitalWrite(LED_PIN, LOW);
            delay(2000);
        }
    }

}