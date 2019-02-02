
#pragma once

#include <functional>

void runAfterDelayAndWaitForReload(std::function<void()>&& func, int milliseconds = 1000);
void waitForReload(int milliseconds = 1000);
void waitForReloadWithSignal(int milliseconds = 1000);
