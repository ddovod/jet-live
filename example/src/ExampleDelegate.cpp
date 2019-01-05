
#include "ExampleDelegate.hpp"
#include <iostream>

ExampleDelegate::ExampleDelegate(std::function<void()>&& codePreLoadCallback,
    std::function<void()>&& codePostLoadCallback)
    : m_codePreLoadCallback(std::move(codePreLoadCallback))
    , m_codePostLoadCallback(std::move(codePostLoadCallback))
{
}

void ExampleDelegate::onLog(jet::LogSeverity severity, const std::string& message)
{
    std::string severityString;
    switch (severity) {
        case jet::LogSeverity::kDebug: severityString.append("[D]"); break;
        case jet::LogSeverity::kInfo: severityString.append("[I]"); break;
        case jet::LogSeverity::kWarning: severityString.append("[W]"); break;
        case jet::LogSeverity::kError: severityString.append("[E]"); break;
    }
    std::cout << severityString << ": " << message << std::endl;
}

void ExampleDelegate::onCodePreLoad()
{
    m_codePreLoadCallback();
}

void ExampleDelegate::onCodePostLoad()
{
    m_codePostLoadCallback();
}

size_t ExampleDelegate::getWorkerThreadsCount()
{
    return 2;
}
