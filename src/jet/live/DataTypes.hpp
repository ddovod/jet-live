
#pragma once

#include <string>
#include <unordered_map>
#include <utils.hpp>
#include <vector>

namespace jet
{
    /**
     * The type of a linker.
     * Different linkers have different capabilities, so we should
     * know which linker we are using.
     */
    enum class LinkerType
    {
        kUnknown,   /** Unknown linker. */
        kLLVM_lld,  /** LLVM LLD linker. */
        kLLVM_lld6, /** This version of lld is broken. https://reviews.llvm.org/D45261 */
        kGNU_ld,    /** GNU ld linker. */
        kApple_ld,  /** Apple ld linker. */
    };

    /**
     * Represents a region of virtual memory.
     * Used to find free region to place shared library into.
     */
    struct MemoryRegion
    {
        uintptr_t regionBegin = 0;
        uintptr_t regionEnd = 0;
        bool isInUse = false;
    };

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
        uint64_t hash = 0;            /** Connects local symbol to the file it belongs to, 0 for non-locals. */
    };

    /**
     * Represents a set of symbols by types.
     */
    struct Symbols
    {
        std::unordered_map<std::string, small_vector<Symbol, 1>> functions; /** Hookable function symbols. */
        std::unordered_map<std::string, small_vector<Symbol, 1>> variables; /** Transferrable variable symbols. */
    };

    /**
     * Represents an executable or a shared library info.
     */
    struct Program
    {
        std::string path; /** Program filepath. Empty for this executable. */
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

        // These are symbolic debugging entry types (if symbol.n_type & N_STAB != 0)
        kGSYM,    /* global symbol: name,,NO_SECT,type,0 */
        kFNAME,   /* procedure name (f77 kludge): name,,NO_SECT,0,0 */
        kFUN,     /* procedure: name,,n_sect,linenumber,address */
        kSTSYM,   /* static symbol: name,,n_sect,type,address */
        kLCSYM,   /* .lcomm symbol: name,,n_sect,type,address */
        kBNSYM,   /* begin nsect sym: 0,,n_sect,0,address */
        kAST,     /* AST file path: name,,NO_SECT,0,0 */
        kOPT,     /* emitted with gcc2_compiled and in gcc source */
        kRSYM,    /* register sym: name,,NO_SECT,type,register */
        kSLINE,   /* src line: 0,,n_sect,linenumber,address */
        kENSYM,   /* end nsect sym: 0,,n_sect,0,address */
        kSSYM,    /* structure elt: name,,NO_SECT,type,struct_offset */
        kSO,      /* source file name: name,,n_sect,0,address */
        kOSO,     /* object file name: name,,0,0,st_mtime */
        kLSYM,    /* local sym: name,,NO_SECT,type,offset */
        kBINCL,   /* include file beginning: name,,NO_SECT,0,sum */
        kSOL,     /* #included file name: name,,n_sect,0,address */
        kPARAMS,  /* compiler parameters: name,,NO_SECT,0,0 */
        kVERSION, /* compiler version: name,,NO_SECT,0,0 */
        kOLEVEL,  /* compiler -O level: name,,NO_SECT,0,0 */
        kPSYM,    /* parameter: name,,NO_SECT,type,offset */
        kEINCL,   /* include file end: name,,NO_SECT,0,0 */
        kENTRY,   /* alternate entry: name,,n_sect,linenumber,address */
        kLBRAC,   /* left bracket: 0,,NO_SECT,nesting level,address */
        kEXCL,    /* deleted include file: name,,NO_SECT,0,sum */
        kRBRAC,   /* right bracket: 0,,NO_SECT,nesting level,address */
        kBCOMM,   /* begin common: name,,NO_SECT,0,0 */
        kECOMM,   /* end common: name,,n_sect,0,0 */
        kECOML,   /* end common (local name): 0,,n_sect,0,address */
        kLENG,    /* second stab entry with length information */
        kPC,      /* global pascal symbol: name,,NO_SECT,subtype,line */
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
        uint64_t size = 0;
        uintptr_t virtualAddress = 0;  // n_value
    };

    struct MachoContext
    {
        std::vector<std::string> sectionNames;
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
        ElfSymbolBinding binding;
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
