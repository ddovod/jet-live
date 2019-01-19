
#include "jet/live/Utility.hpp"
#include <iomanip>
#include <process.hpp>
#include <sstream>
#include <unistd.h>
#include <mach-o/x86_64/reloc.h>

namespace jet
{
    std::vector<MemoryRegion> getMemoryRegions()
    {
        std::vector<MemoryRegion> res;

        std::string cmd = "vmmap -interleaved -forkCorpse " + std::to_string(getpid());
        std::string procOut;
        std::string procError;
        TinyProcessLib::Process{cmd,
            "",
            [&procOut](const char* bytes, size_t n) { procOut += std::string(bytes, n); },
            [&procError](const char* bytes, size_t n) { procError += std::string(bytes, n); }}
            .get_exit_status();

        std::stringstream ss;
        std::string line;
        bool parse = false;
        std::stringstream procOutStream{procOut};
        while (std::getline(procOutStream, line)) {
            if (line.find("==== regions for process") == 0) {
                // Skipping one line
                std::getline(std::stringstream{procOut}, line);
                parse = true;
                continue;
            }
            if (!parse) {
                continue;
            }
            if (parse && line.empty()) {
                break;
            }

            MemoryRegion region;
            auto addrBeginStr = "0x" + line.substr(23, 16);
            auto addrEndStr = "0x" + line.substr(40, 16);
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

    std::string relToString(uint32_t relocType)
    {
        switch (relocType) {
            case X86_64_RELOC_SIGNED: return "X86_64_RELOC_SIGNED";
            case X86_64_RELOC_SIGNED_1: return "X86_64_RELOC_SIGNED_1";
            case X86_64_RELOC_SIGNED_2: return "X86_64_RELOC_SIGNED_2";
            case X86_64_RELOC_SIGNED_4: return "X86_64_RELOC_SIGNED_4";
            case X86_64_RELOC_UNSIGNED: return "X86_64_RELOC_UNSIGNED";
            case X86_64_RELOC_BRANCH: return "X86_64_RELOC_BRANCH";
            case X86_64_RELOC_GOT_LOAD: return "X86_64_RELOC_GOT_LOAD";
            case X86_64_RELOC_GOT: return "X86_64_RELOC_GOT";
            case X86_64_RELOC_SUBTRACTOR: return "X86_64_RELOC_SUBTRACTOR";
            case X86_64_RELOC_TLV: return "X86_64_RELOC_TLV";
            default: return "UNKNOWN";
        }
    }
}
