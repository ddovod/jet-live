
#include "StaticsCopyStep.hpp"
#include <cstring>
#include "jet/live/LiveContext.hpp"
#include "jet/live/Utility.hpp"

namespace jet
{
    void StaticsCopyStep::reload(LiveContext* context, Program* newProgram)
    {
        context->listener->onLog(LogSeverity::kInfo, "Copying statics from old code to new one...");

        auto totalVars = getTotalVariables(newProgram->symbols);
        size_t copiedVars = 0;
        for (const auto& syms : newProgram->symbols.variables) {
            for (const auto& sym : syms.second) {
                void* oldVarPtr = nullptr;
                size_t oldVarSize = 0;
                const auto& progs = context->programs;
                for (auto it = progs.rbegin(); it != progs.rend(); it++) {
                    if (auto foundSym = findVariable(it->symbols, sym.name, sym.hash)) {
                        oldVarSize = foundSym->size;
                        oldVarPtr = reinterpret_cast<void*>(foundSym->runtimeAddress);
                        break;
                    }
                }
                if (!oldVarPtr) {
                    continue;
                }

                auto newVarPtr = reinterpret_cast<void*>(sym.runtimeAddress);

                // Trying to do our best
                memcpy(newVarPtr, oldVarPtr, std::min(sym.size, oldVarSize));
                copiedVars++;
            }
        }

        context->listener->onLog(
            LogSeverity::kInfo, "Done, copied: " + std::to_string(copiedVars) + "/" + std::to_string(totalVars));
    }
}
