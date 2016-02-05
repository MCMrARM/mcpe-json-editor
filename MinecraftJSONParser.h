#pragma once

#include <QString>
#include <QMap>
#include <QList>
#include <functional>
#include "Vec2.h"
class QJsonValue;
class QJsonObject;
class MCGUIComponent;

class MinecraftJSONParser
{
public:
    typedef std::function<void (MCGUIComponent* component)> ComponentCallback;

    MinecraftJSONParser();

    QMap<QString, MCGUIComponent*> resolvedComponents;
    QMap<QString, QList<ComponentCallback>> resolveCallbacks;

    void loadJSONFile(const QString &file);
    void parseComponent(QString name, const QString &mcNamespace, const QJsonObject &object, ComponentCallback parseCallback);
    void parseAndRegisterComponent(QString name, const QString &mcNamespace, const QJsonObject &object);

    void registerComponent(MCGUIComponent* component);
    void requireComponent(const QString& name, ComponentCallback callback);

    void checkForMissingComponents();
};
