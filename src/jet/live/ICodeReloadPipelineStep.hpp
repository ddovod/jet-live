
#pragma once

namespace jet
{
    struct LiveContext;
    struct Program;
    class ICodeReloadPipelineStep
    {
    public:
        virtual ~ICodeReloadPipelineStep() {}
        virtual void reload(LiveContext* context, Program* newProgram) = 0;
    };
}
