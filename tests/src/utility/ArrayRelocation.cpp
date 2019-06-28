
#include "ArrayRelocation.hpp"

namespace
{
    int values[] = {
        533, 868
    };
}

int arelTouchValues()
{
    int res = 0;
    for (auto& el : values) {
        res += (el += 10);
    }
    return res;
}

int arelGetValue()
{
   return values[0]; // <jet_tag: arel_var:1>
//   return values[1]; // <jet_tag: arel_var:2>
}
