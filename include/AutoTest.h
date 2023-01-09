#ifndef AUTOTEST_H__
#define AUTOTEST_H__

#include <mbed.h>
#include <chrono>
#include <stdio.h>

#include "UARTDataOut.h"

void TestPWM(PwmOut& pwm, std::chrono::seconds sleepTime);

#endif //AUTOTEST_H__