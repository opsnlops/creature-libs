#pragma once

#if !defined(ESP32)
#error This code is intended to run only on the ESP32 board
#endif

#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>

#define MAGIC_BROKER_ROLE "magic"

class MagicBroker
{
public:
    MagicBroker();
    MagicBroker(IPAddress _ip, uint16_t _port);

    int operator==(MagicBroker _that);

    IPAddress ipAddress;
    uint16_t port;

    boolean find();
    boolean isValid();
    String getMagicRole();

protected:
    boolean valid;
    IPAddress DEFAULT_IP_ADDRESS = IPAddress(0, 0, 0, 0);
    uint16_t DEFAULT_MQTT_PORT = 1883;
    String BROKER_ROLE = MAGIC_BROKER_ROLE;
};
