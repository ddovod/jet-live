
#pragma once

#include "jet/live/ICodeReloadPipelineStep.hpp"

namespace jet
{
    class FunctionsHookingStep : public ICodeReloadPipelineStep
    {
    public:
        void reload(LiveContext* context, Program* newProgram) override;
    };
}
