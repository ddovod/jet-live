
#pragma once

#include <string>

namespace jet
{
    struct CompilationUnit;

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
         * Called before symbol relocation, function hooking and static-state copying.
         * On success, the shared library has already been opened with dlopen.
         * Also called when linking or loading fails.
         */
        virtual void onCodePreLoad() {}

        /**
         * Called after function hooking and state transfer have been attempted.
         * Also called when linking or loading fails; this is not a success notification.
         */
        virtual void onCodePostLoad() {}

        /**
         * Allows the application to select which compilation unit to consider when parsing compile_commands.json.
         * Return true if the compilation unit should be considered (default), or false if it should be discarded.
         */
        virtual bool filterCompilationUnit(const CompilationUnit&) { return true; }
    };
}
