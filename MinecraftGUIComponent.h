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
    QMap<QString, MCGUIComponent*> controls;

    MCGUIComponent(MinecraftJSONParser& parser, const QString &mcNamespace, const QString &name, Type type, const MCGUIComponent *base, const QJsonObject &object);

    static Type getTypeFromString(const QString& type);
    static MCGUIComponent* createComponentOfType(MinecraftJSONParser &parser, Type type, const QString &mcNamespace, const QString &name, const MCGUIComponent *base, QJsonObject const &object);

    virtual Vec2 calculateSize(const MCGUIContext *context) = 0;

};

struct MCGUIBaseControl {

    MCGUIVariable<bool> visible = true;
    MCGUIVariable<int> layer = 0;
    MCGUIVariable<bool> clipsChildren = false;
    MCGUIVariable<Vec2> clipOffset;
    MCGUIVariable<bool> allowClipping = true;
    MCGUIVariable<QJsonObject> propertyBag;

    MCGUIBaseControl(const MCGUIComponent &component, const MCGUIComponent *base, const QJsonObject &object);

};

struct MCGUIBaseDataBindingComponent {

    QList<MCGUIDataBinding> bindings;

    MCGUIBaseDataBindingComponent(const MCGUIComponent &component, const MCGUIComponent *base, const QJsonObject &object);

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

    Vec2 get(const MCGUIContext *context) {
        return { x.get(context), y.get(context) };
    }

};

struct MCGUIBaseLayoutComponent {

    MCGUIVariable<MCGUIAnchorPoint> anchorFrom = MCGUIAnchorPoint::CENTER;
    MCGUIVariable<MCGUIAnchorPoint> anchorTo = MCGUIAnchorPoint::CENTER;
    MCGUIVariable<MCGUIDraggable> draggable = MCGUIDraggable::NOT_DRAGGABLE;
    MCGUIVariable<bool> followsCursor = false;
    MCGUIAnimatedVariable<MCGUILayoutOffset> offset, size;

    MCGUIBaseLayoutComponent(const MCGUIComponent &component, const MCGUIComponent *base, const QJsonObject &object);

};

struct MCGUIBaseInputComponent {

    // TODO: Keymappings
    MCGUIVariable<QList<MCGUIComponentVariable>> scrollReport;
    MCGUIVariable<bool> alwaysListenToInput = false;
    MCGUIVariable<bool> focusEnabled = false;
    MCGUIVariable<int> defaultFocusPrecedence = 0;
    MCGUIVariable<bool> alwaysHandlePointer = false;
    MCGUIVariable<MCGUIComponentVariable> contentPanel;

    MCGUIBaseInputComponent(const MCGUIComponent &component, const MCGUIComponent *base, const QJsonObject &object);

};

struct MCGUIBaseSoundComponent {

    MCGUIVariable<QString> soundName;
    MCGUIVariable<float> soundVolume = 1.f;
    MCGUIVariable<float> soundPitch = 1.f;

    MCGUIBaseSoundComponent(const MCGUIComponent &component, const MCGUIComponent *base, const QJsonObject &object);

};

struct MCGUIBaseButtonComponent {

    MCGUIVariable<MCGUIControlVariable> defaultControl;
    MCGUIVariable<MCGUIControlVariable> hoverControl;
    MCGUIVariable<MCGUIControlVariable> pressedControl;

    MCGUIBaseButtonComponent(const MCGUIComponent &component, const MCGUIComponent *base, const QJsonObject &object);

};

struct MCGUIColor {

    float r, g, b, a;

    static const MCGUIColor WHITE;

};

struct MCGUIBaseTextComponent {

    MCGUIVariable<QString> text;
    MCGUIVariable<MCGUIAnchorPoint> alignment = MCGUIAnchorPoint::CENTER;
    MCGUIVariable<MCGUIColor> color = MCGUIColor::WHITE;
    MCGUIVariable<float> alpha = 1.f;
    MCGUIVariable<bool> shadow = false;
    //MCGUIVariable<MCGUIFontSize> fontSize;
    MCGUIVariable<bool> wrap = false;
    MCGUIVariable<bool> clip = false;
    MCGUIVariable<bool> localize = true;
    MCGUIVariable<bool> runeFont = false;

    MCGUIBaseTextComponent(const MCGUIComponent &component, const MCGUIComponent *base, const QJsonObject &object);

};

struct MCGUIBaseCarouselTextComponent {

    MCGUIVariable<QString> text;
    MCGUIVariable<MCGUIAnchorPoint> alignment = MCGUIAnchorPoint::CENTER;
    MCGUIVariable<MCGUIColor> color = MCGUIColor::WHITE;
    MCGUIVariable<float> alpha = 1.f;
    MCGUIVariable<bool> shadow = false;
    //MCGUIVariable<MCGUIFontSize> fontSize;
    MCGUIVariable<bool> wrap = false;
    MCGUIVariable<bool> clip = false;
    MCGUIVariable<bool> localize = true;
    MCGUIVariable<bool> alwaysRotate = false;
    MCGUIVariable<float> rotateSpeed = 0.1f;
    MCGUIVariable<MCGUIColor> hoverColor = MCGUIColor::WHITE;
    MCGUIVariable<float> hoverAlpha = 1.f;
    MCGUIVariable<MCGUIColor> pressedColor = MCGUIColor::WHITE;
    MCGUIVariable<float> pressedAlpha = 1.f;

    MCGUIBaseCarouselTextComponent(const MCGUIComponent &component, const MCGUIComponent *base, const QJsonObject &object);

};

struct MCGUIBaseCustomRendererComponent {

