
#include "TestListener.hpp"
#include <iostream>

void TestListener::setCallbacks(std::function<void ()> &&codePreLoadCallback, std::function<void ()> &&codePostLoadCallback)
{
    m_codePreLoadCallback = std::move(codePreLoadCallback);
    m_codePostLoadCallback = std::move(codePostLoadCallback);
}

void TestListener::onLog(jet::LogSeverity severity, const std::string &message)
{
    switch (severity) {
        case jet::LogSeverity::kDebug: std::cout << "[D] "; break;
        case jet::LogSeverity::kInfo: std::cout << "[I] "; break;
        case jet::LogSeverity::kWarning: std::cout << "[W] "; break;
        case jet::LogSeverity::kError: std::cout << "[E] "; break;
    }
    std::cout << message << std::endl;
}

void TestListener::onCodePreLoad()
{
    if (m_codePreLoadCallback) {
        m_codePreLoadCallback();
    }
}

void TestListener::onCodePostLoad()
{
    if (m_codePostLoadCallback) {
        m_codePostLoadCallback();
    }
}

