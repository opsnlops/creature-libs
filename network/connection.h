
#pragma once

#include <WiFi.h>


class NetworkConnection
{

public:

    NetworkConnection();

    static WiFiClass WiFi;

    static TimerHandle_t wifiReconnectTimer;

    static void connectToWiFi();
    static void signal_conenction_error();

    void wifi_init();

    static void signal_sos();

    // Event handlers
    static void onWifiReady(WiFiEvent_t event, WiFiEventInfo_t info);
    static void onStart(WiFiEvent_t event, WiFiEventInfo_t info);
    static void onConnected(WiFiEvent_t event, WiFiEventInfo_t info);
    static void onGotIP(WiFiEvent_t event, WiFiEventInfo_t info);
    static void onDisconnected(WiFiEvent_t event, WiFiEventInfo_t info);

};
