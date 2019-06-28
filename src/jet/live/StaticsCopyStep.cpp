
#include "StaticsCopyStep.hpp"
#include <cstring>
#include "jet/live/LiveContext.hpp"
#include "jet/live/Utility.hpp"

namespace jet
{
    void StaticsCopyStep::reload(LiveContext* context, Program* newProgram)
    {
        context->events->addLog(LogSeverity::kDebug, "Copying statics from old code to new one...");

        auto totalVars = getTotalVariables(newProgram->symbols);
        size_t copiedVars = 0;
        for (const auto& syms : newProgram->symbols.variables) {
            for (const auto& sym : syms.second) {
                void* oldVarPtr = nullptr;
                size_t oldVarSize = 0;
                const auto& progs = context->programs;
                for (const auto& prog : progs) {
                    if (auto foundSym = findVariable(prog.symbols, sym.name, sym.hash)) {
                        oldVarSize = foundSym->size;
                        oldVarPtr = reinterpret_cast<void*>(foundSym->runtimeAddress);
                        break;
                    }
                }
                if (!oldVarPtr || oldVarSize == 0) {
                    continue;
                }

                auto newVarPtr = reinterpret_cast<void*>(sym.runtimeAddress);

                // Trying to do our best
                memcpy(newVarPtr, oldVarPtr, std::min(sym.size, oldVarSize));
                copiedVars++;
            }
        }

        context->events->addLog(
            LogSeverity::kDebug, "Done, copied: " + std::to_string(copiedVars) + "/" + std::to_string(totalVars));
    }
}
