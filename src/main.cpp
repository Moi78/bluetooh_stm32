#include <mbed.h>
#include <stdint.h>

#include "Commands.h"
#include "SerialPeriph.h"
#include "CommandParser.h"
#include "Bot.h"
#include "UARTDataOut.h"

SerialPeriph pc(USBTX, USBRX, 9600);
SerialPeriph rn42(D1, D0, 115000);

int main() {
    rn42.set_flow_control(mbed::SerialBase::Flow::RTS, PA_12);

    UART::serialOut.SetOutputPort(&rn42);

    // Setting up command parser for bluetooth com
    CommandParser<Bot> parser;
    parser.AddCommandCallback(&setPwm, "setPwm");
    parser.AddCommandCallback(&autotest, "autotest");
    parser.AddCommandCallback(&getSoftwareInfos, "getSoftwareInfos");
    parser.AddCommandCallback(&echo, "echo");
    parser.AddCommandCallback(&monitor_state, "monitor_state");

    Bot bot = Bot();
    bot.InitBot();

    parser.SetUserPTR(&bot);

    while(1) {
        // Updating serial I/O
        pc.Poll();
        rn42.Poll();

        // Bot update
        bot.MainRoutine();

        // Command dispatch
        if(rn42.GotReturn()) {
            if(!parser.DispatchCommand(rn42.ReadFromPort())) {
                UART::serialOut << "ERROR: Unknown command." << UART::endl;
            }
        }
    }
}