
#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace jet
{
    struct CompilationUnit
    {
        std::string compilerPath;
        std::string compilationCommandStr;
        std::string compilationDirStr;
        std::string sourceFilePath;
        std::string objFilePath;
        std::string depFilePath;
        bool hasColorDiagnosticsFlag = false;
    };

    struct Symbol
    {
        std::string name;
        size_t size = 0;
        intptr_t runtimeAddress = 0;
    };

    struct Symbols
    {
        std::unordered_map<std::string, Symbol> functions;
        std::unordered_map<std::string, Symbol> variables;
    };

    struct Program
    {
        std::string path;
        void* handle = nullptr;
        Symbols symbols;
    };

    // Mach-O specific structures
    enum class MachoSymbolType : uint8_t
    {
        kUndefined,          // undefined, no section
        kAbsolute,           // absolute, no section
        kSection,            // defined in section
        kPreboundUndefined,  // defined in a dylib
        kIndirect,           // indirect
        kStab,               // symbolic debugging entry
    };

    enum class MachoSymbolReferenceType : uint8_t
    {
        kUndefinedNonLazy,
        kUndefinedLazy,
        kDefined,
        kPrivateDefined,
        kPrivateUndefinedNonLazy,
        kPrivateUndefinedLazy,
    };

    struct MachoSymbol
    {
        std::string name;
        MachoSymbolType type;
        MachoSymbolReferenceType referenceType;
        bool referencedDynamically = false;
        bool descDiscarded = false;
        bool weakRef = false;
        bool weakDef = false;
        uint8_t sectionIndex = 0;      // 0 if NO_SECT
        bool privateExternal = false;  // N_PEXT
        bool external = false;         // N_EXT
    };

    struct MachoContext
    {
        uint32_t textSectionIndex = 0;  // index of '__text' section inside '__TEXT' segment
    };

    // Elf specific structures
    enum class ElfSymbolType : uint8_t
    {
        kNo,
        kObject,
        kFunction,
        kSection,
        kFile,
        kCommonObject,
        kThreadLocalObject,
        kIndirect,
    };

    enum class ElfSymbolBinding : uint8_t
    {
        kLocal,
        kGlobal,
        kWeak,
        kUnique,
    };

    enum class ElfSymbolVisibility : uint8_t
    {
        kDefault,
        kInternal,
        kHidden,
        kProtected,
    };

    struct ElfSymbol
    {
        std::string name;
        ElfSymbolType type;
        ElfSymbolVisibility visibility;
        uint16_t sectionIndex = 0;
        uint64_t size = 0;
        intptr_t virtualAddress = 0;
    };

    struct ElfContext
    {
        std::vector<std::string> sectionNames;
    };
}
