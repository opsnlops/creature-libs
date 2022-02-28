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

    TaskHandle_t logQueueReaderTaskHandler;

    void start_log_reader()
    {
        xTaskCreate(logQueueReaderTask,
                    "logQueueReaderTask",
                    4096,
                    NULL,
                    1,
                    &logQueueReaderTaskHandler);
    }

    portTASK_FUNCTION(logQueueReaderTask, pvParameters)
    {

        for (;;)
        {
            LogMessage *lm = NULL;
            xQueueReceive(Logger::logMessageQueue, &lm, (TickType_t)10);

            // For now just dump it to the console
            Serial.println(lm->getMessage());

            delete lm;
        }
    }

}
