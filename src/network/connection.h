
#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <Preferences.h>

extern "C"
{
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
}

#define WIFI_CONFIG_STORAGE_SPACE "creature-wifi"

namespace creatures
{

    class NetworkConnection
    {

    public:
        NetworkConnection();

        static WiFiClass WiFi;

        static TimerHandle_t wifiReconnectTimer;

        static boolean connectToWiFi();
        static void signal_conenction_error();

        void wifi_init();

        static boolean isConnected();
        static void disconnectFromWiFi();

        static void signal_sos();

        // Event handlers
        static void onWifiReady(WiFiEvent_t event, WiFiEventInfo_t info);
        static void onStart(WiFiEvent_t event, WiFiEventInfo_t info);
        static void onConnected(WiFiEvent_t event, WiFiEventInfo_t info);
        static void onGotIP(WiFiEvent_t event, WiFiEventInfo_t info);
        static void onDisconnected(WiFiEvent_t event, WiFiEventInfo_t info);

        //
        // Configuration
        //

        static String getNetworkName();
        static String getWifiPassword();
    };

}