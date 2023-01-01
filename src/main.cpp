#include <mbed.h>
#include <stdint.h>

#include "SerialPeriph.h"
#include "CommandParser.h"
#include "AutoTest.h"

SerialPeriph pc(USBTX, USBRX, 9600);
SerialPeriph rn42(D1, D0, 115000);

PwmOut pLed1(PA_4);
PwmOut pLed2(PA_3);

void setPwm(std::vector<std::string> args) {
    if(args.size() < 3) {
        return;
    }

    for(int i = 0; i < args.size(); i++) {
        if(args[i] == "-l") {
            float data = 0.0f;
            data = std::stof(args[i + 1]);

            pLed1.write(data / 100.0f);
        } else if(args[i] == "-r") {
            float data = 0.0f;
            data = std::stof(args[i + 1]);

            pLed2.write(data / 100.0f);
        }
    }
}

void autotest(std::vector<std::string> args) {
    if(args.size() < 2) {
        return;
    }

    for(int i = 0; i < args.size(); i++) {
        if(args[i] == "pwmL") {
            TestPWM(pLed1, 2s, rn42);
        } else if(args[i] == "pwmR") {
            TestPWM(pLed2, 2s, rn42);
        }
    }
}

int main() {
    CommandParser parser;
    parser.AddCommandCallback(&setPwm, "setPwm");
    parser.AddCommandCallback(&autotest, "autotest");

    rn42.set_flow_control(mbed::SerialBase::Flow::RTS, PA_12);

    pLed1.period(1.0f / 1000.0f);
    pLed1.write(0.2f);

    pLed2.period(1.0f / 1000.0f);
    pLed2.write(0.2f);

    while(1) {
        pc.Poll();
        rn42.Poll();

        if(rn42.GotReturn()) {
            parser.DispatchCommand(rn42.ReadFromPort());
        }
    }
}