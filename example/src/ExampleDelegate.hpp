
#pragma once

#include <functional>
#include <jet/live/LiveDelegate.hpp>

class ExampleDelegate : public jet::LiveDelegate
{
public:
    ExampleDelegate(std::function<void()>&& codePreLoadCallback, std::function<void()>&& codePostLoadCallback);
    void onLog(jet::LogSeverity severity, const std::string& message) override;
    void onCodePreLoad() override;
    void onCodePostLoad() override;
    size_t getWorkerThreadsCount() override;

private:
    std::function<void()> m_codePreLoadCallback;
    std::function<void()> m_codePostLoadCallback;
};
