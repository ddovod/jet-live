#include "jet/live/Symbols.hpp"
#include <memory>
#include <stdio.h>
#include <string.h>
#include <mach-o/loader.h>
#include <mach-o/nlist.h>
#include "jet/live/DataTypes.hpp"

namespace jet
{
    std::vector<std::string> getSymbols(const LiveContext* context, const std::string& libPath)
    {
        std::vector<std::string> res;

        // Loading mach-o binary
        auto f = fopen(libPath.c_str(), "r");
        fseek(f, 0, SEEK_END);
        auto length = ftell(f);
        fseek(f, 0, SEEK_SET);
        auto content = std::unique_ptr<char[]>(new char[length]);
        fread(content.get(), 1, length, f);
        fclose(f);

        //
        auto header = reinterpret_cast<mach_header_64*>(content.get());
        if (header->magic != MH_MAGIC_64) {
            context->delegate->onLog(LogSeverity::kError, "Cannot read symbols, not a Mach-O 64 binary");
            return res;
        }

        uint32_t sectionIndex = 0;
        MachoContext machoContext;
        auto machoPtr = content.get();
        auto commandOffset = sizeof(mach_header_64);
        for (uint32_t iCmd = 0; iCmd < header->ncmds; iCmd++) {
            auto command = reinterpret_cast<load_command*>(machoPtr + commandOffset);
            switch (command->cmd) {
                case LC_SEGMENT_64: {
                    auto segmentCommand = reinterpret_cast<segment_command_64*>(machoPtr + commandOffset);
                    if (strcmp(segmentCommand->segname, "__TEXT") == 0) {
                        // Found __TEXT segment
                        auto sectionsPtr =
                            reinterpret_cast<struct section_64*>(machoPtr + commandOffset + sizeof(*segmentCommand));
                        for (uint32_t i = 0; i < segmentCommand->nsects; i++) {
                            auto& section = sectionsPtr[i];
                            sectionIndex++;
                            if (strcmp(section.sectname, "__text") == 0) {
                                // Found __text section in __TEXT segment
                                machoContext.textSectionIndex = sectionIndex;
                            }
                        }
                    } else {
                        // Skipping whole segment
                        sectionIndex += segmentCommand->nsects;
                    }
                    break;
                }

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
                        machoSymbol.name = stringTable + symbol.n_un.n_strx + 1;  // All symbols starts with '_', so
                                                                                  // just skipping 1 char
                        if (context->delegate->shouldReloadMachoSymbol(machoContext, machoSymbol)) {
                            res.push_back(machoSymbol.name);
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
