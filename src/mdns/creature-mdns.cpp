

#include <Arduino.h>
#include <ESPmDNS.h>

#include "esp_log.h"

#include "creature-mdns.h"

namespace creatures
{

    CreatureMDNS::CreatureMDNS(String creatureName)
    {
        // Save this for later!
        this->creatureName = creatureName;

        ESP_LOGD(TAG, "Attempting to register %s in mDNS", creatureName);
        if (!MDNS.begin(creatureName.c_str()))
        {
            ESP_LOGE(TAG, "Error setting up MDNS responder!");
        }
        ESP_LOGI(TAG, "MDNS set up");
    }

    void CreatureMDNS::registerService(uint16_t port)
    {
        ESP_LOGD(TAG, "Attempting to register port %d in mDNS", port);
        MDNS.addService("creatures", "tcp", port);
    }

    void CreatureMDNS::addStandardTags()
    {
        ESP_LOGD(TAG, "Adding standard mDNS services");
        addServiceText("board", ARDUINO_BOARD);
        addServiceText("variant", ARDUINO_VARIANT);
        addServiceText("name", creatureName);
        addServiceText("arduino", String(ARDUINO));
    }

    void CreatureMDNS::addServiceText(String key, String value)
    {
        ESP_LOGD(TAG, "Adding mDNS text record: key %s, value %s", key.c_str(), value.c_str());
        MDNS.addServiceTxt("creatures", "tcp", key, value);
    }

}