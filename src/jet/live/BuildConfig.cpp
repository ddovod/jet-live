
#include "BuildConfig.hpp"

namespace jet
{
    const std::string& getCmakeGenerator()
    {
#ifdef JET_LIVE_CMAKE_GENERATOR
        static const std::string cmakeGenerator = JET_LIVE_CMAKE_GENERATOR;
#else
#error JET_LIVE_CMAKE_GENERATOR is not defined
        static const std::string cmakeGenerator;
#endif
        return cmakeGenerator;
    }

    const std::string& getCmakeBuildDirectory()
    {
#ifdef JET_LIVE_CMAKE_BUILD_DIR
        static const std::string cmakeBuildDir = JET_LIVE_CMAKE_BUILD_DIR;
#else
#error JET_LIVE_CMAKE_BUILD_DIR is not defined
        static const std::string cmakeBuildDir;
#endif
        return cmakeBuildDir;
    }
}
