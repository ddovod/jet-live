
#pragma once

#include <string>
#include <vector>
#include "jet/live/DataTypes.hpp"
#include "jet/live/LiveContext.hpp"

namespace jet
{
    Symbols getSymbols(const LiveContext* context, const std::string& libPath);
}
