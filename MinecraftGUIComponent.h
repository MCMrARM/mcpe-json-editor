#pragma once

#include <QString>
#include <QJsonValue>
#include <QJsonObject>
#include <QList>
#include <QMap>
#include "Vec2.h"
#include "MinecraftGUIVariable.h"
#include "MinecraftGUIBindings.h"
#include "MinecraftGUIButtonId.h"

class MinecraftJSONParser;

struct MCGUIVariableExtendComponent {
    MCGUIComponent* component = nullptr;
    QJsonObject object;
    QString name, mcNamespace;
    MCGUIVariable<QString> extendName;
    QMap<QString, MCGUIComponent*> components;

    MCGUIComponent *get(const MCGUIContext *context);
};

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
    QList<MCGUIVariableExtendComponent> controls;

    MCGUIComponent(const QString &mcNamespace, const QString &name, Type type, const MCGUIComponent *base, const QJsonObject &object);

    static Type getTypeFromString(const QString& type);
    static MCGUIComponent* createComponentOfType(Type type, const QString &mcNamespace, const QString &name, const MCGUIComponent *base, QJsonObject object);

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

    LEFT, RIGHT, UP, DOWN, CENTER

};

struct MCGUIBaseSpriteComponent {

    MCGUIVariable<QString> texture;
    MCGUIAnimatedVariable<Vec2> uv;
    MCGUIVariable<Vec2> uvSize;
    MCGUIVariable<float> alpha;
    MCGUIAnimatedVariable<MCGUIColor> color;
    MCGUIVariable<Vec2> ninesliceSize;
    MCGUIVariable<bool> tiled;
    MCGUIVariable<MCGUIDirection> clipDirection;
    MCGUIAnimatedVariable<float> clipRatio;

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

struct MCGUIComponentButton : public MCGUIComponent, public MCGUIBaseControl, public MCGUIBaseButtonComponent, public MCGUIBaseDataBindingComponent, public MCGUIBaseLayoutComponent, public MCGUIBaseInputComponent, public MCGUIBaseSoundComponent {

    MCGUIComponentButton(const QString &mcNamespace, const QString &name, const MCGUIComponent *base, QJsonObject const &object);

    virtual Vec2 calculateSize(const MCGUIContext *context) { return size.get(context).get(context); }

};

struct MCGUIComponentCarouselLabel : public MCGUIComponent, public MCGUIBaseControl, public MCGUIBaseDataBindingComponent, public MCGUIBaseLayoutComponent, public MCGUIBaseInputComponent, public MCGUIBaseCarouselTextComponent {

    MCGUIComponentCarouselLabel(const QString &mcNamespace, const QString &name, const MCGUIComponent *base, QJsonObject const &object);

    virtual Vec2 calculateSize(const MCGUIContext *context) { return size.get(context).get(context); }

};

struct MCGUIComponentCustom : public MCGUIComponent, public MCGUIBaseControl, public MCGUIBaseDataBindingComponent, public MCGUIBaseLayoutComponent, public MCGUIBaseCustomRendererComponent {

    MCGUIComponentCustom(const QString &mcNamespace, const QString &name, const MCGUIComponent *base, QJsonObject const &object);

    virtual Vec2 calculateSize(const MCGUIContext *context) { return size.get(context).get(context); }

};

struct MCGUIComponentEditBox : public MCGUIComponent, public MCGUIBaseControl, public MCGUIBaseButtonComponent, public MCGUIBaseDataBindingComponent, public MCGUIBaseInputComponent, public MCGUIBaseLayoutComponent, public MCGUIBaseTextEditComponent {

    MCGUIComponentEditBox(const QString &mcNamespace, const QString &name, const MCGUIComponent *base, QJsonObject const &object);

    virtual Vec2 calculateSize(const MCGUIContext *context) { return size.get(context).get(context); }

};

struct MCGUIComponentGrid : public MCGUIComponent, public MCGUIBaseControl, public MCGUIBaseDataBindingComponent, public MCGUIBaseGridComponent, public MCGUIBaseLayoutComponent {

    MCGUIComponentGrid(const QString &mcNamespace, const QString &name, const MCGUIComponent *base, QJsonObject const &object);

