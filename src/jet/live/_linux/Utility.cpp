
#include "jet/live/Utility.hpp"
#include <elf.h>
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

            auto addrDelim = line.find('-');
            auto addrEnd = line.find(' ');
            if (addrDelim == std::string::npos || addrEnd == std::string::npos) {
                continue;
            }
            size_t nameBegin = addrEnd;
            for (int i = 0; i < 4; i++) {
                nameBegin = line.find(' ', nameBegin + 1);
            }
            while (line[nameBegin] == ' ') {
                if (nameBegin == line.size() - 1) {
                    break;
                }
                nameBegin++;
            }
            if (nameBegin != line.size() - 1) {
                region.name = line.substr(nameBegin);
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

            if (res.empty()) {
                res.push_back(region);
            } else if (res.back().name == region.name && !region.name.empty()) {
                res.back().regionEnd = region.regionEnd;
            } else if (res.back().regionEnd != region.regionBegin) {
                MemoryRegion freeRegion;
                freeRegion.regionBegin = res.back().regionEnd;
                freeRegion.regionEnd = region.regionBegin;
                freeRegion.isInUse = false;
                res.push_back(freeRegion);
                res.push_back(region);
            } else {
                res.push_back(region);
            }
        }

        return res;
    }

    std::string relToString(uint32_t relocType)
    {
        switch (relocType) {
            case R_X86_64_NONE: return "R_X86_64_NONE";
            case R_X86_64_64: return "R_X86_64_64\t";
            case R_X86_64_PC32: return "R_X86_64_PC32";
            case R_X86_64_GOT32: return "R_X86_64_GOT32";
            case R_X86_64_PLT32: return "R_X86_64_PLT32";
            case R_X86_64_COPY: return "R_X86_64_COPY";
            case R_X86_64_GLOB_DAT: return "R_X86_64_GLOB_DAT";
            case R_X86_64_JUMP_SLOT: return "R_X86_64_JUMP_SLOT";
            case R_X86_64_RELATIVE: return "R_X86_64_RELATIVE";
            case R_X86_64_GOTPCREL: return "R_X86_64_GOTPCREL";
            case R_X86_64_32: return "R_X86_64_32\t";
            case R_X86_64_32S: return "R_X86_64_32S\t";
            case R_X86_64_16: return "R_X86_64_16\t";
            case R_X86_64_PC16: return "R_X86_64_PC16";
            case R_X86_64_8: return "R_X86_64_8\t";
            case R_X86_64_PC8: return "R_X86_64_PC8";
            case R_X86_64_DTPMOD64: return "R_X86_64_DTPMOD64";
            case R_X86_64_DTPOFF64: return "R_X86_64_DTPOFF64";
            case R_X86_64_TPOFF64: return "R_X86_64_TPOFF64";
            case R_X86_64_TLSGD: return "R_X86_64_TLSGD";
            case R_X86_64_TLSLD: return "R_X86_64_TLSLD";
            case R_X86_64_DTPOFF32: return "R_X86_64_DTPOFF32";
            case R_X86_64_GOTTPOFF: return "R_X86_64_GOTTPOFF";
            case R_X86_64_TPOFF32: return "R_X86_64_TPOFF32";
            case R_X86_64_PC64: return "R_X86_64_PC64";
            case R_X86_64_GOTOFF64: return "R_X86_64_GOTOFF64";
            case R_X86_64_GOTPC32: return "R_X86_64_GOTPC32";
            case R_X86_64_GOT64: return "R_X86_64_GOT64";
            case R_X86_64_GOTPCREL64: return "R_X86_64_GOTPCREL64";
            case R_X86_64_GOTPC64: return "R_X86_64_GOTPC64";
            case R_X86_64_GOTPLT64: return "R_X86_64_GOTPLT64";
            case R_X86_64_PLTOFF64: return "R_X86_64_PLTOFF64";
            case R_X86_64_SIZE32: return "R_X86_64_SIZE32";
            case R_X86_64_SIZE64: return "R_X86_64_SIZE64";
            case R_X86_64_GOTPC32_TLSDESC: return "R_X86_64_GOTPC32_TLSDESC";
            case R_X86_64_TLSDESC_CALL: return "R_X86_64_TLSDESC_CALL";
            case R_X86_64_TLSDESC: return "R_X86_64_TLSDESC";
            case R_X86_64_IRELATIVE: return "R_X86_64_IRELATIVE";
            case R_X86_64_RELATIVE64: return "R_X86_64_RELATIVE64";
            // case R_X86_64_GOTPCRELX: return "R_X86_64_GOTPCRELX";
            // case R_X86_64_REX_GOTPCRELX: return "R_X86_64_REX_GOTPCRELX";
            case R_X86_64_NUM: return "R_X86_64_NUM\t";
            default: return "UNKNOWN";
        }
    }
}
