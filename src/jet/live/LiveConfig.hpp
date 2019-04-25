
#pragma once

#include <string>
#include <vector>

namespace jet
{
    /**
     * Configuration parameters.
     */
    struct LiveConfig
    {
        /**
         * The maximum amount of possible worker threads used by the library.
         * Usually all these threads are busy compiling new code.
         */
        size_t workerThreadsCount = 4;

        /**
         * If `true`, also reload code when app receives `SIGUSR1`.
         */
        bool reloadOnSignal = true;

        /*
         * If `true`, disables odr violation detector in address sanitizer.
         * This is needed because each code reload violates odr rule.
         * But since we are here, we need exactly this, right?
         */
        bool disableAsanOdrViolationDetector = true;
    };
}
