#include <Arduino.h>
#include "time.h"

extern "C"
{
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/queue.h"
}

#include "logging.h"

/**
 * @brief Only include the syslog stuff if it's on in the config
 */
#ifdef CREATURE_LOG_SYSLOG

#include <Syslog.h>
#include <WiFiUdp.h>
#include <WiFi.h>

// A UDP instance to let us send and receive packets over UDP
WiFiUDP udpClient;

// Keep track of if the WiFi is connected
extern boolean gWifiConnected;
extern const char *gCreatureName;

// Let's use LOCAL0 for creatures
Syslog syslog(udpClient, SYSLOG_SERVER, SYSLOG_PORT, gCreatureName, NULL, LOG_LOCAL0);

#endif

namespace creatures
{

    TaskHandle_t logQueueReaderTaskHandler;

    void start_log_reader()
    {
        xTaskCreate(logQueueReaderTask,
                    "logQueueReaderTask",
                    20480,
                    NULL,
                    1,
                    &logQueueReaderTaskHandler);
    }

    /**
     * @brief Creates a task that polls the logging queue
     *
     * It then spits things to the Serial port, and optionally to syslog so that a
     * Linux host can handle the heavy lifting.
     */
    portTASK_FUNCTION(logQueueReaderTask, pvParameters)
    {

        for (;;)
        {
            struct LogMessage lm;
            char buffer[LOGGING_MESSAGE_MAX_LENGTH + 16 + 1];
#ifdef CREATURE_LOG_SYSLOG
            uint8_t syslogLevel = 0;
#endif
            char levelBuffer[5];
            if (xQueueReceive(Logger::logMessageQueue, &lm, (TickType_t)5000) == pdPASS)
            {
                // Blank out the buffer
                memset(buffer, '\0', LOGGING_MESSAGE_MAX_LENGTH + 16 + 1);
#ifdef CREATURE_LOG_SYSLOG
                memset(levelBuffer, '\0', 5);
#endif

                // For now just dump it to the console
                if (lm.level == LOG_LEVEL_VERBOSE)
                {
#ifdef CREATURE_LOG_SYSLOG
                    // Syslog doesn't have a "verbose"
                    syslogLevel = LOG_DEBUG;
#endif
                    strncpy(levelBuffer, "[V] ", 3);
                }
                else if (lm.level == LOG_LEVEL_DEBUG)
                {
#ifdef CREATURE_LOG_SYSLOG
                    syslogLevel = LOG_DEBUG;
#endif
                    strncpy(levelBuffer, "[D]", 3);
                }
                else if (lm.level == LOG_LEVEL_INFO)
                {
#ifdef CREATURE_LOG_SYSLOG
                    syslogLevel = LOG_INFO;
#endif
                    strncpy(levelBuffer, "[I]", 3);
                }
                else if (lm.level == LOG_LEVEL_WARNING)
                {
#ifdef CREATURE_LOG_SYSLOG
                    syslogLevel = LOG_WARNING;
#endif
                    strncpy(levelBuffer, "[W]", 3);
                }
                else if (lm.level == LOG_LEVEL_ERROR)
                {
#ifdef CREATURE_LOG_SYSLOG
                    syslogLevel = LOG_ERR;
#endif
                    strncpy(levelBuffer, "[E]", 3);
                }
                else if (lm.level == LOG_LEVEL_FATAL)
                {
#ifdef CREATURE_LOG_SYSLOG
                    syslogLevel = LOG_EMERG;
#endif
                    strncpy(levelBuffer, "[F]", 3);
                }
                else
                {
#ifdef CREATURE_LOG_SYSLOG
                    syslogLevel = LOG_NOTICE;
#endif
                    strncpy(levelBuffer, "[?]", 3);
                }

                // Format our message
                snprintf(buffer, LOGGING_MESSAGE_MAX_LENGTH + 16, "[%d]%s %s", millis(), levelBuffer, lm.message);

#ifdef CREATURE_LOG_SERIAL
                // Print to the serial port
                Serial.println(buffer);
#endif

#ifdef CREATURE_LOG_SYSLOG
                // Log to syslog
                if (gWifiConnected)
                    syslog.log(syslogLevel, buffer);
#endif
            }
        }
    }

}
