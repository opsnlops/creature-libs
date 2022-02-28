#pragma once

#include <Arduino.h>

extern "C"
{
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/queue.h"
}

#define LOG_LEVEL 1

#define LOGGING_QUEUE_LENGTH 16

namespace creatures
{

    class LogMessage
    {
    public:
        LogMessage(u8_t logLevel, String message);

        String getMessage();
        u8_t getLogLevel();

    protected:
        String message;
        u8_t loglevel;
    };

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
        static void verbose();
        static void debug(String message);
        static void info();
        static void warning();
        static void error();
        static void fatal();

        // Public so the task can get access to it
        static QueueHandle_t logMessageQueue;

    protected:
        static LogMessage createMessageObject(String message);
    };

    void start_log_reader();
    portTASK_FUNCTION_PROTO(logQueueReaderTask, pvParameters);

}