
#include "Utility.hpp"
#include <whereami.h>

namespace jet
{
    std::string getExecutablePath()
    {
        std::string pathString;
        int pathLength = wai_getExecutablePath(nullptr, 0, nullptr);
        pathString.resize(pathLength + 1);
        wai_getExecutablePath(const_cast<char*>(pathString.data()), pathLength, nullptr);
        pathString[pathLength] = '\0';
        return pathString;
    }
}
