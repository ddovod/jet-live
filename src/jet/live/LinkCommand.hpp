
#pragma once

#include <string>
#include <vector>

namespace jet
{
    /**
     * Creates shared library linkage command string.
     * \param libname Name of shared library.
     * \param compilerPath Path to the compiler.
     * \param objectFilePaths Paths to all object files which should be linked into the library.
     */
    std::string createLinkCommand(const std::string& libName,
        const std::string& compilerPath,
        const std::vector<std::string>& objectFilePaths);
}
