
#include <Arduino.h>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

#include "esp_log.h"
#include "esp_sntp.h"

#include "time.h"

using namespace std;

namespace creatures
{

    static const char *TAG = "Time";

    void Time::init()
    {

        String TIME_SERVER = DEFAULT_TIME_SERVER;
        String TIME_ZONE = DEFAULT_TIME_ZONE;

        ESP_LOGD("in Time::init()");
        configTzTime(DEFAULT_TIME_ZONE, const_cast<char *>(TIME_SERVER.c_str()) );

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
        // wait for time to be set
        int retry = 0;
        const int retry_count = 15;
        while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count)
        {
            ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }
        
        String currentTime = getCurrentTime();
        ESP_LOGI(TAG, "The current date/time on Whidbey Island is: %s", currentTime);
    }

    /**
     * @brief Returns the current time using strftime()
     *
     * @param format The time format to use. Defaults to "%c"
     * @return String with the current time
     */
    String Time::getCurrentTime(const char *format)
    {
        ESP_LOGD(TAG, "Getting the current time");
        time_t now = 0;
        struct tm timeinfo = {0};

        // Go fetch the current time from the RTC
        time(&now);

        char strftime_buf[64];
        localtime_r(&now, &timeinfo);
        strftime(strftime_buf, sizeof(strftime_buf), format, &timeinfo);

        return String(strftime_buf);
    }

    String Time::getCurrentTime()
    {
        return getCurrentTime("%c");
    }
}