
#include "SimpleCommandInterpreter.hpp"

SimpleCommandInterpreter::SimpleCommandInterpreter(int currentCommandsCounter)
    : m_currentCommandsCounter(currentCommandsCounter)
{
}

int SimpleCommandInterpreter::getCurrentCommandsCounter() const
{
    return m_currentCommandsCounter;
}

std::string SimpleCommandInterpreter::runCommand(const std::string& command)
{
    auto totalCommands = std::to_string(++m_currentCommandsCounter);
    std::string result;

    // Implement your commands here
    if (command == "Hello") {
        result = "Hi there!";
    } else {
        result = "Sorry, I don't know what '" + command
                 + "' means. Fix it in runtime in 'SimpleCommandInterpreter::runCommand'";
    }

    return "ttl: " + totalCommands + " > " + result;
}
