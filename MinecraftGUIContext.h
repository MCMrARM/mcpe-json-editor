#pragma once

#include <QString>
#include <QMap>
#include <QJsonValue>

struct MCGUIComponent;

class MCGUIContext {

public:
    QMap<QString, QJsonValue> variables;
    QMap<QString, MCGUIComponent*>& components;

};
