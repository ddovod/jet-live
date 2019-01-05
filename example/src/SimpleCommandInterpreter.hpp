
#pragma once

#include <string>

class SimpleCommandInterpreter
{
public:
    explicit SimpleCommandInterpreter(int currentCommandsCounter = 0);
    int getCurrentCommandsCounter() const;
    std::string runCommand(const std::string& command);

private:
    double someStuff;
    int m_currentCommandsCounter = 0;
};
