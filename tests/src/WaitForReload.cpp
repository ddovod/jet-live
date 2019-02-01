
#include "WaitForReload.hpp"
#include "Globals.hpp"
#include <signal.h>
#include <unistd.h>

void runAfterDelayAndWaitForReload(std::function<void()>&& func)
{
    bool cont = true;
    int updatesCount = 0;
    g_testListenerPtr->setCallbacks(nullptr, [&cont] {
        cont = false;
    });
    while (cont) {
        if (updatesCount == 10) {
            func();
        }
        g_live->update();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        updatesCount++;
    }
    g_testListenerPtr->setCallbacks(nullptr, nullptr);
}

void waitForReload()
{
    runAfterDelayAndWaitForReload([] {
        g_live->tryReload();
    });
}

void waitForReloadWithSignal()
{
    runAfterDelayAndWaitForReload([] {
        kill(getpid(), SIGUSR1);
    });
}
