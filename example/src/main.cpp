
#include <atomic>
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
#include <jet/live/Live.hpp>
#include <jet/live/Utility.hpp>
#include "ExampleDelegate.hpp"
#include "SimpleCommandInterpreter.hpp"

std::mutex g_inputMutex;
std::string g_inputCommand;
std::atomic_bool g_runloopContinue{true};
std::unique_ptr<SimpleCommandInterpreter> g_commandInterpreter = jet::make_unique<SimpleCommandInterpreter>();
int g_tempCommandsCounter = 0;

void preLoadCallback()
{
    // If you're changing the layout of SimpleCommandInterpreter class, uncomment next lines
    g_tempCommandsCounter = g_commandInterpreter->getCurrentCommandsCounter();
    g_commandInterpreter.reset();
}

void postLoadCallback()
{
    // If you're changing the layout of SimpleCommandInterpreter class, uncomment next line
    g_commandInterpreter = jet::make_unique<SimpleCommandInterpreter>(g_tempCommandsCounter);
}

std::string getNextCommand()
{
    std::string cmd;
    {
        std::lock_guard<std::mutex> lock(g_inputMutex);
        cmd = g_inputCommand;
        g_inputCommand.clear();
    }
    return cmd;
}

int main()
{
    auto live = jet::make_unique<jet::Live>(jet::make_unique<ExampleDelegate>(preLoadCallback, postLoadCallback));

    // Polling input in background
    std::thread inputThread{[] {
        std::string command;
        while (g_runloopContinue) {
            std::getline(std::cin, command);
            {
                std::lock_guard<std::mutex> lock(g_inputMutex);
                g_inputCommand = command;
            }
        }
    }};

    // Simple 60hz runloop
    std::cout << "Enter command" << std::endl << "Available commands: 'exit', 'reload', 'hello'" << std::endl;
    while (g_runloopContinue) {
        auto cmd = getNextCommand();
        if (!cmd.empty()) {
            if (cmd == "reload") {
                live->tryReload();
            } else if (cmd == "exit") {
                g_runloopContinue = false;
            } else {
                std::cout << g_commandInterpreter->runCommand(cmd) << std::endl;
            }
        }

        live->update();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    std::cout << "Press enter" << std::endl;
    inputThread.join();

    return 0;
}
