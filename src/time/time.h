

#pragma once

#include <Arduino.h>

#include <time.h>
#include <sys/time.h>

#include "esp_log.h"
#include "esp_sntp.h"


#define DEFAULT_TIME_SERVER "10.3.2.6"
#define DEFAULT_TIME_ZONE "PST8PDT,M3.2.0,M11.1.0" // Time zone on Whidbey Island

namespace creatures
{

    class Time
    {

    public:
        static void init();

        static void logTime();

        static void logConfiguredTimeServers();

        static void obtainTime();

        static String getCurrentTime();
        static String getCurrentTime(const char *);
    };

}