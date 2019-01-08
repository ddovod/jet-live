
#include "WaitForReload.hpp"
#include "Globals.hpp"

void waitForReload()
{
    bool cont = true;
    int updatesCount = 0;
    g_testDelegatePtr->setCallbacks(nullptr, [&cont] {
        cont = false;
    });
    while (cont) {
        if (updatesCount == 10) {
            g_live->tryReload();
        }
        g_live->update();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        updatesCount++;
    }
    g_testDelegatePtr->setCallbacks(nullptr, nullptr);
}
