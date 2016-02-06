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
    MCGUIVariable(T val) {
        set(val);
    }

    MCGUIVariable(QJsonValue val, T def) {
        setJSON(val, def);
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
    void setJSON(QJsonValue val) {
        setJSON(val, this->val);
    }
    void setJSON(QJsonValue val, T def) {
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

struct MCGUIComponentVariable {

    QString componentName;

    MCGUIComponent* get(MCGUIContext *context);
    void set(QString name) {
        componentName = name;
    }
    MCGUIComponentVariable& operator=(QString name) {
        componentName = name;
        return *this;
    }

};

struct MCGUIControlVariable {

    QString componentName;

    MCGUIComponent* get(MCGUIComponent *ownerComponent);
    void set(QString name) {
        componentName = name;
    }
    MCGUIControlVariable& operator=(QString name) {
        componentName = name;
        return *this;
    }

};