    virtual Vec2 calculateSize(const MCGUIContext *context) { return size.get(context).get(context); }

};

struct MCGUIComponentGridItem : public MCGUIComponent, public MCGUIBaseControl, public MCGUIBaseGridItemComponent, public MCGUIBaseLayoutComponent {

    MCGUIComponentGridItem(const QString &mcNamespace, const QString &name, const MCGUIComponent *base, QJsonObject const &object);

    virtual Vec2 calculateSize(const MCGUIContext *context) { return size.get(context).get(context); }

};

struct MCGUIComponentImage : public MCGUIComponent, public MCGUIBaseControl, public MCGUIBaseDataBindingComponent, public MCGUIBaseLayoutComponent, public MCGUIBaseSpriteComponent {

    MCGUIComponentImage(const QString &mcNamespace, const QString &name, const MCGUIComponent *base, QJsonObject const &object);

    virtual Vec2 calculateSize(const MCGUIContext *context) { return size.get(context).get(context); }

};

struct MCGUIComponentInputPanel : public MCGUIComponent, public MCGUIBaseInputComponent {

    MCGUIComponentInputPanel(const QString &mcNamespace, const QString &name, const MCGUIComponent *base, QJsonObject const &object);

    virtual Vec2 calculateSize(const MCGUIContext *context) { return {0.f, 0.f}; }

};

struct MCGUIComponentLabel : public MCGUIComponent, public MCGUIBaseControl, public MCGUIBaseDataBindingComponent, public MCGUIBaseLayoutComponent, public MCGUIBaseTextComponent {

    MCGUIComponentLabel(const QString &mcNamespace, const QString &name, const MCGUIComponent *base, QJsonObject const &object);

    virtual Vec2 calculateSize(const MCGUIContext *context) { return size.get(context).get(context); }

};

struct MCGUIComponentPanel : public MCGUIComponent, public MCGUIBaseControl, public MCGUIBaseDataBindingComponent, public MCGUIBaseLayoutComponent {

    MCGUIComponentPanel(const QString &mcNamespace, const QString &name, const MCGUIComponent *base, QJsonObject const &object);

    virtual Vec2 calculateSize(const MCGUIContext *context) { return size.get(context).get(context); }

};

struct MCGUIComponentScreen : public MCGUIComponent, public MCGUIBaseDataBindingComponent, public MCGUIBaseLayoutComponent {

    MCGUIComponentScreen(const QString &mcNamespace, const QString &name, const MCGUIComponent *base, QJsonObject const &object);

    virtual Vec2 calculateSize(const MCGUIContext *context) { return context->screenSize; }

};

struct MCGUIComponentScrollbar : public MCGUIComponent, public MCGUIBaseControl, public MCGUIBaseInputComponent, public MCGUIBaseLayoutComponent, public MCGUIBaseScrollbarComponent {

    MCGUIComponentScrollbar(const QString &mcNamespace, const QString &name, const MCGUIComponent *base, QJsonObject const &object);

    virtual Vec2 calculateSize(const MCGUIContext *context) { return context->screenSize; }

};

struct MCGUIComponentScrollbarBox : public MCGUIComponent, public MCGUIBaseControl, public MCGUIBaseInputComponent, public MCGUIBaseLayoutComponent {

    MCGUIComponentScrollbarBox(const QString &mcNamespace, const QString &name, const MCGUIComponent *base, QJsonObject const &object);

    virtual Vec2 calculateSize(const MCGUIContext *context) { return context->screenSize; }

};

struct MCGUIComponentTab : public MCGUIComponent, public MCGUIBaseControl, public MCGUIBaseButtonComponent, public MCGUIBaseDataBindingComponent, public MCGUIBaseInputComponent, public MCGUIBaseLayoutComponent, public MCGUIBaseSoundComponent, public MCGUIBaseTabComponent {

    MCGUIComponentTab(const QString &mcNamespace, const QString &name, const MCGUIComponent *base, QJsonObject const &object);

    virtual Vec2 calculateSize(const MCGUIContext *context) { return size.get(context).get(context); }

};

struct MCGUIUnknownComponent : public MCGUIComponent {

    QJsonObject object;

    MCGUIUnknownComponent(const QString &mcNamespace, const QString &name, const MCGUIComponent *base, QJsonObject const &object);

    virtual Vec2 calculateSize(const MCGUIContext *context) { return {0.f, 0.f}; }

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
