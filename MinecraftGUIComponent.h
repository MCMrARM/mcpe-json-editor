#pragma once

#include <QString>
#include <QJsonValue>
#include <QJsonObject>
#include <QList>
#include <QMap>
#include "Vec2.h"
#include "MinecraftGUIVariable.h"
#include "MinecraftGUIBindings.h"

class MinecraftJSONParser;

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
    QList<MCGUIComponent*> controls;

    MCGUIComponent(MinecraftJSONParser& parser, const QString &mcNamespace, const QString &name, const MCGUIComponent *base, const QJsonObject &object);

    static Type getTypeFromString(const QString& type);
    static MCGUIComponent* createComponentOfType(MinecraftJSONParser &parser, Type type, const QString &mcNamespace, const QString &name, const MCGUIComponent *base, QJsonObject const &object);

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

    MCGUIVariable<bool> visible = true;
    MCGUIVariable<int> layer = 0;
    MCGUIVariable<bool> clipsChildren = false;
    MCGUIVariable<Vec2> clipOffset;
    MCGUIVariable<bool> allowClipping = true;
    MCGUIVariable<QJsonObject> propertyBag;

    MCGUIControl(const MCGUIComponent &component, const MCGUIComponent *base, const QJsonObject &object);

};

struct MCGUIButtonComponent {

    MCGUIVariable<MCGUIComponentVariable> defaultControl;
    MCGUIVariable<MCGUIComponentVariable> hoverControl;
    MCGUIVariable<MCGUIComponentVariable> pressedControl;

    MCGUIButtonComponent(const MCGUIComponent &component, const MCGUIComponent *base, const QJsonObject &object);

};

struct MCGUIDataBindingComponent {

    QList<MCGUIDataBinding> bindings;

    MCGUIDataBindingComponent(const MCGUIComponent &component, const MCGUIComponent *base, const QJsonObject &object);

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
    void set(const QJsonValue &obj);

};

struct MCGUILayoutOffset {

    MCGUILayoutAxis x, y;

    MCGUILayoutOffset() : x(MCGUILayoutAxis::Axis::X), y(MCGUILayoutAxis::Axis::Y) {
        //
    }

};

struct MCGUILayoutComponent {

    MCGUIVariable<MCGUIAnchorPoint> anchorFrom = MCGUIAnchorPoint::CENTER;
    MCGUIVariable<MCGUIAnchorPoint> anchorTo = MCGUIAnchorPoint::CENTER;
    MCGUIVariable<MCGUIDraggable> draggable = MCGUIDraggable::NOT_DRAGGABLE;
    MCGUIVariable<bool> followsCursor = false;
    MCGUIAnimatedVariable<MCGUILayoutOffset> offset, size;

    MCGUILayoutComponent(const MCGUIComponent &component, const MCGUIComponent *base, const QJsonObject &object);

};

struct MCGUIComponentButton : public MCGUIComponent, public MCGUIControl, public MCGUIButtonComponent, public MCGUIDataBindingComponent, public MCGUILayoutComponent {

    MCGUIComponentButton(MinecraftJSONParser &parser, const QString &mcNamespace, const QString &name, const MCGUIComponent *base, QJsonObject const &object);

    virtual Vec2 calculateSize(const MCGUIContext *context) { return { size.get(context).x.get(context), size.get(context).y.get(context) }; }

};

struct MCGUIComponentPanel : public MCGUIComponent, public MCGUIControl, public MCGUIDataBindingComponent, public MCGUILayoutComponent {

    MCGUIComponentPanel(MinecraftJSONParser &parser, const QString &mcNamespace, const QString &name, const MCGUIComponent *base, QJsonObject const &object);

    virtual Vec2 calculateSize(const MCGUIContext *context) { return { size.get(context).x.get(context), size.get(context).y.get(context) }; }

};
