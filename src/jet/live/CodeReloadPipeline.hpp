
#pragma once

#include <memory>
#include <vector>
#include "jet/live/ICodeReloadPipelineStep.hpp"

namespace jet
{
    struct LiveContext;
    struct Program;
    class CodeReloadPipeline
    {
    public:
        void addStep(std::unique_ptr<ICodeReloadPipelineStep>&& step);
        void reload(LiveContext* context, Program* newProgram);

    private:
        std::vector<std::unique_ptr<ICodeReloadPipelineStep>> m_steps;
    };
}
