
#include "MacosFunctionOutOfScope.hpp"

std::string getStdStringValue()
{
    std::string stringValue = "some "; // <jet_tag: fun_out_of_scope:1>
//    std::string stringValue = "some another "; // <jet_tag: fun_out_of_scope:2>
    stringValue += "string";
    return stringValue;
}
