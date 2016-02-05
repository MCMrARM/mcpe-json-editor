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

    virtual Vec2 calculateSize(const MCGUIContext *context) = 0;

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

enum class MCGUIAnchorPoint {

    TOP_LEFT,    TOP_MIDDLE,    TOP_RIGHT,
    LEFT_MIDDLE, CENTER,        RIGHT_MIDDLE,
    BOTTOM_LEFT, BOTTOM_MIDDLE, BOTTOM_RIGHT

};

enum class MCGUIDraggable {

    NOT_DRAGGABLE, HORIZONTAL, VERTICAL, BOTH

};

struct MCGUILayoutAxis {

    struct Component {

        enum class Unit {
            PIXELS, PERCENT_X, PERCENT_Y
        };

        Unit unit;
        float value;

    };

    enum class Axis {
        X, Y
    };

    MCGUILayoutAxis(Axis axis) : axis(axis) {
        //
    }

    Axis axis;

    std::vector<Component> components;

    float get(const MCGUIContext *context);

    void set(const QString &str);

};

struct MCGUILayoutOffset {

    MCGUILayoutAxis x, y;

    MCGUILayoutOffset() : x(MCGUILayoutAxis::Axis::X), y(MCGUILayoutAxis::Axis::Y) {
        //
    }

};

struct MCGUILayoutControl {

    MCGUIVariable<MCGUIAnchorPoint> anchorFrom, anchorTo;
    MCGUIVariable<MCGUIDraggable> draggable;
    MCGUIVariable<bool> followsCursor;
    MCGUIAnimatedVariable<MCGUILayoutOffset> offset, size;

    MCGUILayoutControl(const MCGUIComponent &component, const QJsonObject &object);

};

struct MCGUIComponentButton : public MCGUIComponent, public MCGUIControl, public MCGUIButtonControl, public MCGUIDataBindingControl, public MCGUILayoutControl {

    MCGUIComponentButton(const QString &mcNamespace, const QString &name, QJsonObject const &object);

    virtual Vec2 calculateSize(const MCGUIContext *context) { return { size.get(context).x.get(context), size.get(context).y.get(context) }; }

};

struct MCGUIComponentPanel : public MCGUIComponent, public MCGUIControl, public MCGUIDataBindingControl, public MCGUILayoutControl {

    MCGUIComponentPanel(const QString &mcNamespace, const QString &name, QJsonObject const &object);

    virtual Vec2 calculateSize(const MCGUIContext *context) { return { size.get(context).x.get(context), size.get(context).y.get(context) }; }

};
