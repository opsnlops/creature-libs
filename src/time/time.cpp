
#include <Arduino.h>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

#include "esp_log.h"
#include "esp_sntp.h"

#include "time.h"

using namespace std;

static const char *TAG = "time";

namespace creatures
{

    static const char *TAG = "Time";

    static ip_addr_t creatureTimeServer = IPADDR4_INIT_BYTES(10, 3, 2, 6);

    void Time::init()
    {
        ESP_LOGD(TAG, "in Time::init()");
        sntp_setoperatingmode(SNTP_OPMODE_POLL);
        sntp_setserver(0, &creatureTimeServer);
        sntp_set_sync_interval(900000);             // Sync every 15 minutes 
        sntp_init();
        setenv("TZ", DEFAULT_TIME_ZONE, 1);
        tzset();

        ESP_LOGI(TAG, "Time inited");
    }

    void Time::logConfiguredTimeServers()
    {
        ESP_LOGI(TAG, "List of configured NTP servers:");
        for (uint8_t i = 0; i < SNTP_MAX_SERVERS; ++i)
        {
            if (sntp_getservername(i))
            {
                ESP_LOGI(TAG, "server %d: %s", i, sntp_getservername(i));
            }
            else
            {
                // we have either IPv4 or IPv6 address, let's print it
                char buff[64];
                ip_addr_t const *ip = sntp_getserver(i);
                if (ipaddr_ntoa_r(ip, buff, 64) != NULL)
                    ESP_LOGI(TAG, "server %d: %s", i, buff);
            }
        }
    }

    void Time::obtainTime(void)
    {
        // Debugging time sync
        logConfiguredTimeServers();

        // wait for time to be set
        int retry = 0;
        const int retry_count = 15;
        while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count)
        {
            ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
            vTaskDelay(pdMS_TO_TICKS(2000));
        }
        ESP_LOGI(TAG, "Time obtained");
    }

    /**
     * @brief Returns the current time using strftime()
     *
     * @param format The time format to use. Defaults to "%c"
     * @return String with the current time
     */
    String Time::getCurrentTime(const char *format)
    {
        ESP_LOGV(TAG, "Getting the current time");
        struct tm timeinfo;

        // Go fetch the current time from the RTC
        getLocalTime(&timeinfo);

        char strftime_buf[64];
        strftime(strftime_buf, sizeof(strftime_buf), format, &timeinfo);

        return String(strftime_buf);
    }

    String Time::getCurrentTime()
    {
        return getCurrentTime("%c");
    }
}