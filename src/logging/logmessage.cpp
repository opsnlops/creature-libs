
#include <Arduino.h>

#include "logging.h"

/**
 * @brief Implemention of one message in the queue
 * 
 * This is one message that goes into the queue. These are mostly for intermal use. I put them
 * in a different file just to keep it clear to me that these objects are _not_ statically
 * scoped.
 */

namespace creatures
{

    LogMessage::LogMessage(u8_t logLevel, String message)
    {
        this->loglevel = logLevel;
        this->message = message;  
    }

    String LogMessage::getMessage()
    {
        return message;
    }

    u8_t LogMessage::getLogLevel()
    {
        return loglevel;
    }
}