
#include "servo.h"
#include "logging/logging.h"

namespace creatures
{
    static Logger l;

    CreatureServos::CreatureServos(u8_t numberOfServos)
    {
        this->numberOfServos = numberOfServos;
        l.info("Created a new CreatureServo with %d servos", numberOfServos);
    }

    void CreatureServos::init()
    {
        l.debug("in CreatureServos::init()");

        pwmServerDriver = new Adafruit_PWMServoDriver();
        l.verbose("created the server driver");


        pwmServerDriver->begin();
        pwmServerDriver->setOscillatorFrequency(27000000);
        pwmServerDriver->setPWMFreq(SERVO_FREQ); // Analog servos run at ~50 Hz updates

        // Allow things to settle
        vTaskDelay(pdMS_TO_TICKS(10));

        l.debug("set up the server driver");
    }

    u8_t CreatureServos::getNumberOfServos()
    {
        return numberOfServos;
    }

    void CreatureServos::moveServo(u8_t servoNumber, u8_t position)
    {
        l.debug("moving servo %d to position %d", servoNumber, position);

        // Make sure we were given a number that we're actually using
        if(servoNumber > (numberOfServos - 1))
        {
            l.fatal("Unable to move servo %d since we only are using %d", servoNumber, numberOfServos);
            return;
        }

        uint16_t pwmValue = map(position, 0, UINT8_MAX, SERVOMIN, SERVOMAX);
        l.verbose("%d -> %d", position, pwmValue);
        pwmServerDriver->setPWM(servoNumber, 0, pwmValue);
    }
}