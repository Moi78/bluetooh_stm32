#ifndef COMMAND_PARSER_H__
#define COMMAND_PARSER_H__

#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <type_traits>

template<class userptr_t>
class CommandParser {
public:
    typedef std::function<void(std::vector<std::string>, userptr_t*)> CommandCallback;

    CommandParser() = default;
    ~CommandParser() = default;

    bool DispatchCommand(std::string cmd) {
        auto parsed = ParseCommand(cmd);

        // Searching if 1) Command exists and then 2) Call command callback function
        if(parsed.size() > 0) {
            if(m_commandTable.find(parsed[0]) != m_commandTable.end()) {
                m_commandTable[parsed[0]](parsed, m_user_ptr);
            } else {
                return false;
            }
        }

        return true;
    }

    void SetUserPTR(userptr_t* ptr) {
        m_user_ptr = ptr;
    }

    void AddCommandCallback(CommandCallback callback, std::string command) {
        m_commandTable.insert(std::pair<std::string, CommandCallback>(command, callback));
    }

private:
    // This functions splits a string into a string array and uses spaces as splitter
    std::vector<std::string> ParseCommand(std::string cmd) {
        UART::serialOut << "PARSE" << UART::endl;

        std::vector<std::string> args;

        std::string currentWord;
        for(auto chr : cmd) {
            if(chr != ' ') {
                if(chr != '\n') {
                    currentWord += chr;
                }
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

    std::unordered_map<std::string, CommandCallback> m_commandTable;
    userptr_t* m_user_ptr;
};

#endif // COMMAND_PARSER_H__