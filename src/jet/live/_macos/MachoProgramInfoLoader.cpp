
#include "MachoProgramInfoLoader.hpp"
#include <dlfcn.h>
#include <teenypath.h>
#include <mach-o/dyld.h>
#include <mach-o/dyld_images.h>
#include <mach-o/nlist.h>
#include "jet/live/LiveContext.hpp"
#include <set>

namespace jet
{
    std::vector<std::string> MachoProgramInfoLoader::getAllLoadedProgramsPaths(const LiveContext* context) const
    {
        std::vector<std::string> filepaths;

        for (uint32_t i = 0; i < _dyld_image_count(); i++) {
            auto imagePath = TeenyPath::path{_dyld_get_image_name(i)}.resolve_absolute();
            if (imagePath.string() == context->thisExecutablePath) {
                filepaths.emplace_back("");
            } else {
                filepaths.emplace_back(imagePath.string());
            }
        }

        return filepaths;
    }

    Symbols MachoProgramInfoLoader::getProgramSymbols(const LiveContext* context, const std::string& filepath) const
    {
        Symbols res;
        MachoContext machoContext;

        intptr_t imageAddressSlide = 0;
        bool found = false;
        std::string realFilepath = filepath.empty() ? context->thisExecutablePath : filepath;
        for (uint32_t i = 0; i < _dyld_image_count(); i++) {
            auto imagePath = TeenyPath::path{_dyld_get_image_name(i)}.resolve_absolute();
            if (imagePath.string() == realFilepath) {
                imageAddressSlide = _dyld_get_image_vmaddr_slide(i);
                found = true;
                break;
            }
        }
        if (!found) {
            context->delegate->onLog(LogSeverity::kError, "Cannot find address slide of image " + realFilepath);
            return res;
        }

        const auto baseAddress = static_cast<uintptr_t>(imageAddressSlide);

        // Parsing mach-o binary
        auto f = fopen(realFilepath.c_str(), "r");
        fseek(f, 0, SEEK_END);
        auto length = static_cast<size_t>(ftell(f));
        fseek(f, 0, SEEK_SET);
        auto content = std::unique_ptr<char[]>(new char[length]);
        fread(content.get(), 1, length, f);
        fclose(f);

        auto header = reinterpret_cast<mach_header_64*>(content.get());
        if (header->magic != MH_MAGIC_64) {
            // Probably it is some system "fat" library, we're not interested in it
            // context->delegate->onLog(LogSeverity::kError, "Cannot read symbols, not a Mach-O 64 binary");
            return res;
        }

        uint32_t sectionIndex = 0;
        auto machoPtr = content.get();
        auto commandOffset = sizeof(mach_header_64);
        std::vector<std::set<uint64_t>> symbolsBounds;
        for (uint32_t iCmd = 0; iCmd < header->ncmds; iCmd++) {
            auto command = reinterpret_cast<load_command*>(machoPtr + commandOffset);
            switch (command->cmd) {
                case LC_SEGMENT_64: {
                    auto segmentCommand = reinterpret_cast<segment_command_64*>(machoPtr + commandOffset);
                    auto sectionsPtr =
                        reinterpret_cast<struct section_64*>(machoPtr + commandOffset + sizeof(*segmentCommand));
                    for (uint32_t i = 0; i < segmentCommand->nsects; i++) {
                        auto& section = sectionsPtr[i];
                        sectionIndex++;
                        if (machoContext.sectionNames.size() <= sectionIndex) {
                            machoContext.sectionNames.resize(sectionIndex + 1);
                            symbolsBounds.resize(sectionIndex + 1);
                        }
                        machoContext.sectionNames[sectionIndex] = std::string(section.sectname);
                        symbolsBounds[sectionIndex].insert(section.addr + section.size);
                    }
                    break;
                }

                default: break;
            }

            commandOffset += command->cmdsize;
        }

        commandOffset = sizeof(mach_header_64);
        for (uint32_t iCmd = 0; iCmd < header->ncmds; iCmd++) {
            auto command = reinterpret_cast<load_command*>(machoPtr + commandOffset);
            switch (command->cmd) {
                case LC_SYMTAB: {
                    auto table = reinterpret_cast<symtab_command*>(machoPtr + commandOffset);
                    auto symbolsPtr = reinterpret_cast<nlist_64*>(machoPtr + table->symoff);
                    for (uint32_t i = 0; i < table->nsyms; i++) {
                        if ((symbolsPtr[i].n_type & N_TYPE) == N_SECT) {
                            symbolsBounds[symbolsPtr[i].n_sect].insert(symbolsPtr[i].n_value);
                        }
                    }
                    break;
                }

                default: break;
            }
            commandOffset += command->cmdsize;
        }

        commandOffset = sizeof(mach_header_64);
        for (uint32_t iCmd = 0; iCmd < header->ncmds; iCmd++) {
            auto command = reinterpret_cast<load_command*>(machoPtr + commandOffset);
            switch (command->cmd) {
                case LC_SYMTAB: {
                    auto table = reinterpret_cast<symtab_command*>(machoPtr + commandOffset);
                    auto symbolsPtr = reinterpret_cast<nlist_64*>(machoPtr + table->symoff);
                    auto stringTable = machoPtr + table->stroff;
                    for (uint32_t i = 0; i < table->nsyms; i++) {
                        auto& symbol = symbolsPtr[i];
                        MachoSymbol machoSymbol;
                        if (symbol.n_type & N_STAB) {
                            machoSymbol.type = MachoSymbolType::kStab;
                        } else {
                            switch (symbol.n_type & N_TYPE) {
                                case N_UNDF: machoSymbol.type = MachoSymbolType::kUndefined; break;
                                case N_ABS: machoSymbol.type = MachoSymbolType::kAbsolute; break;
                                case N_SECT: machoSymbol.type = MachoSymbolType::kSection; break;
                                case N_PBUD: machoSymbol.type = MachoSymbolType::kPreboundUndefined; break;
                                case N_INDR: machoSymbol.type = MachoSymbolType::kIndirect; break;
                                default: continue;  // Some symbol we're not interested in
                            }
                        }

                        switch (symbol.n_desc & REFERENCE_TYPE) {
                            case REFERENCE_FLAG_UNDEFINED_NON_LAZY:
                                machoSymbol.referenceType = MachoSymbolReferenceType::kUndefinedNonLazy;
                                break;
                            case REFERENCE_FLAG_UNDEFINED_LAZY:
                                machoSymbol.referenceType = MachoSymbolReferenceType::kUndefinedLazy;
                                break;
                            case REFERENCE_FLAG_DEFINED:
                                machoSymbol.referenceType = MachoSymbolReferenceType::kDefined;
                                break;
                            case REFERENCE_FLAG_PRIVATE_DEFINED:
                                machoSymbol.referenceType = MachoSymbolReferenceType::kPrivateDefined;
                                break;
                            case REFERENCE_FLAG_PRIVATE_UNDEFINED_NON_LAZY:
                                machoSymbol.referenceType = MachoSymbolReferenceType::kPrivateUndefinedNonLazy;
                                break;
                            case REFERENCE_FLAG_PRIVATE_UNDEFINED_LAZY:
                                machoSymbol.referenceType = MachoSymbolReferenceType::kPrivateUndefinedLazy;
                                break;
                        }

                        machoSymbol.referencedDynamically = symbol.n_desc & REFERENCED_DYNAMICALLY;
                        machoSymbol.descDiscarded = symbol.n_desc & N_DESC_DISCARDED;
                        machoSymbol.weakRef = symbol.n_desc & N_WEAK_REF;
                        machoSymbol.weakDef = symbol.n_desc & N_WEAK_DEF;
                        machoSymbol.privateExternal = symbol.n_type & N_PEXT;
                        machoSymbol.external = symbol.n_type & N_EXT;
                        machoSymbol.sectionIndex = symbol.n_sect;
                        machoSymbol.virtualAddress = symbol.n_value;
                        // All symbol names starts with '_', so just skipping 1 char
                        machoSymbol.name = stringTable + symbol.n_un.n_strx + 1;

                        if (machoSymbol.type == MachoSymbolType::kSection) {
                            auto addrFound = symbolsBounds[machoSymbol.sectionIndex].find(machoSymbol.virtualAddress);
                            assert(addrFound != symbolsBounds[machoSymbol.sectionIndex].end());
                            addrFound++;
                            if (addrFound != symbolsBounds[machoSymbol.sectionIndex].end()) {
                                machoSymbol.size = *addrFound - machoSymbol.virtualAddress;
                            } else {
                                context->delegate->onLog(LogSeverity::kDebug, "wtf?");
                            }
                        }

                        Symbol sym;
                        sym.name = machoSymbol.name;
                        sym.runtimeAddress = baseAddress + machoSymbol.virtualAddress;
                        sym.size = machoSymbol.size;

                        if (context->delegate->shouldReloadMachoSymbol(machoContext, machoSymbol)) {
                            res.functions[sym.name] = sym;
                        }

                        if (context->delegate->shouldTransferMachoSymbol(machoContext, machoSymbol)) {
                            res.variables[sym.name] = sym;
                        }
                    }
                    break;
                }

                default: break;
            }
            commandOffset += command->cmdsize;
        }
        return res;
    }
}
