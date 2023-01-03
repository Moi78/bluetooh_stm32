#ifndef COMMAND_PARSER_H__
#define COMMAND_PARSER_H__

#include <string>
#include <vector>
#include <functional>
#include <unordered_map>

typedef std::function<void(std::vector<std::string>)> CommandCallback;

class CommandParser {
public:
    CommandParser();
    ~CommandParser();

    bool DispatchCommand(std::string cmd);

    void AddCommandCallback(CommandCallback callback, std::string command);

private:
    std::vector<std::string> ParseCommand(std::string cmd);

    std::unordered_map<std::string, CommandCallback> m_commandTable;
};

#endif // COMMAND_PARSER_H__