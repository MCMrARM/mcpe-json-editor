#pragma once

#include <QString>
#include <QJsonValue>
#include <QJsonObject>
#include <QList>
#include <QMap>
#include "Vec2.h"
#include "Vec4.h"
#include "MinecraftGUIVariable.h"
#include "MinecraftGUIBindings.h"
#include "MinecraftGUIButtonId.h"

class MinecraftJSONParser;

struct MCGUIVariableExtendComponent {
    MCGUIComponent* component = nullptr;
    QJsonObject object;
    QString mcNamespace;
    MCGUIVariable<QString> name;
    QMap<QString, MCGUIComponent*> components;

    MCGUIComponent *get(MCGUIContext *context);
};

struct MCGUIComponent {

    enum class Type {
        BUTTON, CAROUSEL_LABEL, CUSTOM, EDIT_BOX, GRID, GRID_ITEM, IMAGE, INPUT_PANEL, LABEL, PANEL, SCREEN, SCROLLBAR, SCROLLBAR_BOX, TAB, UNKNOWN
    };

    struct Variables {

        QString requires;
        QMap<QString, QJsonValue> vars;

        Variables() {
            //
        }

        Variables(QJsonObject const &o);

    };


    QString mcNamespace;
    QString name;
    const MCGUIComponent* base = nullptr;
    Type type;
    MCGUIVariable<bool> ignored = false;
    QList<Variables> variables;
    QList<MCGUIDataBinding> bindings;
    QList<MCGUIVariableExtendComponent> controls;

    MCGUIComponent(const QString &mcNamespace, const QString &name, Type type, bool parseDataBindings, const MCGUIComponent *base, const QJsonObject &object);

    static Type getTypeFromString(const QString& type);
    static MCGUIComponent* createComponentOfType(Type type, const QString &mcNamespace, const QString &name, const MCGUIComponent *base, QJsonObject object);

    virtual Vec2 getPos(MCGUIContext *context) = 0;
    virtual Vec2 calculateSize(MCGUIContext *context) = 0;

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

enum class MCGUIAnchorPoint {

    TOP_LEFT,    TOP_MIDDLE,    TOP_RIGHT,
    LEFT_MIDDLE, CENTER,        RIGHT_MIDDLE,
    BOTTOM_LEFT, BOTTOM_MIDDLE, BOTTOM_RIGHT

};

Vec2 MCGUIGetAnchorPoint(Vec2 size, MCGUIAnchorPoint point);

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

    float get(MCGUIContext *context);
    float get(Vec2 parentSize);

    void set(const QString &str);
    void set(const QJsonValue &obj);

};

struct MCGUILayoutOffset {

    MCGUILayoutAxis x, y;

    MCGUILayoutOffset() : x(MCGUILayoutAxis::Axis::X), y(MCGUILayoutAxis::Axis::Y) {
        //
    }

