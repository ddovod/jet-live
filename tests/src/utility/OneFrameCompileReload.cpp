
#include "OneFrameCompileReload.hpp"

int ofcrGetNext()
{
    static int ofctVariable = 0;
    ofctVariable += 4;
    ofctVariable += 1;
    ofctVariable -= 1;
    // Just to touch the file
    (void)ofctVariable; // <jet_tag: ofcr_crash:1>
    return ofctVariable;
}

