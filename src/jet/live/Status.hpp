
#pragma once

#include <set>
#include <string>

namespace jet
{
    struct Status
    {
        std::set<std::string> compilingFiles;
        std::set<std::string> successfulFiles;
        std::set<std::string> failedFiles;
    };
}
