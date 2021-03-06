
#include "magicbroker.h"
#include "logging/logging.h"

namespace creatures
{
    static Logger l;

    MagicBroker::MagicBroker()
    {
        MagicBroker(MagicBroker::DEFAULT_IP_ADDRESS, MagicBroker::DEFAULT_MQTT_PORT);
    }

    MagicBroker::MagicBroker(IPAddress _ip, uint16_t _port)
    {
        ipAddress = _ip;
        port = _port;
        valid = false;
    }

    int MagicBroker::operator==(MagicBroker _that)
    {
        if (_that.port == port && _that.ipAddress == ipAddress)
            return 1;
        else
            return 0;
    }

    boolean MagicBroker::isValid()
    {
        return valid;
    }

    String MagicBroker::getMagicRole()
    {
        return BROKER_ROLE.c_str();
    }

    boolean MagicBroker::find()
    {
        l.verbose("enter MagicBroker::find()");
        valid = false;

        l.info("looking for the magic broker");

        int n = MDNS.queryService("mqtt", "tcp");
        if (n == 0)
        {
            l.error("couldn't find the magic broker in mDNS");
        }
        else
        {
            l.debug("%d services(s) found", n);
            for (int i = 0; i < n; ++i)
            {
                String _hostname = MDNS.hostname(i).c_str();
                String _role = MDNS.txt(i, "role").c_str();
                IPAddress _ipAddress = MDNS.IP(i);
                int _port = MDNS.port(i);

                // Print details for each service found
                l.debug("  %d: %s (%s:%d) role: %s",
                        (i + 1),
                        _hostname,
                        _ipAddress.toString().c_str(),
                        _port,
                        _role);

                if (_role == MagicBroker::BROKER_ROLE)
                {
                    this->ipAddress = _ipAddress;
                    this->port = _port;
                    l.info("Found the magic broker! IP: %s, port: %d", ipAddress.toString().c_str(), port);

                    valid = true;
                }
            }
        }

        if (!valid)
        {
            l.warning("unable to find the magic broker (role '%s')", BROKER_ROLE.c_str());
        }

        l.verbose("leave MagicBroker::find()");
        return valid;
    }

}