#pragma once

#include <QString>
#include <QMap>
#include <QJsonValue>

struct MCGUIComponent;

class MCGUIContext {

public:
    QMap<QString, QJsonValue> variables;
    QList<QMap<QString, QJsonValue>> prevVariables;
    QMap<QString, MCGUIComponent*>& components;

    void enter(MCGUIComponent* component);
    void exit();

};
