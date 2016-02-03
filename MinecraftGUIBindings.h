#pragma once

#include "MinecraftGUIVariable.h"

struct MCGUIDataBinding {

    static const int TYPE_GLOBAL = 0;
    static const int TYPE_COLLECTION = 0;
    static const int TYPE_COLLECTION_DETAILS = 0;

    MCGUIVariable<QString> name;
    MCGUIVariable<QString> nameOverride;
    MCGUIVariable<QString> collection;
    MCGUIVariable<int> type;

};
