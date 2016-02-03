#include "MinecraftGUIVariable.h"
#include "MinecraftGUIContext.h"
#include "MinecraftGUIComponent.h"
#include <QString>
#include <QJsonArray>
#include "Vec2.h"

template <typename T>
T MCGUIVariable<T>::get(MCGUIContext *context) {
    if (context != nullptr && context->variables.contains(variableName)) {
        MCGUIVariable<T> tmpVar (context->variables[variableName]);
        return tmpVar.get(context);
    }
    return val;
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
void MCGUIVariable<QString>::setVal(QJsonValue val, QString def) {
    this->val = val.toString(def);
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
