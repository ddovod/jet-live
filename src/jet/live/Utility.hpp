
#pragma once

#include <memory>
#include <string>

namespace jet
{
    std::string getExecutablePath();

    template <typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args&&... args)
    {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }
}
