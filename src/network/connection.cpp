

// This isn't gonna work on anything but an ESP32
#if !defined(ESP32)
#error This code is intended to run only on the ESP32 board
#endif

#include <Arduino.h>
#include <WiFi.h>
extern "C"
{
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
}

#include "esp_log.h"

#include "connection.h"
#include "secrets.h"
#include "logging/logging.h"

// Report the state of the WiFi to a global var
extern boolean gWifiConnected;

namespace creatures
{

    static Logger l;
    WiFiClass NetworkConnection::WiFi;
    TimerHandle_t NetworkConnection::wifiReconnectTimer;

    NetworkConnection::NetworkConnection()
    {
        wifi_init();
    }

    void NetworkConnection::wifi_init()
    {
        l.verbose("enter NetworkConnection::wifi_init()");
        WiFi.mode(WIFI_MODE_STA);
        WiFi.onEvent(onWifiReady, ARDUINO_EVENT_WIFI_READY);
        WiFi.onEvent(onStart, ARDUINO_EVENT_WIFI_STA_START);
        WiFi.onEvent(onConnected, ARDUINO_EVENT_WIFI_STA_CONNECTED);
        WiFi.onEvent(onGotIP, ARDUINO_EVENT_WIFI_STA_GOT_IP);
        WiFi.onEvent(onDisconnected, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
        wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(30000), pdFALSE, (void *)0, reinterpret_cast<TimerCallbackFunction_t>(connectToWiFi));

        l.verbose("leave NetworkConnection::wifi_init()");
    }

    void NetworkConnection::connectToWiFi()
    {
        l.info("Connecting to WiFi network: %s", WIFI_NETWORK);
        l.debug("My MAC address: %s", WiFi.macAddress().c_str());
        int attempt = 0;

        WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);
        while (!WiFi.isConnected())
        {
            l.debug(" not yet! attempt: %d", attempt);
            switch (WiFi.status())
            {
            case (WL_IDLE_STATUS):
                l.debug("  reason: idle");
                break;
            case (WL_NO_SSID_AVAIL):
                l.debug("  reason: no SSID available");
                break;
            case (WL_SCAN_COMPLETED):
                l.debug("  reason: Scan Completed");
                break;
            case (WL_CONNECTED):
                l.debug("  reason: connected (what?)");
                break;
            case (WL_CONNECT_FAILED):
                l.debug("  reason: connect failed");
                break;
            case (WL_CONNECTION_LOST):
                l.debug("  reason: connection lost");
                break;
            case (WL_DISCONNECTED):
                l.debug("  reason: disconnected");
                break;
            default:
                l.debug(" reason: unknown (%d)", WiFi.status());
                break;
            }

            vTaskDelay(pdMS_TO_TICKS(2000));

            // Only wait so long
            if (attempt++ == 50)
            {
                signal_sos();
            }
        }
        l.info("connected to wifi");
    }

    /**
     * @brief Disconnects from the WiFi network
     *
     */
    void NetworkConnection::disconnectFromWiFi()
    {
        l.debug("Disconnecting from Wifi network %s", WIFI_NETWORK);

        if (NetworkConnection::isConnected())
        {
            WiFi.disconnect();
            l.info("Disconnected from Wifi network %s", WIFI_NETWORK);
        }
    }

    boolean NetworkConnection::isConnected()
    {
        return (WiFi.status() == WL_CONNECTED);
    }

    //
    // Event Handlers
    //
    void NetworkConnection::onWifiReady(WiFiEvent_t event, WiFiEventInfo_t info)
    {
        l.debug("Wifi Ready");
    }

    void NetworkConnection::onStart(WiFiEvent_t event, WiFiEventInfo_t info)
    {
        l.debug("WiFi started");
    }

    void NetworkConnection::onConnected(WiFiEvent_t event, WiFiEventInfo_t info)
    {
        // Flag that we're online
        gWifiConnected = true;
        l.info("WiFi connected");
    }

    void NetworkConnection::onGotIP(WiFiEvent_t event, WiFiEventInfo_t info)
    {
        l.info("IP address: %s", WiFi.localIP().toString().c_str());
    }

    void NetworkConnection::onDisconnected(WiFiEvent_t event, WiFiEventInfo_t info)
    {
        // Flag that we're now offline
        gWifiConnected = false;
        l.error("WiFi lost connection! Reason: %d", info.wifi_sta_disconnected.reason);
        // xTimerStart(wifiReconnectTimer, 0);
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