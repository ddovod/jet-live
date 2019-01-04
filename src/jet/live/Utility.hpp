
#pragma once

#include <memory>
#include <string>

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
}
