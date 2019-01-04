
#pragma once

#include <string>
#include <vector>

namespace jet
{
    std::string createLinkCommand(const std::string& libName,
        const std::string& compilerPath,
        const std::vector<std::string>& objectFilePaths);
}
