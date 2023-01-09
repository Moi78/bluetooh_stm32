#include <mbed.h>
#include <stdint.h>

#include "SerialPeriph.h"
#include "CommandParser.h"
#include "AutoTest.h"
#include "defs.h"
#include "Bot.h"

SerialPeriph pc(USBTX, USBRX, 9600);
SerialPeriph rn42(D1, D0, 115000);

PwmOut pLed1(PA_4);
PwmOut pLed2(PA_3);

bool monEnabled = false;

main_states_t current_main_routine = main_states_t::INIT;

DigitalIn BP(A0, PullUp);
DigitalIn JACK(A1, PullUp);

void btnState(std::vector<std::string> args) {
    rn42.PrintStr(BP == 1 ? "Up\n" : "Down\n");
}

void setPwm(std::vector<std::string> args) {
    if(args.size() < 3) {
        rn42.PrintStr("ERROR: Not enough arguments\n");
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
        rn42.PrintStr("ERROR: Not enough arguments\n");
        return;
    }

    for(int i = 1; i < args.size(); i++) {
        if(args[i] == "pwmL") {
            TestPWM(pLed1, 2s, rn42);
        } else if(args[i] == "pwmR") {
            TestPWM(pLed2, 2s, rn42);
        } else {
            rn42.PrintStr("ERROR: Unkown test \"" + args[i] + "\"\n");
        }
    }
}

void getSoftwareInfos(std::vector<std::string> args) {
    std::string version_string = std::to_string(FIRM_VER >> 16) + "." + std::to_string((FIRM_VER & 0x0F0) >> 8) + "." + std::to_string(FIRM_VER & 0x00F);

    rn42.PrintStr("*-*-*-*-*-* LAMA FACHE *-*-*-*-*-*\n");
    rn42.PrintStr("Software ver. " + version_string + " // Built " + BUILD_DATE + "\n");
}

void echo(std::vector<std::string> args) {
    if(args.size() < 2) {
        return;
    }

    for(int i = 1; i < args.size(); i++) {
        rn42.PrintStr(args[i] + " ");
    }
    rn42.PrintStr("\n");
}

void print_monitor_state() {
    std::unordered_map<main_states_t, std::string> stringify {
        {main_states_t::INIT, "INIT\n"},
        {main_states_t::RUN, "RUN\n"},
        {main_states_t::STOP, "STOP\n"}
    };

    rn42.PrintStr(stringify[current_main_routine]);
}

void monitor_state(std::vector<std::string> args) {
    monEnabled = !monEnabled;

    if(monEnabled) {
        rn42.PrintStr("Monitor enabled\n");
    } else {
        rn42.PrintStr("Monitor disabled\n");
    }
}

int main() {
    CommandParser parser;
    parser.AddCommandCallback(&setPwm, "setPwm");
    parser.AddCommandCallback(&autotest, "autotest");
    parser.AddCommandCallback(&getSoftwareInfos, "getSoftwareInfos");
    parser.AddCommandCallback(&echo, "echo");
    parser.AddCommandCallback(&btnState, "btnState");
    parser.AddCommandCallback(&monitor_state, "monitor_state");

    rn42.set_flow_control(mbed::SerialBase::Flow::RTS, PA_12);

    pLed1.period(1.0f / 1000.0f);
    pLed1.write(0.2f);

    pLed2.period(1.0f / 1000.0f);
    pLed2.write(0.2f);

    Timer anti_flood;
    anti_flood.start();
    while(1) {
        pc.Poll();
        rn42.Poll();

        MainRoutine(current_main_routine, BP, JACK);

        if(monEnabled && anti_flood > 2) {
            anti_flood.reset();
            print_monitor_state();
        }

        if(rn42.GotReturn()) {
            if(!parser.DispatchCommand(rn42.ReadFromPort())) {
                rn42.PrintStr("ERROR: Unknown command.\n");
            }
        }
    }
}