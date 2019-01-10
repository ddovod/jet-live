
#pragma once

#include "IClassVirtualMethod.hpp"

class ClassVirtualMethod : public IClassVirtualMethod
{
public:
    int computeResult(int v1, int v2) override;
};
