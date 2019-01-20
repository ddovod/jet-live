
#include "ExternGlobalVariable.hpp"

extern int someExternGlobalVariable;

std::pair<int, int> getNextGlobal()
{
    someExternGlobalVariable += 1;
    someExternGlobalVariable -= 1;
    return {0, someExternGlobalVariable++};; // <jet_tag: extern_glob_var:1>
//    return {1, someExternGlobalVariable++};; // <jet_tag: extern_glob_var:2>
}
