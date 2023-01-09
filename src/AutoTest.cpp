#include "AutoTest.h"

void TestPWM(PwmOut& pwm, std::chrono::seconds sleepTime) {
    UART::serialOut << "------ MOTOR TEST ------" << UART::endl;

    for(float i = 0; i < 1.25; i += 0.25) {
        UART::serialOut << "PWM " << std::to_string((int)(i * 100)) << "%" << UART::endl;
        pwm.write(i);

        ThisThread::sleep_for(sleepTime);
    }

    pwm.write(0);
    UART::serialOut << "DONE" << UART::endl;
}