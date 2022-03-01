

#include <Arduino.h>
#include <ESPmDNS.h>

#include "esp_log.h"

#include "creature-mdns.h"
#include "logging/logging.h"

static const char *TAG = "mdns";

namespace creatures
{

    static Logger l;

    CreatureMDNS::CreatureMDNS(String creatureName, String powerType)
    {
        // Save this for later!
        this->creatureName = creatureName;
        this->powerType = powerType;

        l.debug("Attempting to register %s in mDNS", creatureName);
        if (!MDNS.begin(creatureName.c_str()))
        {
            l.error("Error setting up MDNS responder!");
        }
        l.info("MDNS set up");
    }

    void CreatureMDNS::registerService(uint16_t port)
    {
        l.debug("Attempting to register port %d in mDNS", port);
        MDNS.addService("creatures", "tcp", port);
    }

    void CreatureMDNS::addStandardTags()
    {
        l.debug("Adding standard mDNS services");
        addServiceText("board", ARDUINO_BOARD);
        addServiceText("variant", ARDUINO_VARIANT);
        addServiceText("name", creatureName);
        addServiceText("arduino", String(ARDUINO));
        addServiceText("power", powerType);
    }

    void CreatureMDNS::addServiceText(String key, String value)
    {
        l.debug("Adding mDNS text record: key %s, value %s", key.c_str(), value.c_str());
        MDNS.addServiceTxt("creatures", "tcp", key, value);
    }

}