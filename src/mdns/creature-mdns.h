
#pragma once

/*

 This is called creature-mdns to avoid a name conflict with the system MDNS

*/

#include <string>
#include <ESPmDNS.h>

#include "esp_log.h"

namespace creatures
{

    /**
     * @brief Wrapper class to make our entries in mDNS be consistent
     * @author Bunny
     */
    class CreatureMDNS
    {

    public:
        CreatureMDNS(std::string creatureName);
        void registerService(uint16_t port);
        void addStandardTags();
        void addServiceText(std::string key, std::string value);

    protected:
        std::string creatureName;
    };

}