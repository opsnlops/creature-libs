
#include <Arduino.h>

extern "C"
{
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/queue.h"
}

#include "logging.h"

namespace creatures
{


    QueueHandle_t Logger::logMessageQueue;

    void Logger::init()
    {
        logMessageQueue = xQueueCreate(LOGGING_QUEUE_LENGTH, sizeof(LogMessage*));
        start_log_reader();
    }

    void Logger::debug(String message)
    {
        LogMessage localMessage = createMessageObject(message);
        xQueueSend(logMessageQueue, &localMessage, (TickType_t)10);
    }


    LogMessage Logger::createMessageObject(String message)
    {
        return LogMessage(LOG_LEVEL, message);
    } 
   
}
