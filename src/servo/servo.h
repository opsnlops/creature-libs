
#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

/**
 * @brief Servo control library for creatures
 *
 * Allow creatures to move!
 *
 */

#include "logging/logging.h"

// Taken from the Adafruit docs. These might need to be adjusted
#define SERVOMIN 110  // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX 800 // This is the 'maximum' pulse length count (out of 4096)
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates

namespace creatures
{
    class CreatureServos
    {
    public:
        CreatureServos(u8_t numberOfServos);
        void init();

        void moveServo(u8_t serverNumber, u8_t position);

        u8_t getNumberOfServos();

    private:
        u8_t numberOfServos;
        Adafruit_PWMServoDriver* pwmServerDriver;
    };
}