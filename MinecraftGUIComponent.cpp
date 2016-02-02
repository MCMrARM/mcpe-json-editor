#include "MinecraftGUIComponent.h"
#include "MinecraftJSONParser.h"

MCGUIComponent::MCGUIComponent(const QString &mcNamespace, const QString &name, const QJsonObject &object) :
    mcNamespace(mcNamespace), name(name) {
    ignored.set(object["ignored"], false);
}

MCGUIComponent::Type MCGUIComponent::getTypeFromString(const QString &type) {
    if (type == "button")
        return Type::BUTTON;
    if (type == "carousel_label")
        return Type::CAROUSEL_LABEL;
    if (type == "custom")
        return Type::CUSTOM;
    if (type == "edit_box")
        return Type::EDIT_BOX;
    if (type == "grid")
        return Type::GRID;
    if (type == "grid_item")
        return Type::GRID_ITEM;
    if (type == "image")
        return Type::IMAGE;
    if (type == "input_panel")
        return Type::INPUT_PANEL;
    if (type == "label")
        return Type::LABEL;
    if (type == "panel")
        return Type::PANEL;
    if (type == "screen")
        return Type::SCREEN;
    if (type == "scrollbar")
        return Type::SCROLLBAR;
    if (type == "scrollbar_box")
        return Type::SCROLLBAR_BOX;
    if (type == "tab")
        return Type::TAB;
    return Type::UNKNOWN;
}

template <typename T>
T MCGUIVariable<T>::get(MCGUIContext *context) {
    if (context != nullptr && context->variables.contains(variableName)) {
        MCGUIVariable<T> tmpVar (context->variables[variableName]);
        return tmpVar.get(context);
    }
    return val;
}

template <typename T>
void MCGUIVariable<T>::set(QJsonValue val, T def) {
    if (val.isString()) {
        variableName = val.toString();
    }
    setVal(val, def);
}

template <>
void MCGUIVariable<bool>::setVal(QJsonValue val, bool def) {
    this->val = val.toBool(def);
}
template <>
void MCGUIVariable<int>::setVal(QJsonValue val, int def) {
    this->val = val.toInt(def);
}
template <>
void MCGUIVariable<Vec2>::setVal(QJsonValue val, Vec2 def) {
    if (val.isArray()) {
        QJsonArray a = val.toArray();
        this->val = {(float) a[0].toDouble(def.x), (float) a[1].toDouble(def.y)};
        return;
    }
    this->val = def;
}
template <>
void MCGUIVariable<MCGUIComponentVariable>::setVal(QJsonValue val, MCGUIComponentVariable def) {
    this->val = val.toString(def.componentName);
}
template <>
void MCGUIVariable<QJsonObject>::setVal(QJsonValue val, QJsonObject def) {
    this->val = val.toObject(def);
}

MCGUIComponent* MCGUIComponentVariable::get(MCGUIContext *context) {
    if (context == nullptr || !context->components.contains(componentName))
        return nullptr;
    return context->components[componentName];
}

MCGUIComponent* MCGUIComponent::createComponentOfType(Type type, const QString &mcNamespace, const QString &name, const QJsonObject &object) {
    switch (type) {
    case Type::BUTTON:
        return new MCGUIComponentButton(mcNamespace, name, object);
    }
    return nullptr;
}

MCGUIControl::MCGUIControl(const MCGUIComponent &component, const QJsonObject &object) {
    visible.set(object["visible"], true);
    layer.set(object["layer"], 0);
    clipsChildren.set(object["clips_children"], false);
    clipOffset.set(object["clip_offset"], {0.f, 0.f});
    allowClipping.set(object["allow_clipping"], true);
    propertyBag.set(object["property_bag"], QJsonObject());
}

MCGUIButtonControl::MCGUIButtonControl(const MCGUIComponent &component, const QJsonObject &object) {
    defaultControl.set(object["default_control"], {""});
    hoverControl.set(object["hover_control"], {""});
    pressedControl.set(object["pressed_control"], {""});
}

MCGUIComponentButton::MCGUIComponentButton(const QString &mcNamespace, const QString &name, const QJsonObject &object) :
    MCGUIComponent(mcNamespace, name, object),
    MCGUIControl(*this, object), MCGUIButtonControl(*this, object) {
    //
}
