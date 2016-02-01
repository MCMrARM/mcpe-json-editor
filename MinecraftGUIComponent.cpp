#include "MinecraftGUIComponent.h"
#include "MinecraftJSONParser.h"

MinecraftGUIComponent::MinecraftGUIComponent(const QString &mcNamespace, const QString &name, const QString &base) :
    mcNamespace(mcNamespace), name(name), base(base) {

}

MinecraftGUIComponent::Type MinecraftGUIComponent::getTypeFromString(const QString &type) {
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

MinecraftGUIControl::MinecraftGUIControl(const QJsonObject &object) {
    visible = object["visible"].toBool(true);
    layer = object["layer"].toInt(0);
    clipsChildren = object["clips_children"].toBool(false);
    clipOffset = MinecraftJSONParser::getVec2(object["clip_offset"], {0.f, 0.f});
    allowClipping = object["allow_clipping"].toBool(true);
    if (object["property_bag"].isObject())
        propertyBag = object["property_bag"].toObject();
}


MinecraftGUIComponentButton::MinecraftGUIComponentButton(const QString &mcNamespace, const QString &name, const QString &base, const QJsonObject &object) :
    MinecraftGUIComponent(mcNamespace, name, base),
    MinecraftGUIControl(object) {
    //
}
