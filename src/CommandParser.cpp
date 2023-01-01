#include "CommandParser.h"

CommandParser::CommandParser() {

}

CommandParser::~CommandParser() {

}

void CommandParser::DispatchCommand(std::string cmd) {
    auto parsed = ParseCommand(cmd);

    if(parsed.size() > 0) {
        if(m_commandTable.find(parsed[0]) != m_commandTable.end()) {
            m_commandTable[parsed[0]](parsed);
        }
    }
}

void CommandParser::AddCommandCallback(CommandCallback callback, std::string command) {
    m_commandTable.insert(std::pair<std::string, CommandCallback>(command, callback));
}

std::vector<std::string> CommandParser::ParseCommand(std::string cmd) {
    std::vector<std::string> args;

    std::string currentWord;
    for(auto chr : cmd) {
        if(chr != ' ') {
            currentWord += chr;
        } else {
            if(currentWord.size() > 0) {
                args.push_back(currentWord);

                currentWord.clear();
            }
        }
    }

    // Don't forget the last word...
    if(currentWord.size() > 0) {
        args.push_back(currentWord);
    }

    return args;
} 