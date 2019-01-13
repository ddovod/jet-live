
#pragma once

#include <memory>
#include <string>
#include "jet/live/DataTypes.hpp"

namespace jet
{
    struct LiveContext;

    /**
     * Retreives the path to this executable file.
     */
    std::string getExecutablePath();

    /**
     * c++11 has no `std::make_unique`, so this is a replacement for it.
     */
    template <typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args&&... args)
    {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }

    /**
     * Gives string representation of the elf symbol.
     * Used for debugging.
     */
    std::string toString(const ElfContext& context, const ElfSymbol& elfSymbol);

    /**
     * Gives string representation of the macho symbol.
     * Used for debugging.
     */
    std::string toString(const MachoContext& context, const MachoSymbol& machoSymbol);

    /**
     * Gives string representation of the linker type.
     */
    std::string toString(LinkerType linkerType);

    /**
     * Creates shared library linkage command string.
     * It tries to create a link command to produce the library which will be loaded
     * into the `baseAddress` address.
     * \param libName Name of the shared library.
     * \param compilerPath Path to the compiler.
     * \param baseAddress Preffered base address for the library.
     * \param linkerType Type of the linker.
     * \param objectFilePaths Paths to all object files which should be linked into the library.
     */
    std::string createLinkCommand(const std::string& libName,
        const std::string& compilerPath,
        uintptr_t baseAddress,
        LinkerType linkerType,
        const std::vector<std::string>& objectFilePaths);

    /**
     * Finds base address for library which will be created from the given object files.
     * This function estimates shared library size using `objectFilePaths` object files,
     * finds free page-aligned virtual memory region which can be used to load library
     * into and returns the address of the beginning of the region.
     */
    uintptr_t findPrefferedBaseAddressForLibrary(const std::vector<std::string>& objectFilePaths);

    /**
     * Finds out which linker is used in the system.
     */
    LinkerType getSystemLinkerType(const LiveContext* context);

    /**
     * Retrieves memory mapping of the process.
     * Returns a sorted vector with memory regions.
     * First element is an executable file region.
     * Each region end is next region begin, like
     * `regions[0].regionEnd == regions[1].regionBegin`
     */
    std::vector<MemoryRegion> getMemoryRegions();
}