    Vec2 get(Vec2 parentSize) {
        return { x.get(parentSize), y.get(parentSize) };
    }
    Vec2 get(MCGUIContext *context) {
       return get(context->getParentComponentSize());
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

enum class MCGUIFontSize {

    NORMAL, SMALL

};

struct MCGUIBaseTextComponent {

    MCGUIVariable<QString> text;
    MCGUIVariable<MCGUIAnchorPoint> alignment = MCGUIAnchorPoint::CENTER;
    MCGUIVariable<MCGUIColor> color = MCGUIColor::WHITE;
    MCGUIVariable<float> alpha = 1.f;
    MCGUIVariable<bool> shadow = false;
    MCGUIVariable<MCGUIFontSize> fontSize;
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
    MCGUIVariable<MCGUIFontSize> fontSize;
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

enum class MCGUIGridRescalingType {

    HORIZONTAL, VERTICAL, NONE

};

struct MCGUIBaseGridComponent {

    MCGUIVariable<IVec2> gridDimensions;
    MCGUIVariable<QString> gridDimensionBinding;
    MCGUIVariable<QString> collectionName;
    MCGUIVariable<MCGUIGridRescalingType> gridRescalingType;
    MCGUIVariable<int> maximumGridItems = 0;
    MCGUIVariable<MCGUIComponentVariable> gridItemTemplate;

    MCGUIBaseGridComponent(const MCGUIComponent &component, const MCGUIComponent *base, const QJsonObject &object);

};

struct MCGUIBaseGridItemComponent {

    MCGUIVariable<IVec2> gridPosition;

    MCGUIBaseGridItemComponent(const MCGUIComponent &component, const MCGUIComponent *base, const QJsonObject &object);

};

enum class MCGUIDirection {

    NONE, LEFT, RIGHT, UP, DOWN, CENTER

};

struct MCGUIBaseSpriteComponent {

    MCGUIVariable<QString> texture;
    MCGUIAnimatedVariable<Vec2> uv;
    MCGUIVariable<Vec2> uvSize;
    MCGUIVariable<float> alpha = 1.f;
    MCGUIAnimatedVariable<MCGUIColor> color = MCGUIColor::WHITE;
    MCGUIVariable<Vec4> ninesliceSize;
    MCGUIVariable<bool> tiled = false;
    MCGUIVariable<MCGUIDirection> clipDirection = MCGUIDirection::NONE;
    MCGUIAnimatedVariable<float> clipRatio = 1.f;

    MCGUIBaseSpriteComponent(const MCGUIComponent &component, const MCGUIComponent *base, const QJsonObject &object);

};

struct MCGUIBaseScrollbarComponent {

    MCGUIVariable<MCGUIButtonId> scrollbarBoxTrackButton;
    MCGUIVariable<MCGUIButtonId> scrollbarTouchButton;
    MCGUIVariable<float> dampening = 1.f;
    MCGUIVariable<float> scrollSpeed = 1.f;
    MCGUIVariable<MCGUIControlVariable> scrollbarBox;

    MCGUIBaseScrollbarComponent(const MCGUIComponent &component, const MCGUIComponent *base, const QJsonObject &object);

};

struct MCGUIBaseTabComponent {

    MCGUIVariable<int> tabGroup = 0;
    MCGUIVariable<int> tabIndex = 0;
    MCGUIVariable<MCGUIComponentVariable> tabContent;

    MCGUIBaseTabComponent(const MCGUIComponent &component, const MCGUIComponent *base, const QJsonObject &object);

};

struct MCGUILayoutComponent : public MCGUIComponent, public MCGUIBaseLayoutComponent {

    MCGUILayoutComponent(const QString &mcNamespace, const QString &name, Type type, bool parseDataBindings, const MCGUIComponent *base, const QJsonObject &object) :
        MCGUIComponent(mcNamespace, name, type, parseDataBindings, base, object), MCGUIBaseLayoutComponent(*this, base, object) {
        //
    }

    virtual Vec2 getPos(MCGUIContext *context);
    virtual Vec2 calculateSize(MCGUIContext *context) {
        return size.get(context).get(context);
    }

};

struct MCGUIComponentButton : public MCGUILayoutComponent, public MCGUIBaseControl, public MCGUIBaseButtonComponent, public MCGUIBaseInputComponent, public MCGUIBaseSoundComponent {

    MCGUIComponentButton(const QString &mcNamespace, const QString &name, const MCGUIComponent *base, QJsonObject const &object);

};

struct MCGUIComponentCarouselLabel : public MCGUILayoutComponent, public MCGUIBaseControl, public MCGUIBaseInputComponent, public MCGUIBaseCarouselTextComponent {

    MCGUIComponentCarouselLabel(const QString &mcNamespace, const QString &name, const MCGUIComponent *base, QJsonObject const &object);

};

struct MCGUIComponentCustom : public MCGUILayoutComponent, public MCGUIBaseControl, public MCGUIBaseCustomRendererComponent {

    MCGUIComponentCustom(const QString &mcNamespace, const QString &name, const MCGUIComponent *base, QJsonObject const &object);

};

struct MCGUIComponentEditBox : public MCGUILayoutComponent, public MCGUIBaseControl, public MCGUIBaseButtonComponent, public MCGUIBaseInputComponent, public MCGUIBaseTextEditComponent {

    MCGUIComponentEditBox(const QString &mcNamespace, const QString &name, const MCGUIComponent *base, QJsonObject const &object);

};

struct MCGUIComponentGrid : public MCGUILayoutComponent, public MCGUIBaseControl, public MCGUIBaseGridComponent {

    MCGUIComponentGrid(const QString &mcNamespace, const QString &name, const MCGUIComponent *base, QJsonObject const &object);

};

struct MCGUIComponentGridItem : public MCGUILayoutComponent, public MCGUIBaseControl, public MCGUIBaseGridItemComponent {

    MCGUIComponentGridItem(const QString &mcNamespace, const QString &name, const MCGUIComponent *base, QJsonObject const &object);


};

struct MCGUIComponentImage : public MCGUILayoutComponent, public MCGUIBaseControl, public MCGUIBaseSpriteComponent {

    MCGUIComponentImage(const QString &mcNamespace, const QString &name, const MCGUIComponent *base, QJsonObject const &object);

};

struct MCGUIComponentInputPanel : public MCGUILayoutComponent, public MCGUIBaseControl, public MCGUIBaseInputComponent {

    MCGUIComponentInputPanel(const QString &mcNamespace, const QString &name, const MCGUIComponent *base, QJsonObject const &object);

};

struct MCGUIComponentLabel : public MCGUILayoutComponent, public MCGUIBaseControl, public MCGUIBaseTextComponent {

    MCGUIComponentLabel(const QString &mcNamespace, const QString &name, const MCGUIComponent *base, QJsonObject const &object);

};

struct MCGUIComponentPanel : public MCGUILayoutComponent, public MCGUIBaseControl {

    MCGUIComponentPanel(const QString &mcNamespace, const QString &name, const MCGUIComponent *base, QJsonObject const &object);

};

struct MCGUIComponentScreen : public MCGUIComponent {

    MCGUIComponentScreen(const QString &mcNamespace, const QString &name, const MCGUIComponent *base, QJsonObject const &object);

    virtual Vec2 getPos(MCGUIContext *context) { return {0.f, 0.f}; }
    virtual Vec2 calculateSize(MCGUIContext *context) { return context->screenSize; }

};

struct MCGUIComponentScrollbar : public MCGUILayoutComponent, public MCGUIBaseControl, public MCGUIBaseInputComponent, public MCGUIBaseScrollbarComponent {

    MCGUIComponentScrollbar(const QString &mcNamespace, const QString &name, const MCGUIComponent *base, QJsonObject const &object);

};

struct MCGUIComponentScrollbarBox : public MCGUILayoutComponent, public MCGUIBaseControl, public MCGUIBaseInputComponent {

    MCGUIComponentScrollbarBox(const QString &mcNamespace, const QString &name, const MCGUIComponent *base, QJsonObject const &object);
};

struct MCGUIComponentTab : public MCGUILayoutComponent, public MCGUIBaseControl, public MCGUIBaseButtonComponent, public MCGUIBaseInputComponent, public MCGUIBaseSoundComponent, public MCGUIBaseTabComponent {

    MCGUIComponentTab(const QString &mcNamespace, const QString &name, const MCGUIComponent *base, QJsonObject const &object);

};

struct MCGUIUnknownComponent : public MCGUIComponent {

    QJsonObject object;

    MCGUIUnknownComponent(const QString &mcNamespace, const QString &name, const MCGUIComponent *base, QJsonObject const &object);

    virtual Vec2 getPos(MCGUIContext *context) { return {0.f, 0.f}; }
    virtual Vec2 calculateSize(MCGUIContext *context) { return {0.f, 0.f}; }

};

// Cast & copy macros
#define MCGUICastToType(el, toType) ( \
    el->type == MCGUIComponent::Type::BUTTON ? ((toType*)(MCGUIComponentButton*) el) : \
    el->type == MCGUIComponent::Type::CAROUSEL_LABEL ? ((toType*)(MCGUIComponentCarouselLabel*) el) : \
    el->type == MCGUIComponent::Type::CUSTOM ? ((toType*)(MCGUIComponentCustom*) el) : \
    el->type == MCGUIComponent::Type::EDIT_BOX ? ((toType*)(MCGUIComponentEditBox*) el) : \
    el->type == MCGUIComponent::Type::GRID ? ((toType*)(MCGUIComponentGrid*) el) : \
    el->type == MCGUIComponent::Type::GRID_ITEM ? ((toType*)(MCGUIComponentGridItem*) el) : \
    el->type == MCGUIComponent::Type::IMAGE ? ((toType*)(MCGUIComponentImage*) el) : \
    el->type == MCGUIComponent::Type::INPUT_PANEL ? ((toType*)(MCGUIComponentInputPanel*) el) : \
    el->type == MCGUIComponent::Type::LABEL ? ((toType*)(MCGUIComponentLabel*) el) : \
    el->type == MCGUIComponent::Type::PANEL ? ((toType*)(MCGUIComponentPanel*) el) : \
    el->type == MCGUIComponent::Type::SCREEN ? ((toType*)(MCGUIComponentScreen*) el) : \
    el->type == MCGUIComponent::Type::SCROLLBAR ? ((toType*)(MCGUIComponentScrollbar*) el) : \
    el->type == MCGUIComponent::Type::SCROLLBAR_BOX ? ((toType*)(MCGUIComponentScrollbarBox*) el) : \
    el->type == MCGUIComponent::Type::TAB ? ((toType*)(MCGUIComponentTab*) el) : \
    (toType*) nullptr \
    )
#define MCGUIIsOfBaseType_Control(el) (el->type == MCGUIComponent::Type::BUTTON || \
    el->type == MCGUIComponent::Type::CAROUSEL_LABEL || \
    el->type == MCGUIComponent::Type::CUSTOM || \
    el->type == MCGUIComponent::Type::EDIT_BOX || \
    el->type == MCGUIComponent::Type::GRID || \
    el->type == MCGUIComponent::Type::GRID_ITEM || \
    el->type == MCGUIComponent::Type::IMAGE || \
    el->type == MCGUIComponent::Type::LABEL || \
    el->type == MCGUIComponent::Type::PANEL || \
    el->type == MCGUIComponent::Type::SCROLLBAR || \
    el->type == MCGUIComponent::Type::SCROLLBAR_BOX || \
    el->type == MCGUIComponent::Type::TAB)
#define MCGUIIsOfBaseType_ButtonComponent(el) (el->type == MCGUIComponent::Type::BUTTON || \
    el->type == MCGUIComponent::Type::EDIT_BOX)
