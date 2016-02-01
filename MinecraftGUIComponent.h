#pragma once

#include <QString>
#include <QJsonObject>
#include "Vec2.h"

struct MinecraftGUIComponent {

    enum class Type {
        BUTTON, CAROUSEL_LABEL, CUSTOM, EDIT_BOX, GRID, GRID_ITEM, IMAGE, INPUT_PANEL, LABEL, PANEL, SCREEN, SCROLLBAR, SCROLLBAR_BOX, TAB, UNKNOWN
    };

    QString mcNamespace;
    QString name;
    QString base;
    Type type;

    MinecraftGUIComponent(const QString &mcNamespace, const QString &name, const QString &base);

    static Type getTypeFromString(const QString& type);

};

struct MinecraftGUIControl {

    bool visible;
    int layer;
    bool clipsChildren;
    Vec2 clipOffset;
    bool allowClipping;
    QJsonObject propertyBag;

    MinecraftGUIControl(QJsonObject const &object);

};

struct MinecraftGUIComponentButton : public MinecraftGUIComponent, public MinecraftGUIControl {

    MinecraftGUIComponentButton(const QString &mcNamespace, const QString &name, const QString &base, QJsonObject const &object);

};
