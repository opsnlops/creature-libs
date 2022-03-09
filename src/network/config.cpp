

// This isn't gonna work on anything but an ESP32
#if !defined(ESP32)
#error This code is intended to run only on the ESP32 board
#endif

#include <Arduino.h>
#include <Preferences.h>

#include "connection.h"
#include "logging/logging.h"

namespace creatures
{

    static Logger l;
    static Preferences prefs;

    /**
     * @brief Gets the name of the WiFi network from NVS
     *
     * Attempts to load the WiFi name from the MCU's NVS that's stored on flash.
     *
     * @return String with the Wifi name, or a blank string if it's not found
     */
    String NetworkConnection::getNetworkName()
    {
        l.debug("fetching the WiFi network name from NVS");
        prefs.begin(WIFI_CONFIG_STORAGE_SPACE, true);

        String networkName = prefs.getString("ssid", "");
        l.debug("found WiFi network name: %s", networkName.c_str());

        prefs.end();

        return networkName;
    }

    /**
     * @brief Gets the WiFi password from NVS
     *
     * @return String with the WiFi password, or a blank string if it's not found
     */
    String NetworkConnection::getWifiPassword()
    {
        l.debug("fetching the WiFi password name from NVS");
        prefs.begin(WIFI_CONFIG_STORAGE_SPACE, true);

        String networkPassword = prefs.getString("passwd", "");
        l.debug("found a WiFi password that's %d characters long", networkPassword.length());

        prefs.end();

        return networkPassword;
    }

}