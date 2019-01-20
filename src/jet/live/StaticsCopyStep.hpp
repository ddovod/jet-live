
#pragma once

#include "jet/live/ICodeReloadPipelineStep.hpp"

namespace jet
{
    class StaticsCopyStep : public ICodeReloadPipelineStep
    {
    public:
        void reload(LiveContext* context, Program* newProgram) override;
    };
}
