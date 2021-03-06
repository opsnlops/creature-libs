#pragma once

#include <Arduino.h>
#include "time.h"

extern "C"
{
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/queue.h"
}

#define LOG_LEVEL_VERBOSE 5
#define LOG_LEVEL_DEBUG 4
#define LOG_LEVEL_INFO 3
#define LOG_LEVEL_WARNING 2
#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_FATAL 0

#define LOGGING_QUEUE_LENGTH 16

#define LOGGING_MESSAGE_MAX_LENGTH 512

#define MONITOR_BAUD_RATE 115200

#ifdef CREATURE_LOG_SYSLOG

// Most likely this should come from mDNS at some point?
#define SYSLOG_SERVER "10.9.1.5"
#define SYSLOG_PORT 514

#endif

namespace creatures
{

    struct LogMessage
    {
        u_int8_t level;
        char message[LOGGING_MESSAGE_MAX_LENGTH];
    } __attribute__((packed));

    /**
     * @brief Creature debug logging
     *
     * This class provides a wrapper around a logging framework. It allows me to not
     * depend on someone else's logging framework in my code, so I can change the
     * logging around as I see fit without a bunch of code changes.
     */
    class Logger
    {
    public:
        static void init();
        static void verbose(const char *message, ...);
        static void debug(const char *message, ...);
        static void info(const char *message, ...);
        static void warning(const char *message, ...);
        static void error(const char *message, ...);
        static void fatal(const char *message, ...);

        // Public so the task can get access to it
        static QueueHandle_t logMessageQueue;

    private:
        static LogMessage createMessageObject(u_int8_t level, const char *message, va_list args);
        static String creatureName;
    };

    void start_log_reader();
    portTASK_FUNCTION_PROTO(logQueueReaderTask, pvParameters);

}