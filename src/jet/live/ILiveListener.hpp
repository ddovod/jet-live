
#pragma once

#include <string>

namespace jet
{
    enum class LogSeverity
    {
        kDebug,
        kInfo,
        kWarning,
        kError
    };

    /**
     * Base class for custom delegate.
     */
    class ILiveListener
    {
    public:
        virtual ~ILiveListener() {}

        /**
         * Called on each log message from the library.
         */
        virtual void onLog(LogSeverity, const std::string&) {}

        /**
         * Called right before shared library with new code is loaded into the process address space.
         */
        virtual void onCodePreLoad() {}

        /**
         * Called right after all functions are hooked and state is transferred.
         */
        virtual void onCodePostLoad() {}
    };
}
