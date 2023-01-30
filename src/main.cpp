#include <mbed.h>
#include <stdint.h>
#include <atomic>

#include "Commands.h"
#include "SerialPeriph.h"
#include "CommandParser.h"
#include "Bot.h"
#include "UARTDataOut.h"

SerialPeriph pc(USBTX, USBRX, 9600);
SerialPeriph rn42(D1, D0, 115000);

int main() {
    UART::serialOut.SetOutputPort(&rn42);

    // Setting up command parser for bluetooth com
    CommandParser<Bot>* parser = new CommandParser<Bot>();
    parser->AddCommandCallback(&setPwm, "setPwm");
    parser->AddCommandCallback(&autotest, "autotest");
    parser->AddCommandCallback(&getSoftwareInfos, "getSoftwareInfos");
    parser->AddCommandCallback(&echo, "echo");
    parser->AddCommandCallback(&monitor_state, "monitor_state");
    parser->AddCommandCallback(&setError, "setError");
    parser->AddCommandCallback(&showSensorState, "showSensorState");
    parser->AddCommandCallback(&go_forward, "forward");

    // DEFAULT ERROR -3

    Bot bot = Bot();
    bot.InitBot();
    bot.SetError(-3);

    parser->SetUserPTR(&bot);

    while(1) {
        // Updating serial I/O
        pc.Poll();
        rn42.Poll();

        // Command dispatch
        if(rn42.GotReturn()) {
            std::string cmd = rn42.ReadFromPort(true);

            if(!parser->DispatchCommand(cmd)) {
                UART::serialOut << "ERROR: Unknown command." << UART::endl;
            }
        }

        // Bot update
        bot.UpdateCaptRead();
        bot.MainRoutine();
    }
}