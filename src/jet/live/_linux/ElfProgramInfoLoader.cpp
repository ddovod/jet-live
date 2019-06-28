
#include "ElfProgramInfoLoader.hpp"
#include <elfio/elf_types.hpp>
#include "jet/live/LiveContext.hpp"
// clang-format off
#include <cstdint>
#include <elfio/elfio.hpp>
#include <link.h>
// clang-format on
#include "jet/live/Utility.hpp"
#include <iostream>
#include <map>

namespace
{
    uintptr_t getBaseAddress(const std::string& filepath)
    {
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

        return dlArgument.baseAddress;
    }
}

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
        const auto baseAddress = ::getBaseAddress(filepath);

        // This executable has empty string name on linux
        ELFIO::elfio elfFile;
        std::string realFilepath = filepath.empty() ? context->thisExecutablePath : filepath;
        if (!elfFile.load(realFilepath)) {
            context->events->addLog(LogSeverity::kError, "Cannot load " + realFilepath + " file");
            return res;
        }

        std::hash<std::string> stringHasher;
        uint64_t currentHash = 0;
        elfContext.sectionNames.resize(elfFile.sections.size());
        for (ElfW(Half) i = 0; i < elfFile.sections.size(); i++) {
            const auto& section = elfFile.sections[i];
            elfContext.sectionNames[i] = section->get_name();

            // This one is needed to apply relocations of STT_SECTION symbols
            Symbol sectionSymbol;
            sectionSymbol.checkHash = false;
            sectionSymbol.name = section->get_name();
            sectionSymbol.runtimeAddress = baseAddress + section->get_offset();
            sectionSymbol.size = 0;
            res.variables[sectionSymbol.name].push_back(sectionSymbol);

            if (section->get_type() == SHT_SYMTAB) {
                const ELFIO::symbol_section_accessor symbols{elfFile, section};
                for (ElfW(Xword) j = 0; j < symbols.get_symbols_num(); j++) {
                    std::string name;
                    ElfW(Addr) value = 0;
                    ElfW(Xword) size = 0;
                    unsigned char bind = 0;
                    unsigned char type = 0;
                    ElfW(Half) sectionIndex = 0;
                    unsigned char other = 0;
                    symbols.get_symbol(j, name, value, size, bind, type, sectionIndex, other);

                    if ((bind == STB_GLOBAL || bind == STB_WEAK) && sectionIndex != STN_UNDEF) {
                        res.exportedSymbolNames.insert(name);
                    }

                    ElfSymbol elfSymbol;
                    elfSymbol.name = name;
                    elfSymbol.sectionIndex = sectionIndex;
                    elfSymbol.size = size;
                    elfSymbol.virtualAddress = value;

                    switch (type) {
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

                    switch (bind) {
                        case STB_LOCAL: elfSymbol.binding = ElfSymbolBinding::kLocal; break;
                        case STB_GLOBAL: elfSymbol.binding = ElfSymbolBinding::kGlobal; break;
                        case STB_WEAK: elfSymbol.binding = ElfSymbolBinding::kWeak; break;
                        case STB_GNU_UNIQUE: elfSymbol.binding = ElfSymbolBinding::kUnique; break;
                        default: break;
                    }

                    switch (ELF64_ST_VISIBILITY(other)) {  // NOLINT
                        case STV_DEFAULT: elfSymbol.visibility = ElfSymbolVisibility::kDefault; break;
                        case STV_INTERNAL: elfSymbol.visibility = ElfSymbolVisibility::kInternal; break;
                        case STV_HIDDEN: elfSymbol.visibility = ElfSymbolVisibility::kHidden; break;
                        case STV_PROTECTED: elfSymbol.visibility = ElfSymbolVisibility::kProtected; break;
                        default: break;
                    }

                    if (elfSymbol.type == ElfSymbolType::kFile) {
                        currentHash = stringHasher(elfSymbol.name);
                    }

                    Symbol symbol;
                    symbol.name = elfSymbol.name;
                    symbol.size = elfSymbol.size;
                    symbol.runtimeAddress = baseAddress + elfSymbol.virtualAddress;
                    if (elfSymbol.binding == ElfSymbolBinding::kLocal) {
                        symbol.checkHash = true;
                        elfSymbol.hash = symbol.hash = currentHash;
                    }

                    if (context->symbolsFilter->shouldReloadElfSymbol(elfContext, elfSymbol)) {
                        res.functions[symbol.name].push_back(symbol);
                    }

                    if (context->symbolsFilter->shouldTransferElfSymbol(elfContext, elfSymbol)) {
                        res.variables[symbol.name].push_back(symbol);
                    }
                }
            }
        }

        return res;
    }

    std::vector<Relocation> ElfProgramInfoLoader::getLinkTimeRelocations(const LiveContext* context,
        const std::vector<std::string>& objFilePaths)
    {
        std::vector<Relocation> res;

        for (const auto& el : objFilePaths) {
            ELFIO::elfio elfFile;
            if (!elfFile.load(el)) {
                context->events->addLog(LogSeverity::kError, "Cannot load " + el + " file");
                continue;
            }

            std::unordered_map<ElfW(Xword), ElfW(Half)> symbolsSectionIndexes;
            std::vector<std::map<uintptr_t, ElfSymbol>> symbolsInSections;
            int textSectionIndex = -1;
            int bssSectionIndex = -1;
            int dataSectionIndex = -1;
            std::hash<std::string> stringHasher;
            uint64_t currentHash = 0;
            symbolsInSections.resize(elfFile.sections.size());
            for (uint32_t i = 0; i < elfFile.sections.size(); i++) {
                const auto& section = elfFile.sections[i];
                const auto& sectionName = section->get_name();
                if (sectionName == ".text") {
                    textSectionIndex = static_cast<int>(i);
                } else if (sectionName == ".bss") {
                    bssSectionIndex = static_cast<int>(i);
                } else if (sectionName == ".data") {
                    dataSectionIndex = static_cast<int>(i);
                }

                if (section->get_type() == SHT_SYMTAB) {
                    const ELFIO::symbol_section_accessor symbols{elfFile, section};
                    for (ElfW(Xword) j = 0; j < symbols.get_symbols_num(); j++) {
                        std::string name;
                        ElfW(Addr) value = 0;
                        ElfW(Xword) size = 0;
                        unsigned char bind = 0;
                        unsigned char type = 0;
                        ElfW(Half) sectionIndex = 0;
                        unsigned char other = 0;
                        symbols.get_symbol(j, name, value, size, bind, type, sectionIndex, other);
                        symbolsSectionIndexes[j] = sectionIndex;

                        if (type == STT_SECTION) {
                            continue;
                        }

                        ElfSymbol elfSymbol;
                        elfSymbol.name = name;
                        elfSymbol.sectionIndex = sectionIndex;
                        elfSymbol.size = size;
                        elfSymbol.virtualAddress = value;

                        switch (type) {
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

                        switch (bind) {
                            case STB_LOCAL: elfSymbol.binding = ElfSymbolBinding::kLocal; break;
                            case STB_GLOBAL: elfSymbol.binding = ElfSymbolBinding::kGlobal; break;
                            case STB_WEAK: elfSymbol.binding = ElfSymbolBinding::kWeak; break;
                            case STB_GNU_UNIQUE: elfSymbol.binding = ElfSymbolBinding::kUnique; break;
                            default: break;
                        }

                        switch (ELF64_ST_VISIBILITY(other)) {  // NOLINT
                            case STV_DEFAULT: elfSymbol.visibility = ElfSymbolVisibility::kDefault; break;
                            case STV_INTERNAL: elfSymbol.visibility = ElfSymbolVisibility::kInternal; break;
                            case STV_HIDDEN: elfSymbol.visibility = ElfSymbolVisibility::kHidden; break;
                            case STV_PROTECTED: elfSymbol.visibility = ElfSymbolVisibility::kProtected; break;
                            default: break;
                        }

                        if (elfSymbol.type == ElfSymbolType::kFile) {
                            currentHash = stringHasher(elfSymbol.name);
                            continue;
                        }

                        if (elfSymbol.binding == ElfSymbolBinding::kLocal) {
                            elfSymbol.hash = currentHash;
                        }

                        if (elfSymbol.sectionIndex < symbolsInSections.size()) {
                            symbolsInSections[elfSymbol.sectionIndex][elfSymbol.virtualAddress] = elfSymbol;
                        }
                    }
                }
            }

            for (uint32_t i = 0; i < elfFile.sections.size(); i++) {
                const auto& section = elfFile.sections[i];
                if (section->get_type() == SHT_RELA) {
                    if (section->get_info() != static_cast<ElfW(Word)>(textSectionIndex)) {
                        continue;
                    }

                    const ELFIO::relocation_section_accessor relocs{elfFile, section};
                    for (ElfW(Xword) j = 0; j < relocs.get_entries_num(); j++) {
                        Relocation reloc;

                        ElfW(Addr) offset;
                        ElfW(Word) symbol;
                        ElfW(Word) type;
                        ElfW(Sxword) addend;
                        relocs.get_entry(j, offset, symbol, type, addend);

                        auto sectionIndex = symbolsSectionIndexes[symbol];
                        if (sectionIndex != bssSectionIndex && sectionIndex != dataSectionIndex) {
                            continue;
                        }

                        /*
                         * A: Addend of Elfxx_Rela entries.
                         * B: Image base where the shared object was loaded in process virtual address space.
                         * G: Offset to the GOT relative to the address of the correspondent relocation entry’s symbol.
                         * GOT: Address of the Global Offset Table
                         * L: Section offset or address of the procedure linkage table (PLT, .got.plt).
                         * P: The section offset or address of the storage unit being relocated.
                         *    Retrieved via r_offset.
                         * S: Relocation entry’s correspondent symbol value. Z: Size of relocations entry’s symbol.
                         */
                        intptr_t symRelAddr = 0;
                        switch (type) {
                            // Link-time relocation, we should fix it by ourself
                            case R_X86_64_PC32:  // 32,      S + A – P
                                reloc.size = sizeof(int32_t);
                                symRelAddr = static_cast<intptr_t>(addend + 4);
                                break;
                            case R_X86_64_PC64:  // 64,      S + A – P
                                reloc.size = sizeof(int64_t);
                                symRelAddr = static_cast<intptr_t>(addend + 4);
                                break;

                            // Load time relocations, will be fixed by dynamic linker
                            case R_X86_64_GOTPCREL:  // 32,      G + GOT + A – P
                                continue;

                            // Not yet supported relocations
                            case R_X86_64_PC8:      // 8,       S + A – P
                            case R_X86_64_PC16:     // 16,      S + A – P
                            case R_X86_64_GOTPC32:  // 32,      GOT + A – P
                            case R_X86_64_PLT32:    // 32,      L + A – P
                                context->events->addLog(
                                    LogSeverity::kError, "Relocation " + relToString(type) + " is not implemented");
                                continue;

                            // Non-PIC relocations
                            case R_X86_64_RELATIVE:   // 64,      B + A
                            case R_X86_64_GLOB_DAT:   // 64,      S
                            case R_X86_64_NONE:       // None,    None
                            case R_X86_64_8:          // 8,       S + A
                            case R_X86_64_16:         // 16,      S + A
                            case R_X86_64_32:         // 32,      S + A
                            case R_X86_64_32S:        // 32,      S + A
                            case R_X86_64_64:         // 64,      S + A
                            case R_X86_64_GOT32:      // 32,      G + A
                            case R_X86_64_COPY:       // None,    Value is copied directly from shared object
                            case R_X86_64_JUMP_SLOT:  // 64,      S
                            case R_X86_64_GOTOFF64:   // 64,      S + A – GOT
                            case R_X86_64_SIZE32:     // 32,      Z + A
                            case R_X86_64_SIZE64:     // 64,      Z + A
                            default:
                                context->events->addLog(LogSeverity::kError,
                                    "Relocation " + relToString(type) + " is not possible in PIC code");
                                continue;
                        }

                        const auto& symsInSection = symbolsInSections[sectionIndex];
                        auto symFound = symsInSection.find(static_cast<uintptr_t>(symRelAddr));
                        if (symRelAddr < 0) {
                            symFound = symsInSection.begin();
                        }
                        if (symFound == symsInSection.end()) {
                            symFound = symsInSection.upper_bound(static_cast<uintptr_t>(symRelAddr));
                            if (symFound != symsInSection.begin()) {
                                symFound--;
                            }
                        }
                        if (symFound == symsInSection.end()) {
                            context->events->addLog(LogSeverity::kError,
                                                    "WTF: file " + el + ", " + 
                                                    "section " + section->get_name() + ", " +
                                                    "target section index " + std::to_string(sectionIndex) + ", " +
                                                    "offset " + std::to_string(offset) + ", " +
                                                    "symbol " + std::to_string(symbol) + ", " +
                                                    "type " + std::to_string(type) + ", "
                                                    "addend " + std::to_string(addend));
                            continue;
                        }

                        auto found = symbolsInSections[section->get_info()].upper_bound(offset);
                        if (found != symbolsInSections[section->get_info()].begin()) {
                            found--;
                        }
                        if (found->second.virtualAddress > offset
                            || offset >= found->second.virtualAddress + found->second.size) {
                            context->events->addLog(LogSeverity::kError, "WTF1");
                            continue;
                        }

                        reloc.targetSymbolName = found->second.name;
                        reloc.targetSymbolHash = found->second.hash;
                        reloc.relocationOffsetRelativeTargetSymbolAddress = offset - found->second.virtualAddress;
                        reloc.relocationSymbolName = symFound->second.name;
                        reloc.relocationSymbolHash = symFound->second.hash;
                        res.push_back(reloc);
                    }
                }
            }
        }
        return res;
    }

    std::vector<std::string> ElfProgramInfoLoader::getUndefinedSymbolNames(const LiveContext* context,
        const std::string filepath)
    {
        std::vector<std::string> res;

        // This executable has empty string name on linux
        ELFIO::elfio elfFile;
        std::string realFilepath = filepath.empty() ? context->thisExecutablePath : filepath;
        if (!elfFile.load(realFilepath)) {
            context->events->addLog(LogSeverity::kError, "Cannot load " + realFilepath + " file");
            return res;
        }

        for (uint32_t i = 0; i < elfFile.sections.size(); i++) {
            const auto& section = elfFile.sections[i];
            if (section->get_type() == SHT_SYMTAB) {
                const ELFIO::symbol_section_accessor symbols{elfFile, section};
                res.reserve(symbols.get_symbols_num());
                for (ElfW(Xword) j = 0; j < symbols.get_symbols_num(); j++) {
                    std::string name;
                    ElfW(Addr) value = 0;
                    ElfW(Xword) size = 0;
                    unsigned char bind = 0;
                    unsigned char type = 0;
                    ElfW(Half) sectionIndex = 0;
                    unsigned char other = 0;
                    symbols.get_symbol(j, name, value, size, bind, type, sectionIndex, other);

                    if (sectionIndex == STN_UNDEF && !name.empty()) {
                        res.push_back(name);
                    }
                }
            }
        }

        return res;
    }

    std::vector<std::string> ElfProgramInfoLoader::getExportedSymbolNames(const LiveContext* context,
        const std::string filepath)
    {
        std::vector<std::string> res;

        // This executable has empty string name on linux
        ELFIO::elfio elfFile;
        std::string realFilepath = filepath.empty() ? context->thisExecutablePath : filepath;
        if (!elfFile.load(realFilepath)) {
            context->events->addLog(LogSeverity::kError, "Cannot load " + realFilepath + " file");
            return res;
        }

        for (uint32_t i = 0; i < elfFile.sections.size(); i++) {
            const auto& section = elfFile.sections[i];
            if (section->get_type() == SHT_SYMTAB) {
                const ELFIO::symbol_section_accessor symbols{elfFile, section};
                res.reserve(symbols.get_symbols_num());
                for (ElfW(Xword) j = 0; j < symbols.get_symbols_num(); j++) {
                    std::string name;
                    ElfW(Addr) value = 0;
                    ElfW(Xword) size = 0;
                    unsigned char bind = 0;
                    unsigned char type = 0;
                    ElfW(Half) sectionIndex = 0;
                    unsigned char other = 0;
                    symbols.get_symbol(j, name, value, size, bind, type, sectionIndex, other);

                    if ((bind == STB_GLOBAL || bind == STB_WEAK) && sectionIndex != STN_UNDEF) {
                        res.push_back(name);
                    }
                }
            }
        }

        return res;
    }
}
