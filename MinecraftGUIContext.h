#pragma once

#include <QString>
#include <QMap>
#include <QJsonValue>
#include "Vec2.h"

struct MCGUIComponent;

class MCGUIContext {

public:
    QMap<QString, QJsonValue> variables;
    QList<QMap<QString, QJsonValue>> prevVariables;
    QMap<QString, MCGUIComponent*>& components;
    QList<MCGUIComponent*> componentStack;
    Vec2 screenSize;

    MCGUIContext(QMap<QString, MCGUIComponent*>& components) : components(components) {
        //
    }

    void enter(MCGUIComponent* component);
    void exit();

};
