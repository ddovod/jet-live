
#include "TestListener.hpp"
#include <iostream>
#include <chrono>
#include <ctime>

void TestListener::setCallbacks(std::function<void ()> &&codePreLoadCallback, std::function<void ()> &&codePostLoadCallback)
{
    m_codePreLoadCallback = std::move(codePreLoadCallback);
    m_codePostLoadCallback = std::move(codePostLoadCallback);
}

void TestListener::onLog(jet::LogSeverity severity, const std::string &message)
{
    using namespace std::chrono;
    high_resolution_clock::time_point p = high_resolution_clock::now();
    milliseconds ms = duration_cast<milliseconds>(p.time_since_epoch());
    seconds s = duration_cast<seconds>(ms);
    std::time_t t = s.count();
    std::size_t milliSeconds = ms.count() % 1000;
    char buf[1024];
    strftime(buf, sizeof(buf), "%F %T", gmtime(&t));
    std::cout << '[' << buf << ':' << milliSeconds << ']';

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

