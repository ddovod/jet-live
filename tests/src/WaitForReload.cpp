
#include "WaitForReload.hpp"
#include "Globals.hpp"
#include <signal.h>
#include <unistd.h>

void runAfterDelayAndWaitForReload(std::function<void()>&& func, int milliseconds)
{
    bool cont = true;
    int updatesCount = 0;
    g_testListenerPtr->setCallbacks(nullptr, [&cont] {
        cont = false;
    });
    while (cont) {
        if (updatesCount == milliseconds / 100) {
            func();
        }
        g_live->update();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        updatesCount++;
        if (updatesCount > 100) {
            break;
        }
    }
    g_testListenerPtr->setCallbacks(nullptr, nullptr);
}

void waitForReload(int milliseconds)
{
    runAfterDelayAndWaitForReload([] {
        g_live->tryReload();
    }, milliseconds);
}

void waitForReloadWithSignal(int milliseconds)
{
    runAfterDelayAndWaitForReload([] {
        kill(getpid(), SIGUSR1);
    }, milliseconds);
}
