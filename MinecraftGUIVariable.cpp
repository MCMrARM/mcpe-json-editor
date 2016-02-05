#include "MinecraftGUIVariable.h"
#include "MinecraftGUIContext.h"
#include "MinecraftGUIComponent.h"
#include <QString>
#include <QJsonArray>
#include "Vec2.h"

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
void MCGUIVariable<QString>::setVal(QJsonValue val, QString def) {
    this->val = val.toString(def);
}
template <>
void MCGUIVariable<QJsonObject>::setVal(QJsonValue val, QJsonObject def) {
    this->val = val.toObject(def);
}
template <>
void MCGUIVariable<MCGUIAnchorPoint>::setVal(QJsonValue val, MCGUIAnchorPoint def) {
    QString v = val.toString("");
    if (v == "top_left")
        this->val = MCGUIAnchorPoint::TOP_LEFT;
    else if (v == "top_middle")
        this->val = MCGUIAnchorPoint::TOP_MIDDLE;
    else if (v == "top_right")
        this->val = MCGUIAnchorPoint::TOP_RIGHT;
    else if (v == "left_middle")
        this->val = MCGUIAnchorPoint::LEFT_MIDDLE;
    else if (v == "center")
        this->val = MCGUIAnchorPoint::CENTER;
    else if (v == "right_middle")
        this->val = MCGUIAnchorPoint::RIGHT_MIDDLE;
    else if (v == "bottom_left")
        this->val = MCGUIAnchorPoint::BOTTOM_LEFT;
    else if (v == "bottom_middle")
        this->val = MCGUIAnchorPoint::BOTTOM_MIDDLE;
    else if (v == "bottom_right")
        this->val = MCGUIAnchorPoint::BOTTOM_RIGHT;
    else
        this->val = def;
}
template <>
void MCGUIVariable<MCGUIDraggable>::setVal(QJsonValue val, MCGUIDraggable def) {
    QString v = val.toString("");
    if (v == "not_draggable")
        this->val = MCGUIDraggable::NOT_DRAGGABLE;
    else if (v == "horizontal")
        this->val = MCGUIDraggable::HORIZONTAL;
    else if (v == "vertical")
        this->val = MCGUIDraggable::VERTICAL;
    else if (v == "both")
        this->val = MCGUIDraggable::BOTH;
    else
        this->val = def;
}
template <>
void MCGUIVariable<MCGUILayoutOffset>::setVal(QJsonValue val, MCGUILayoutOffset def) {
    if (val.isArray()) {
        QJsonArray a = val.toArray();
        if (a.size() >= 2) {
            this->val.x.set(a[0].toString(""));
            this->val.y.set(a[1].toString(""));
            return;
        }
    }
    this->val = def;
}

MCGUIComponent* MCGUIComponentVariable::get(MCGUIContext *context) {
    if (context == nullptr || !context->components.contains(componentName))
        return nullptr;
    return context->components[componentName];
}
