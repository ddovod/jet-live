
#pragma once

#include <functional>

void runAfterDelayAndWaitForReload(std::function<void()>&& func);
void waitForReload();
void waitForReloadWithSignal();
