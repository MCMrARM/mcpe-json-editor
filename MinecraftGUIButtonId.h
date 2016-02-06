#pragma once

#include <QString>

struct MCGUIButtonId {

    int id;

    static MCGUIButtonId getByString(QString id);

};
