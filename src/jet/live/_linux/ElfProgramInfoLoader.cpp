
#include "ElfProgramInfoLoader.hpp"
#include "jet/live/LiveContext.hpp"
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wconversion"
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
// clang-format off
#include <elfio/elfio.hpp>
#include <link.h>
// clang-format on
#pragma clang diagnostic pop

namespace jet
{
    std::vector<std::string> ElfProgramInfoLoader::getAllLoadedProgramsPaths(const LiveContext*) const
    {
        struct DlArgument
        {
            std::vector<std::string> filepaths;
        };
        DlArgument dlArgument;
        dl_iterate_phdr(
            [](struct dl_phdr_info* info, size_t, void* data) {
                auto arg = reinterpret_cast<DlArgument*>(data);
                const char* libPath = nullptr;
                if (info->dlpi_name && (info->dlpi_name[0] != 0)) {
                    libPath = info->dlpi_name;
                } else {
                    libPath = "";
                }
                arg->filepaths.emplace_back(libPath);
                return 0;
            },
            &dlArgument);

        return dlArgument.filepaths;
    }

    Symbols ElfProgramInfoLoader::getProgramSymbols(const LiveContext* context, const std::string& filepath) const
    {
        Symbols res;
        ElfContext elfContext;

        struct DlArgument
        {
            uintptr_t baseAddress = 0;
            std::string libPath;
        };
        DlArgument dlArgument;
        dlArgument.libPath = filepath;
        dl_iterate_phdr(
            [](struct dl_phdr_info* info, size_t, void* data) {
                auto arg = reinterpret_cast<DlArgument*>(data);
                const char* libPath = nullptr;
                if (info->dlpi_name && (info->dlpi_name[0] != 0)) {
                    libPath = info->dlpi_name;
                } else {
                    libPath = "";
                }
                if (libPath == arg->libPath) {
                    arg->baseAddress = info->dlpi_addr;
                    return 1;
                }
                return 0;
            },
            &dlArgument);

        const auto baseAddress = dlArgument.baseAddress;

        // This executable has empty string name on linux
        ELFIO::elfio elfFile;
        std::string realFilepath = filepath.empty() ? context->thisExecutablePath : filepath;
        if (!elfFile.load(realFilepath)) {
            context->delegate->onLog(LogSeverity::kError, "Cannot load " + realFilepath + " file");
            return res;
        }

        elfContext.sectionNames.resize(elfFile.sections.size());
        for (uint32_t i = 0; i < elfFile.sections.size(); i++) {
            const auto& section = elfFile.sections[i];
            elfContext.sectionNames[i] = section->get_name();

            if (section->get_type() == SHT_SYMTAB) {
                const ELFIO::symbol_section_accessor symbols{elfFile, section};
                for (uint32_t j = 0; j < symbols.get_symbols_num(); j++) {
                    std::string name;
                    ElfW(Addr) value = 0;
                    ElfW(Xword) size = 0;
                    unsigned char bind = 0;
                    unsigned char type = 0;
                    ElfW(Half) sectionIndex = 0;
                    unsigned char other = 0;
                    symbols.get_symbol(j, name, value, size, bind, type, sectionIndex, other);

                    ElfSymbol elfSymbol;
                    elfSymbol.name = name;
                    elfSymbol.sectionIndex = sectionIndex;
                    elfSymbol.size = size;
                    elfSymbol.virtualAddress = value;

                    switch (ELF64_ST_TYPE(type)) {  // NOLINT
                        case STT_NOTYPE: elfSymbol.type = ElfSymbolType::kNo; break;
                        case STT_OBJECT: elfSymbol.type = ElfSymbolType::kObject; break;
                        case STT_FUNC: elfSymbol.type = ElfSymbolType::kFunction; break;
                        case STT_SECTION: elfSymbol.type = ElfSymbolType::kSection; break;
                        case STT_FILE: elfSymbol.type = ElfSymbolType::kFile; break;
                        case STT_COMMON: elfSymbol.type = ElfSymbolType::kCommonObject; break;
                        case STT_TLS: elfSymbol.type = ElfSymbolType::kThreadLocalObject; break;
                        case STT_GNU_IFUNC: elfSymbol.type = ElfSymbolType::kIndirect; break;
                        default: break;
                    }

                    switch (ELF64_ST_VISIBILITY(other)) {  // NOLINT
                        case STV_DEFAULT: elfSymbol.visibility = ElfSymbolVisibility::kDefault; break;
                        case STV_INTERNAL: elfSymbol.visibility = ElfSymbolVisibility::kInternal; break;
                        case STV_HIDDEN: elfSymbol.visibility = ElfSymbolVisibility::kHidden; break;
                        case STV_PROTECTED: elfSymbol.visibility = ElfSymbolVisibility::kProtected; break;
                        default: break;
                    }

                    Symbol symbol;
                    symbol.name = elfSymbol.name;
                    symbol.size = elfSymbol.size;
                    symbol.runtimeAddress = baseAddress + elfSymbol.virtualAddress;

                    if (context->delegate->shouldReloadElfSymbol(elfContext, elfSymbol)) {
                        res.functions[symbol.name] = symbol;
                    }

                    if (context->delegate->shouldTransferElfSymbol(elfContext, elfSymbol)) {
                        res.variables[symbol.name] = symbol;
                    }
                }
            }
        }

        return res;
    }
}
