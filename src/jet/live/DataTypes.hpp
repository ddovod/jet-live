
#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace jet
{
    /**
     * Represents compilation unit.
     */
    struct CompilationUnit
    {
        std::string compilerPath;             /** Path to the compiler. */
        std::string compilationCommandStr;    /** Full compilation command. */
        std::string compilationDirStr;        /** Working directory from which this cu was compiled. */
        std::string sourceFilePath;           /** Path to the source file. */
        std::string objFilePath;              /** Path to the object file. */
        std::string depFilePath;              /** Path to the `.d` depfile. */
        bool hasColorDiagnosticsFlag = false; /** If `-fcolor-diagnostics` flag is used. */
    };

    /**
     * Represents common symbol.
     */
    struct Symbol
    {
        std::string name;             /** Mangled name of the symbol. */
        size_t size = 0;              /** Size of the symbol. */
        uintptr_t runtimeAddress = 0; /** A pointer to the symbol. */
    };

    /**
     * Represents a set of symbols by types.
     */
    struct Symbols
    {
        std::unordered_map<std::string, Symbol> functions; /** Hookable function symbols. */
        std::unordered_map<std::string, Symbol> variables; /** Transferrable variable symbols. */
    };

    /**
     * Represents an executable or a shared library info.
     */
    struct Program
    {
        std::string path; /** Program filepath. */
        Symbols symbols;  /** Sybmols of the program. */
    };

    // Mach-O specific structures
    // For more info please refer to `mach-o/loader.h` and `mach-o/nlist.h`
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
    // For more info please refer to `link.h` and `elf.h`
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
        uintptr_t virtualAddress = 0;
    };

    struct ElfContext
    {
        std::vector<std::string> sectionNames;
    };
}
