#include "Commands.h"

void setPwm(std::vector<std::string> args, Bot* bot) {
    if(args.size() < 3) {
        UART::serialOut << "ERROR: Not enough arguments" << UART::endl;
        return;
    }

    BotIO* bIO = bot->GetIO();

    for(unsigned int i = 0; i < args.size(); i++) {
        if(args[i] == "-l") {
            float data = 0.0f;
            data = std::stof(args[i + 1]);

            bIO->L.write(data / 100.0f);
        } else if(args[i] == "-r") {
            float data = 0.0f;
            data = std::stof(args[i + 1]);

            bIO->R.write(data / 100.0f);
        } else if(args[i] == "-lr") {
            float data = 0.0f;
            data = std::stof(args[i + 1]);

            bIO->R.write(data / 100.0f);
            bIO->L.write(data / 100.0f);
        }
    }

    UART::serialOut << UART::endl;
}

void setError(std::vector<std::string> args, Bot* bot) {
    if(args.size() < 2) {
        UART::serialOut << "ERROR: Not enough arguments" << UART::endl;
        return;
    }

    float error = std::stof(args[1]);
    bot->SetError(error);
}

void go_forward(std::vector<std::string> args, Bot* bot) {
    if(args.size() < 2) {
        UART::serialOut << "ERROR: Not enough arguments" << UART::endl;
        return;
    }

    float speed = std::stof(args[1]);
    bot->Forward(speed);
}

void autotest(std::vector<std::string> args, Bot* bot) {
    if(args.size() < 2) {
        UART::serialOut << "ERROR: Not enough arguments" << UART::endl;
        return;
    }

    BotIO* bIO = bot->GetIO();

    for(unsigned int i = 1; i < args.size(); i++) {
        if(args[i] == "pwmL") {
            TestPWM(bIO->L, 2s);
        } else if(args[i] == "pwmR") {
            TestPWM(bIO->R, 2s);
        } else {
            UART::serialOut << "ERROR: Unkown test \"" << args[i] << "\"" << UART::endl;
        }
    }
}

void getSoftwareInfos(std::vector<std::string> args, Bot* bot) {
    std::string version_string = std::to_string(FIRM_VER >> 16) + "." + std::to_string((FIRM_VER & 0x0F0) >> 8) + "." + std::to_string(FIRM_VER & 0x00F);

    UART::serialOut << "*-*-*-*-*-* LAMA FACHE *-*-*-*-*-*" << UART::endl;
    UART::serialOut << "Software ver. " << version_string << " // Built " << BUILD_DATE << UART::endl;
}

void echo(std::vector<std::string> args, Bot* bot) {
    if(args.size() < 2) {
        return;
    }

    for(unsigned int i = 1; i < args.size(); i++) {
        UART::serialOut << args[i] << " ";
    }
    UART::serialOut << UART::endl;
}

void monitor_state(std::vector<std::string> args, Bot* bot) {
    bot->ToggleMonitoring();
}

void showSensorState(std::vector<std::string> args, Bot* bot) {
    bot->ShowSensorState();
}