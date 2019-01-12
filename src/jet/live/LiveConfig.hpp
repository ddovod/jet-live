
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
         * A list of directories to monitor for source file changes.
         * If empty, the most common directory for all found compilation units will be used.
         */
        std::vector<std::string> directoriesToMonitor;
    };
}
