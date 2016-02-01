#pragma once

#include <QString>
#include <QMap>
#include "Vec2.h"
class QJsonValue;
class QJsonObject;
class MinecraftGUIComponent;

class MinecraftJSONParser
{
public:
    MinecraftJSONParser();

    QMap<QString, MinecraftGUIComponent*> resolvedComponents;

    void loadJSONFile(const QString &file);
    MinecraftGUIComponent* parseComponent(QString name, const QString &mcNamespace, const QJsonObject &object);

    static Vec2 getVec2(const QJsonValue &val, Vec2 def);
};
