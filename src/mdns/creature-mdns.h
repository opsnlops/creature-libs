
#pragma once

/*

    This is called creature-mdns to avoid a name conflict with the system MDNS

*/

#include <Arduino.h>
#include <ESPmDNS.h>

#include "logging/logging.h"

namespace creatures
{

    /**
     * @brief Wrapper class to make our entries in mDNS be consistent
     * @author Bunny
     */
    class CreatureMDNS
    {
        static Logger l;

    public:
        CreatureMDNS(String creatureName, String powerType);
        void registerService(uint16_t port);
        void addStandardTags();
        void addServiceText(String key, String value);

    protected:
        String creatureName;
        String powerType;
    };

}