
#include <Arduino.h>

extern "C"
{
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/queue.h"
}

#include "logging.h"

#ifndef CREATURE_DEBUG
    #error Please define CREATURE_DEBUG
#endif

namespace creatures
{

    QueueHandle_t Logger::logMessageQueue;

    void Logger::init()
    {

        Serial.begin(MONITOR_BAUD_RATE);
        while (!Serial && !Serial.available())
            ;

        logMessageQueue = xQueueCreate(LOGGING_QUEUE_LENGTH, sizeof(LogMessage));
        start_log_reader();
    }

    /*
        There's gotta be a better way to do this than a bunch of copy/paste things...
    */

    void Logger::verbose(const char *message, ...)
    {
#if CREATURE_DEBUG > 4
        // Copy the arguments to a new va_list
        va_list args;
        va_start(args, message);

        struct LogMessage lm = createMessageObject(LOG_LEVEL_VERBOSE, message, args);
        va_end(args);

        xQueueSend(logMessageQueue, &lm, (TickType_t)10);
#endif
    }

    void Logger::debug(const char *message, ...)
    {
#if CREATURE_DEBUG > 3
        // Copy the arguments to a new va_list
        va_list args;
        va_start(args, message);

        struct LogMessage lm = createMessageObject(LOG_LEVEL_DEBUG, message, args);
        va_end(args);

        xQueueSendToBack(logMessageQueue, &lm, (TickType_t)10);
#endif
    }

    void Logger::info(const char *message, ...)
    {
#if CREATURE_DEBUG > 2
        // Copy the arguments to a new va_list
        va_list args;
        va_start(args, message);

        struct LogMessage lm = createMessageObject(LOG_LEVEL_INFO, message, args);
        va_end(args);

        xQueueSend(logMessageQueue, &lm, (TickType_t)10);
#endif
    }

    void Logger::warning(const char *message, ...)
    {
#if CREATURE_DEBUG > 1
        // Copy the arguments to a new va_list
        va_list args;
        va_start(args, message);

        struct LogMessage lm = createMessageObject(LOG_LEVEL_WARNING, message, args);
        va_end(args);

        xQueueSend(logMessageQueue, &lm, (TickType_t)10);
#endif
    }

    void Logger::error(const char *message, ...)
    {
#if CREATURE_DEBUG > 0
        // Copy the arguments to a new va_list
        va_list args;
        va_start(args, message);

        struct LogMessage lm = createMessageObject(LOG_LEVEL_ERROR, message, args);
        va_end(args);

        xQueueSend(logMessageQueue, &lm, (TickType_t)10);
#endif
    }

    void Logger::fatal(const char *message, ...)
    {
        // Copy the arguments to a new va_list
        va_list args;
        va_start(args, message);

        struct LogMessage lm = createMessageObject(LOG_LEVEL_FATAL, message, args);
        va_end(args);

        xQueueSend(logMessageQueue, &lm, (TickType_t)10);
    }

    LogMessage Logger::createMessageObject(u_int8_t level, const char *message, va_list args)
    {
        char buffer[LOGGING_MESSAGE_MAX_LENGTH + 1];
        memset(buffer, '\0', LOGGING_MESSAGE_MAX_LENGTH + 1);

        vsnprintf(buffer, LOGGING_MESSAGE_MAX_LENGTH, message, args);

        struct LogMessage lm;
        lm.level = level;
        memcpy(lm.message, buffer, LOGGING_MESSAGE_MAX_LENGTH);
        return lm;
    }

}