#define MCGUIIsOfBaseType_DataBindingComponent(el) (el->type == MCGUIComponent::Type::BUTTON || \
    el->type == MCGUIComponent::Type::CAROUSEL_LABEL || \
    el->type == MCGUIComponent::Type::CUSTOM || \
    el->type == MCGUIComponent::Type::EDIT_BOX || \
    el->type == MCGUIComponent::Type::GRID || \
    el->type == MCGUIComponent::Type::IMAGE || \
    el->type == MCGUIComponent::Type::LABEL || \
    el->type == MCGUIComponent::Type::PANEL || \
    el->type == MCGUIComponent::Type::SCREEN || \
    el->type == MCGUIComponent::Type::TAB)
#define MCGUIIsOfBaseType_LayoutComponent(el) (el->type == MCGUIComponent::Type::BUTTON || \
    el->type == MCGUIComponent::Type::CAROUSEL_LABEL || \
    el->type == MCGUIComponent::Type::CUSTOM || \
    el->type == MCGUIComponent::Type::EDIT_BOX || \
    el->type == MCGUIComponent::Type::GRID || \
    el->type == MCGUIComponent::Type::GRID_ITEM || \
    el->type == MCGUIComponent::Type::IMAGE || \
    el->type == MCGUIComponent::Type::INPUT_PANEL || \
    el->type == MCGUIComponent::Type::LABEL || \
    el->type == MCGUIComponent::Type::PANEL || \
    el->type == MCGUIComponent::Type::SCROLLBAR || \
    el->type == MCGUIComponent::Type::SCROLLBAR_BOX || \
    el->type == MCGUIComponent::Type::TAB)
