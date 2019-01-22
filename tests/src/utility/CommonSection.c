
#include "CommonSection.h"

int commonSectionVariable;
char initialized = 0;

int getCommonSectionVar()
{
    if (initialized == 0) {
        commonSectionVariable = 10;
        initialized = 1;
    }
//    (void)initialized; // <jet_tag: common_sect:1>
    commonSectionVariable += 1;
    commonSectionVariable -= 1;
    return commonSectionVariable++;
}

void* getCommonSectionVarAddress()
{
    commonSectionVariable += 1;
    commonSectionVariable -= 1;
    return (void*)(&commonSectionVariable);
}

