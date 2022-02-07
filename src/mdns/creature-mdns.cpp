

#include <ESPmDNS.h>

#include "esp_log.h"

#include "creature-mdns.h"

namespace creatures
{

    CreatureMDNS::CreatureMDNS(std::string creatureName)
    {
        // Save this for later!
        this->creatureName = creatureName;

        ESP_LOGD(TAG, "Attempting to register %s in mDNS", creatureName.c_str());
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
    }

    void CreatureMDNS::addServiceText(std::string key, std::string value)
    {
        ESP_LOGD(TAG, "Adding mDNS text record: key %s, value %s", key.c_str(), value.c_str());
        MDNS.addServiceTxt("creatures", "tcp", key.c_str(), value.c_str());
    }

}