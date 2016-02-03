#include "MinecraftGUIComponent.h"
#include "MinecraftGUIContext.h"
#include "MinecraftJSONParser.h"

MCGUIComponent::MCGUIComponent(const QString &mcNamespace, const QString &name, const QJsonObject &object) :
    mcNamespace(mcNamespace), name(name) {
    ignored.set(object["ignored"], false);
    if (object["variables"].isObject()) {
        QJsonObject o = object["variables"].toObject();
        for (auto it = o.begin(); it != o.end(); it++) {
            variables[it.key()] = *it;
        }
    }
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

MCGUIDataBindingControl::MCGUIDataBindingControl(const MCGUIComponent &component, const QJsonObject &object) {
    //
}

MCGUIComponentButton::MCGUIComponentButton(const QString &mcNamespace, const QString &name, const QJsonObject &object) :
    MCGUIComponent(mcNamespace, name, object),
    MCGUIControl(*this, object), MCGUIButtonControl(*this, object), MCGUIDataBindingControl(*this, object) {
    //
}
