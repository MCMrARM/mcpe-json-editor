#include "MinecraftGUIVariable.h"
#include "MinecraftGUIContext.h"
#include "MinecraftGUIComponent.h"
#include "MinecraftGUIButtonId.h"
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
void MCGUIVariable<float>::setVal(QJsonValue val, float def) {
    this->val = (float) val.toDouble(def);
}
template <>
void MCGUIVariable<double>::setVal(QJsonValue val, double def) {
    this->val = val.toDouble(def);
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
void MCGUIVariable<IVec2>::setVal(QJsonValue val, IVec2 def) {
    if (val.isArray()) {
        QJsonArray a = val.toArray();
        this->val = {a[0].toInt(def.x), a[1].toInt(def.y)};
        return;
    }
    this->val = def;
}
template <>
void MCGUIVariable<MCGUIControlVariable>::setVal(QJsonValue val, MCGUIControlVariable def) {
    this->val = val.toString(def.componentName);
}
template <>
void MCGUIVariable<MCGUIComponentVariable>::setVal(QJsonValue val, MCGUIComponentVariable def) {
    this->val = val.toString(def.componentName);
}
template <>
void MCGUIVariable<QList<MCGUIControlVariable>>::setVal(QJsonValue val, QList<MCGUIControlVariable> def) {
    if (val.isArray()) {
        this->val.clear();
        for (QJsonValue el : val.toArray()) {
            this->val.push_back({ el.toString() });
        }
    } else {
        QString s = val.toString("");
        if (s.isEmpty()) {
            this->val = def;
        } else {
            this->val.clear();
            this->val.push_back({ s });
        }
    }
}
template <>
void MCGUIVariable<QList<MCGUIComponentVariable>>::setVal(QJsonValue val, QList<MCGUIComponentVariable> def) {
    if (val.isArray()) {
        this->val.clear();
        for (QJsonValue el : val.toArray()) {
            this->val.push_back({ el.toString() });
        }
    } else {
        QString s = val.toString("");
        if (s.isEmpty()) {
            this->val = def;
        } else {
            this->val.clear();
            this->val.push_back({ s });
        }
    }
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
void MCGUIVariable<MCGUIColor>::setVal(QJsonValue val, MCGUIColor def) {
    if (val.isArray()) {
        QJsonArray arr = val.toArray();
        this->val = {(float) arr[0].toDouble(def.r), (float) arr[1].toDouble(def.g), (float) arr[2].toDouble(def.b), (float) arr[3].toDouble(def.a)};
    } else {
        this->val = def;
    }
}
template <>
void MCGUIVariable<MCGUIButtonId>::setVal(QJsonValue val, MCGUIButtonId def) {
    this->val = MCGUIButtonId::getByString(val.toString(""));
    if (this->val.id == -1)
        this->val = def;
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
            this->val.x.set(a[0]);
            this->val.y.set(a[1]);
            return;
        }
    }
    this->val = def;
}
template <>
void MCGUIVariable<MCGUITextType>::setVal(QJsonValue val, MCGUITextType def) {
    QString v = val.toString("");
    if (v == "ExtendedASCII")
        this->val = MCGUITextType::ExtendedASCII;
    else if (v == "IdentifierChars")
        this->val = MCGUITextType::IdentifierChars;
    else if (v == "NumberChars")
        this->val = MCGUITextType::NumberChars;
    else
        this->val = def;
}
template <>
void MCGUIVariable<MCGUIGridRescalingType>::setVal(QJsonValue val, MCGUIGridRescalingType def) {
    QString v = val.toString("");
    if (v == "horizontal")
        this->val = MCGUIGridRescalingType::HORIZONTAL;
    else if (v == "vertical")
        this->val = MCGUIGridRescalingType::VERTICAL;
    else if (v == "none")
        this->val = MCGUIGridRescalingType::NONE;
    else
        this->val = def;
}
template <>
void MCGUIVariable<MCGUIDirection>::setVal(QJsonValue val, MCGUIDirection def) {
    QString v = val.toString("");
    if (v == "left")
        this->val = MCGUIDirection::LEFT;
    else if (v == "right")
        this->val = MCGUIDirection::RIGHT;
    else if (v == "up")
        this->val = MCGUIDirection::UP;
    else if (v == "down")
        this->val = MCGUIDirection::DOWN;
    else if (v == "center")
        this->val = MCGUIDirection::CENTER;
    else
        this->val = def;
}

MCGUIComponent* MCGUIComponentVariable::get(MCGUIContext *context) {
    if (context == nullptr || !context->components.contains(componentName))
        return nullptr;
    return context->components[componentName];
}

MCGUIComponent* MCGUIControlVariable::get(MCGUIContext *context, MCGUIComponent *ownerComponent) {
    if (ownerComponent == nullptr)
        return nullptr;
    for (MCGUIVariableExtendComponent &child : ownerComponent->controls) {
        if (child.name == componentName)
            return child.get(context);
    }
    for (MCGUIVariableExtendComponent &child : ownerComponent->controls) {
        MCGUIComponent *r = get(context, child.get(context));
        if (r != nullptr)
            return r;
    }
    return nullptr;
}
