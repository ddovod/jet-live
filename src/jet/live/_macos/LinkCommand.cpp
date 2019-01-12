
#include "jet/live/LinkCommand.hpp"

namespace jet
{
    std::string createLinkCommand(const std::string& libName,
        const std::string& compilerPath,
        const std::vector<std::string>& objectFilePaths)
    {
        // clang-format off
        std::string linkCommand = (compilerPath + " -undefined dynamic_lookup"
                                   + " -fPIC -shared -g"
                                   + " -Wl,-export_dynamic"
                                   + " -Wl,-install_name," + libName
                                   + " -o " + libName + " ");
        // clang-format on
        for (const auto& oFile : objectFilePaths) {
            linkCommand.append("\"" + oFile + "\" ");
        }
        return linkCommand;
    }
}
