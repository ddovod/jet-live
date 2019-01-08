
#include "TestDelegate.hpp"

void TestDelegate::setCallbacks(std::function<void ()> &&codePreLoadCallback, std::function<void ()> &&codePostLoadCallback)
{
    m_codePreLoadCallback = std::move(codePreLoadCallback);
    m_codePostLoadCallback = std::move(codePostLoadCallback);
}

void TestDelegate::onCodePreLoad()
{
    if (m_codePreLoadCallback) {
        m_codePreLoadCallback();
    }
}

void TestDelegate::onCodePostLoad()
{
    if (m_codePostLoadCallback) {
        m_codePostLoadCallback();
    }
}

