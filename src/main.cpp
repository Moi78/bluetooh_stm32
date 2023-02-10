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
    Timer samplingChrono;
    samplingChrono.start();

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
    parser->AddCommandCallback(&bp, "bp");
    parser->AddCommandCallback(&jack, "jack");
    parser->AddCommandCallback(&setK, "setK");
    parser->AddCommandCallback(&setDiv, "setDiv");
    parser->AddCommandCallback(&setSpeed, "setSpeed");
    parser->AddCommandCallback(&reset, "reset");
    parser->AddCommandCallback(&setMegaDiv, "setMegaDiv");

    // DEFAULT ERROR -3

    Bot bot = Bot();
    bot.InitBot();
    bot.SetK(0.008f);
    bot.SetSpeed(0.32f);

    bot.SetError(1.02f);
    bot.SetDiv(1.17f);
    bot.SetMegaDiv(1.48f);

    parser->SetUserPTR(&bot);

    while(1) {
        #if !RACE_MODE
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
        #endif // RACE_MODE

        // Bot update
        bot.UpdateCaptRead();
        bot.ShortcutDetectionUpdate();
        bot.MainRoutine();
    }
}