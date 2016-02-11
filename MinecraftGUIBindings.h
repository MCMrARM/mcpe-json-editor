#pragma once

#include "MinecraftGUIVariable.h"

struct MCGUIDataBinding {

    enum class Type {
        GLOBAL, COLLECTION, COLLECTION_DETAILS
    };

    MCGUIVariable<QString> name;
    MCGUIVariable<QString> nameOverride;
    MCGUIVariable<QString> collection;
    MCGUIVariable<Type> type;

};
