#include <Arduino.h>
#include "time.h"

extern "C"
{
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/queue.h"
}

#include "logging.h"

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

    portTASK_FUNCTION(logQueueReaderTask, pvParameters)
    {

        for (;;)
        {
            struct LogMessage lm;
            if (xQueueReceive(Logger::logMessageQueue, &lm, (TickType_t)5000) == pdPASS)
            {

                Serial.print("[");
                Serial.print(millis());
                Serial.print("]");

                // For now just dump it to the console
                if (lm.level == LOG_LEVEL_VERBOSE)
                    Serial.print("[V] ");
                else if (lm.level == LOG_LEVEL_DEBUG)
                    Serial.print("[D] ");
                else if (lm.level == LOG_LEVEL_INFO)
                    Serial.print("[I] ");
                else if (lm.level == LOG_LEVEL_WARNING)
                    Serial.print("[W] ");
                else if (lm.level == LOG_LEVEL_ERROR)
                    Serial.print("[E] ");
                else if (lm.level == LOG_LEVEL_FATAL)
                    Serial.print("[F] ");
                else
                    Serial.print("[?] ");

                Serial.println(lm.message);
            }
        }
    }

}
