
#pragma once

#ifdef __linux__
#include "jet/live/_linux/ElfProgramInfoLoader.hpp"
namespace jet
{
    using DefaultProgramInfoLoader = ElfProgramInfoLoader;
}

#elif __APPLE__
#include "jet/live/_macos/MachoProgramInfoLoader.hpp"
namespace jet
{
    using DefaultProgramInfoLoader = MachoProgramInfoLoader;
}

#else
#error "Platform is not supported"
#endif
