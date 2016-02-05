#pragma once

#include <QString>
#include <QJsonValue>
#include "MinecraftGUIContext.h"

template <typename T>
class MCGUIVariable {

private:
    void setVal(QJsonValue val, T def);

public:
    T val;
    QString variableName;

    MCGUIVariable() {
        //
    }
    MCGUIVariable(QJsonValue val, T def) {
        set(val, def);
    }
    T get(const MCGUIContext *context) {
        if (context != nullptr && context->variables.contains(variableName)) {
            MCGUIVariable<T> tmpVar (context->variables[variableName], val);
            return tmpVar.get(context);
        }
        return val;
    }
    void setAsVariable(QString varName) {
        this->variableName = varName;
    }

    void set(T val) {
        this->val = val;
    }
    void set(QJsonValue val, T def) {
        if (val.isString()) {
            variableName = val.toString();
        }
        setVal(val, def);
    }

};

template <typename T>
class MCGUIAnimatedVariable : public MCGUIVariable<T> {

public:
    MCGUIAnimatedVariable() : MCGUIVariable<T>() {
        //
    }
    MCGUIAnimatedVariable(QJsonValue val) : MCGUIVariable<T>(val) {
    }

};