    MCGUIVariable<QString> renderer;

    MCGUIBaseCustomRendererComponent(const MCGUIComponent &component, const MCGUIComponent *base, const QJsonObject &object);

};

enum class MCGUITextType {

    ExtendedASCII, IdentifierChars, NumberChars

};

struct MCGUIBaseTextEditComponent {

    MCGUIVariable<MCGUITextType> textType = MCGUITextType::ExtendedASCII;
    MCGUIVariable<int> maxLength = 0;
    MCGUIVariable<bool> enabled = true;
    MCGUIVariable<MCGUIControlVariable> textControl;

    MCGUIBaseTextEditComponent(const MCGUIComponent &component, const MCGUIComponent *base, const QJsonObject &object);

};

struct MCGUIBaseTabComponent {

    MCGUIVariable<int> tabGroup = 0;
    MCGUIVariable<int> tabIndex = 0;
    MCGUIVariable<MCGUIControlVariable> tabContent;

    MCGUIBaseTabComponent(const MCGUIComponent &component, const MCGUIComponent *base, const QJsonObject &object);

};

struct MCGUIComponentButton : public MCGUIComponent, public MCGUIBaseControl, public MCGUIBaseButtonComponent, public MCGUIBaseDataBindingComponent, public MCGUIBaseLayoutComponent, public MCGUIBaseInputComponent, public MCGUIBaseSoundComponent {

    MCGUIComponentButton(MinecraftJSONParser &parser, const QString &mcNamespace, const QString &name, const MCGUIComponent *base, QJsonObject const &object);

    virtual Vec2 calculateSize(const MCGUIContext *context) { return size.get(context).get(context); }

};

struct MCGUIComponentCarouselLabel : public MCGUIComponent, public MCGUIBaseControl, public MCGUIBaseDataBindingComponent, public MCGUIBaseLayoutComponent, public MCGUIBaseInputComponent, public MCGUIBaseCarouselTextComponent {

    MCGUIComponentCarouselLabel(MinecraftJSONParser &parser, const QString &mcNamespace, const QString &name, const MCGUIComponent *base, QJsonObject const &object);

    virtual Vec2 calculateSize(const MCGUIContext *context) { return size.get(context).get(context); }

};

struct MCGUIComponentCustom : public MCGUIComponent, public MCGUIBaseControl, public MCGUIBaseDataBindingComponent, public MCGUIBaseLayoutComponent, public MCGUIBaseCustomRendererComponent {

    MCGUIComponentCustom(MinecraftJSONParser &parser, const QString &mcNamespace, const QString &name, const MCGUIComponent *base, QJsonObject const &object);

    virtual Vec2 calculateSize(const MCGUIContext *context) { return size.get(context).get(context); }

};

struct MCGUIComponentEditBox : public MCGUIComponent, public MCGUIBaseControl, public MCGUIBaseButtonComponent, public MCGUIBaseDataBindingComponent, public MCGUIBaseInputComponent, public MCGUIBaseLayoutComponent, public MCGUIBaseTextEditComponent {

    MCGUIComponentEditBox(MinecraftJSONParser &parser, const QString &mcNamespace, const QString &name, const MCGUIComponent *base, QJsonObject const &object);

    virtual Vec2 calculateSize(const MCGUIContext *context) { return size.get(context).get(context); }

};

struct MCGUIComponentInputPanel : public MCGUIComponent, public MCGUIBaseInputComponent {

    MCGUIComponentInputPanel(MinecraftJSONParser &parser, const QString &mcNamespace, const QString &name, const MCGUIComponent *base, QJsonObject const &object);

    virtual Vec2 calculateSize(const MCGUIContext *context) { return {0.f, 0.f}; }

};

struct MCGUIComponentLabel : public MCGUIComponent, public MCGUIBaseControl, public MCGUIBaseDataBindingComponent, public MCGUIBaseLayoutComponent, public MCGUIBaseTextComponent {

    MCGUIComponentLabel(MinecraftJSONParser &parser, const QString &mcNamespace, const QString &name, const MCGUIComponent *base, QJsonObject const &object);

    virtual Vec2 calculateSize(const MCGUIContext *context) { return size.get(context).get(context); }

};

struct MCGUIComponentPanel : public MCGUIComponent, public MCGUIBaseControl, public MCGUIBaseDataBindingComponent, public MCGUIBaseLayoutComponent {

    MCGUIComponentPanel(MinecraftJSONParser &parser, const QString &mcNamespace, const QString &name, const MCGUIComponent *base, QJsonObject const &object);

    virtual Vec2 calculateSize(const MCGUIContext *context) { return size.get(context).get(context); }

};

struct MCGUIComponentScreen : public MCGUIComponent, public MCGUIBaseDataBindingComponent, public MCGUIBaseLayoutComponent {

    MCGUIComponentScreen(MinecraftJSONParser &parser, const QString &mcNamespace, const QString &name, const MCGUIComponent *base, QJsonObject const &object);

    virtual Vec2 calculateSize(const MCGUIContext *context) { return context->screenSize; }

};

struct MCGUIComponentTab : public MCGUIComponent, public MCGUIBaseControl, public MCGUIBaseButtonComponent, public MCGUIBaseDataBindingComponent, public MCGUIBaseInputComponent, public MCGUIBaseLayoutComponent, public MCGUIBaseSoundComponent, public MCGUIBaseTabComponent {

    MCGUIComponentTab(MinecraftJSONParser &parser, const QString &mcNamespace, const QString &name, const MCGUIComponent *base, QJsonObject const &object);

    virtual Vec2 calculateSize(const MCGUIContext *context) { return size.get(context).get(context); }

};
