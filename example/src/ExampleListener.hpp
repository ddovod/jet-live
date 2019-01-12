
#pragma once

#include <functional>
#include <jet/live/ILiveListener.hpp>

class ExampleListener : public jet::ILiveListener
{
public:
    ExampleListener(std::function<void()>&& codePreLoadCallback, std::function<void()>&& codePostLoadCallback);
    void onLog(jet::LogSeverity severity, const std::string& message) override;
    void onCodePreLoad() override;
    void onCodePostLoad() override;

private:
    std::function<void()> m_codePreLoadCallback;
    std::function<void()> m_codePostLoadCallback;
};