#define MCGUIIsOfBaseType_InputComponent(el) (el->type == MCGUIComponent::Type::BUTTON || \
    el->type == MCGUIComponent::Type::CAROUSEL_LABEL || \
    el->type == MCGUIComponent::Type::EDIT_BOX || \
    el->type == MCGUIComponent::Type::INPUT_PANEL || \
    el->type == MCGUIComponent::Type::SCREEN || \
    el->type == MCGUIComponent::Type::SCROLLBAR || \
    el->type == MCGUIComponent::Type::SCROLLBAR_BOX || \
    el->type == MCGUIComponent::Type::TAB)
#define MCGUIIsOfBaseType_SoundComponent(el) (el->type == MCGUIComponent::Type::BUTTON || \
    el->type == MCGUIComponent::Type::TAB)
#define MCGUIIsOfBaseType_TextComponent(el) (el->type == MCGUIComponent::Type::LABEL)
#define MCGUIIsOfBaseType_CarouselTextComponent(el) (el->type == MCGUIComponent::Type::CAROUSEL_LABEL)
#define MCGUIIsOfBaseType_CustomRendererComponent(el) (el->type == MCGUIComponent::Type::CUSTOM)
#define MCGUIIsOfBaseType_TextEditComponent(el) (el->type == MCGUIComponent::Type::EDIT_BOX)
#define MCGUIIsOfBaseType_GridComponent(el) (el->type == MCGUIComponent::Type::GRID)
#define MCGUIIsOfBaseType_GridItemComponent(el) (el->type == MCGUIComponent::Type::GRID_ITEM)
#define MCGUIIsOfBaseType_SpriteComponent(el) (el->type == MCGUIComponent::Type::IMAGE)
#define MCGUIIsOfBaseType_ScrollbarComponent(el) (el->type == MCGUIComponent::Type::SCROLLBAR)
#define MCGUIIsOfBaseType_TabComponent(el) (el->type == MCGUIComponent::Type::TAB)
#define MCGUIIsOfBaseType(el, type) MCGUIIsOfBaseType_##type(el)
#define MCGUICopyBaseProperties(base, type) \
    if (base != nullptr && MCGUIIsOfBaseType(base, type)) \
        *this = *(MCGUICastToType(base, MCGUIBase##type));
