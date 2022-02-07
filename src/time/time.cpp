
#include <Arduino.h>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <string>

#include "esp_log.h"
#include "esp_sntp.h"

#include "time.h"

using namespace std;

namespace creatures
{

    static const char *TAG = "Time";

    void Time::init()
    {

        string TIME_SERVER = string(DEFAULT_TIME_SERVER);
        string TIME_ZONE = string(DEFAULT_TIME_ZONE);

        ESP_LOGD("in Time::init()");
        sntp_setoperatingmode(SNTP_OPMODE_POLL);
        sntp_setservername(0, const_cast<char *>(TIME_SERVER.c_str()));
        sntp_init();

        // Set the time zone to our local time
        setenv("TZ", DEFAULT_TIME_ZONE, 1);
        tzset();
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
        time_t now = 0;
        struct tm timeinfo = {0};
        int retry = 0;
        const int retry_count = 15;
        while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count)
        {
            ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }
        time(&now);
        localtime_r(&now, &timeinfo);

        char strftime_buf[64];

        localtime_r(&now, &timeinfo);
        strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
        ESP_LOGI(TAG, "The current date/time on Whidbey Island is: %s", strftime_buf);
    }
}