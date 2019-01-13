
#include "jet/live/Utility.hpp"
#include <fstream>
#include <iomanip>
#include <sstream>
#include <unistd.h>

namespace jet
{
    std::vector<MemoryRegion> getMemoryRegions()
    {
        std::vector<MemoryRegion> res;

        auto myPid = getpid();
        std::ifstream f{"/proc/" + std::to_string(myPid) + "/maps"};
        if (!f.is_open()) {
            return res;
        }

        std::stringstream ss;
        std::string line;
        while (std::getline(f, line)) {
            MemoryRegion region;

            auto addrDelim = line.find("-");
            auto addrEnd = line.find(" ");
            if (addrDelim == std::string::npos || addrEnd == std::string::npos) {
                continue;
            }

            auto addrBeginStr = "0x" + std::string(line, 0, addrDelim);
            auto addrEndStr = "0x" + std::string(line, addrDelim + 1, addrEnd - addrDelim - 1);
            ss << std::hex << addrBeginStr;
            ss >> region.regionBegin;
            ss.clear();
            ss << std::hex << addrEndStr;
            ss >> region.regionEnd;
            ss.clear();
            region.isInUse = true;
            if (!res.empty() && res.back().regionEnd != region.regionBegin) {
                MemoryRegion freeRegion;
                freeRegion.regionBegin = res.back().regionEnd;
                freeRegion.regionEnd = region.regionBegin;
                freeRegion.isInUse = false;
                res.push_back(freeRegion);
            }
            res.push_back(region);
        }

        return res;
    }
}
