
#pragma once

#include <memory>
#include <string>
#include "jet/live/DataTypes.hpp"

namespace jet
{
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
}
