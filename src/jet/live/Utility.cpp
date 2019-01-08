
#include "Utility.hpp"
#include <whereami.h>

namespace jet
{
    std::string getExecutablePath()
    {
        std::string pathString;
        auto pathLength = static_cast<size_t>(wai_getExecutablePath(nullptr, 0, nullptr));
        pathString.resize(pathLength);
        wai_getExecutablePath(const_cast<char*>(pathString.data()), static_cast<int>(pathLength), nullptr);
        return pathString;
    }

    std::string toString(const ElfContext& context, const ElfSymbol& elfSymbol)
    {
        std::string s;

        switch (elfSymbol.type) {
            case ElfSymbolType::kNo: s += "No\t\t\t"; break;
            case ElfSymbolType::kObject: s += "Object\t\t"; break;
            case ElfSymbolType::kFunction: s += "Function\t\t"; break;
            case ElfSymbolType::kSection: s += "Section\t\t"; break;
            case ElfSymbolType::kFile: s += "File\t\t"; break;
            case ElfSymbolType::kCommonObject: s += "CommonObject\t"; break;
            case ElfSymbolType::kThreadLocalObject: s += "ThreadLocalObject\t"; break;
            case ElfSymbolType::kIndirect: s += "Indirect\t\t"; break;
        }

        s += "| ";
        switch (elfSymbol.visibility) {
            case ElfSymbolVisibility::kDefault: s += "Default\t"; break;
            case ElfSymbolVisibility::kInternal: s += "Internal\t"; break;
            case ElfSymbolVisibility::kHidden: s += "Hidden\t"; break;
            case ElfSymbolVisibility::kProtected: s += "Protected\t"; break;
        }

        s += "| ";
        const auto& sectionName1 = context.sectionNames.size() <= elfSymbol.sectionIndex ?
                                       std::string("?") :
                                       context.sectionNames[elfSymbol.sectionIndex];
        s += std::to_string(elfSymbol.sectionIndex) + "(" + sectionName1 + ")\t| ";
        s += "ADDR " + std::to_string(elfSymbol.virtualAddress) + "\t| ";
        s += "SIZE " + std::to_string(elfSymbol.size) + "\t| ";
        s += elfSymbol.name;

        return s;
    }

    std::string toString(const MachoContext& context, const MachoSymbol& machoSymbol)
    {
        std::string s;

        switch (machoSymbol.type) {
            case MachoSymbolType::kUndefined: s += "Undefined\t\t"; break;
            case MachoSymbolType::kAbsolute: s += "Absolute\t\t"; break;
            case MachoSymbolType::kSection: s += "Section\t\t"; break;
            case MachoSymbolType::kPreboundUndefined: s += "PreboundUndefined\t"; break;
            case MachoSymbolType::kIndirect: s += "Indirect\t\t"; break;
            case MachoSymbolType::kStab: s += "Stub\t\t\t"; break;
        }

        s += "| ";
        switch (machoSymbol.referenceType) {
            case MachoSymbolReferenceType::kUndefinedNonLazy: s += "UndefinedNonLazy\t\t"; break;
            case MachoSymbolReferenceType::kUndefinedLazy: s += "UndefinedLazy\t\t"; break;
            case MachoSymbolReferenceType::kDefined: s += "Defined\t\t\t"; break;
            case MachoSymbolReferenceType::kPrivateDefined: s += "PrivateDefined\t\t"; break;
            case MachoSymbolReferenceType::kPrivateUndefinedNonLazy: s += "PrivateUndefinedNonLazy\t"; break;
            case MachoSymbolReferenceType::kPrivateUndefinedLazy: s += "PrivateUndefinedLazy\t"; break;
        };

        s += "| ";
        s += "RD " + std::to_string(static_cast<int>(machoSymbol.referencedDynamically)) + "\t| ";
        s += "DD " + std::to_string(static_cast<int>(machoSymbol.descDiscarded)) + "\t| ";
        s += "WR " + std::to_string(static_cast<int>(machoSymbol.weakRef)) + "\t| ";
        s += "WD " + std::to_string(static_cast<int>(machoSymbol.weakDef)) + "\t| ";
        s += "PE " + std::to_string(static_cast<int>(machoSymbol.privateExternal)) + "\t| ";
        s += "E " + std::to_string(static_cast<int>(machoSymbol.external)) + "\t| ";
        const auto& sectionName1 = context.sectionNames.size() <= machoSymbol.sectionIndex ?
                                       std::string("?") :
                                       context.sectionNames[machoSymbol.sectionIndex];
        s += std::to_string(machoSymbol.sectionIndex) + "(" + sectionName1 + ")\t| ";
        s += "ADDR " + std::to_string(machoSymbol.virtualAddress) + "\t| ";
        s += "SIZE " + std::to_string(machoSymbol.size) + "\t| ";
        s += machoSymbol.name;

        return s;
    }
}
