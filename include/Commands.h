#ifndef COMMANDS_H__
#define COMMANDS_H__

#include <string>
#include <vector>
#include <unordered_map>

#include <mbed.h>

#include "SerialPeriph.h"
#include "UARTDataOut.h"
#include "Bot.h"
#include "AutoTest.h"
#include "defs.h" 

void setPwm(std::vector<std::string> args, Bot* bot);
void autotest(std::vector<std::string> args, Bot* bot);
void getSoftwareInfos(std::vector<std::string> args, Bot* bot);
void echo(std::vector<std::string> args, Bot* bot);
void monitor_state(std::vector<std::string> args, Bot* bot);

#endif //COMMANDS_H__