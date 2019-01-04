
#include "jet/live/LinkCommand.hpp"

namespace jet
{
    std::string createLinkCommand(const std::string& libName,
        const std::string& compilerPath,
        const std::vector<std::string>& objectFilePaths)
    {
        std::string linkCommand =
            compilerPath + " -fPIC -g -rdynamic -shared -Wl,-soname," + libName + " -o  " + libName + " ";
        for (const auto& oFile : objectFilePaths) {
            linkCommand.append("\"" + oFile + "\" ");
        }
        return linkCommand;
    }
}
