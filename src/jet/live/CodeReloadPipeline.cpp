
#include "CodeReloadPipeline.hpp"
#include <cassert>

namespace jet
{
    void CodeReloadPipeline::addStep(std::unique_ptr<ICodeReloadPipelineStep>&& step)
    {
        m_steps.emplace_back(std::move(step));
    }

    void CodeReloadPipeline::reload(LiveContext* context, Program* newProgram)
    {
        assert(newProgram);

        for (const auto& step : m_steps) {
            step->reload(context, newProgram);
        }
    }
}
