
#pragma once

class IClassVirtualMethod
{
public:
    virtual ~IClassVirtualMethod() {}

    virtual int computeResult(int v1, int v2) = 0;
};
