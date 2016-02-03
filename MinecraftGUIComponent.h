#pragma once

#include <QString>
#include <QJsonValue>
#include <QJsonObject>
#include <QList>
#include <QMap>
#include "Vec2.h"
#include "MinecraftGUIVariable.h"
#include "MinecraftGUIBindings.h"

struct MCGUIComponent {

    enum class Type {
        BUTTON, CAROUSEL_LABEL, CUSTOM, EDIT_BOX, GRID, GRID_ITEM, IMAGE, INPUT_PANEL, LABEL, PANEL, SCREEN, SCROLLBAR, SCROLLBAR_BOX, TAB, UNKNOWN
    };

    struct Variables {

        QString requires;
        QMap<QString, QJsonValue> vars;

        Variables(QJsonObject const &o);

    };

    QString mcNamespace;
    QString name;
    MCGUIComponent* base = nullptr;
    Type type;
    MCGUIVariable<bool> ignored;
    QList<Variables> variables;

    MCGUIComponent(const QString &mcNamespace, const QString &name, const QJsonObject &object);

    static Type getTypeFromString(const QString& type);
    static MCGUIComponent* createComponentOfType(Type type, const QString &mcNamespace, const QString &name, QJsonObject const &object);

};

struct MCGUIComponentVariable {

    QString componentName;

    MCGUIComponent* get(MCGUIContext *context);
    void set(QString name) {
        componentName = name;
    }
    MCGUIComponentVariable& operator=(QString name) {
        componentName = name;
        return *this;
    }

};

struct MCGUIControl {

    MCGUIVariable<bool> visible;
    MCGUIVariable<int> layer;
    MCGUIVariable<bool> clipsChildren;
    MCGUIVariable<Vec2> clipOffset;
    MCGUIVariable<bool> allowClipping;
    MCGUIVariable<QJsonObject> propertyBag;

    MCGUIControl(const MCGUIComponent &component, const QJsonObject &object);

};

struct MCGUIButtonControl {

    MCGUIVariable<MCGUIComponentVariable> defaultControl;
    MCGUIVariable<MCGUIComponentVariable> hoverControl;
    MCGUIVariable<MCGUIComponentVariable> pressedControl;

    MCGUIButtonControl(const MCGUIComponent &component, const QJsonObject &object);

};

struct MCGUIDataBindingControl {

    QList<MCGUIDataBinding> bindings;

    MCGUIDataBindingControl(const MCGUIComponent &component, const QJsonObject &object);

};

struct MCGUIComponentButton : public MCGUIComponent, public MCGUIControl, public MCGUIButtonControl, public MCGUIDataBindingControl {

    MCGUIComponentButton(const QString &mcNamespace, const QString &name, QJsonObject const &object);

};
