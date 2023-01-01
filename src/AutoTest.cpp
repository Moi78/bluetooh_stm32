#include "AutoTest.h"

void TestPWM(PwmOut& pwm, std::chrono::seconds sleepTime, SerialPeriph& outp) {
    for(float i = 0; i < 1.25; i += 0.25) {
        outp.PrintStr("PWM " + std::to_string((int)(i * 100)) + "%\n");
        pwm.write(i);

        ThisThread::sleep_for(sleepTime);
    }

    pwm.write(0);
}