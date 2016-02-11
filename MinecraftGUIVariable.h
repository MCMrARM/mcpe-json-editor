#pragma once

#include <QString>
#include <QList>
#include <QJsonValue>
#include <QJsonObject>
#include "MinecraftGUIContext.h"

class MCGUIDataBinding;

const MCGUIDataBinding *_MCGUIVariable_getBinding(const QString &name, const QString &variableName, const QList<MCGUIDataBinding> &dataBindings);

template <typename T>
class MCGUIVariable {

private:
    void setVal(QJsonValue val, T def);

public:
    T val;
    QString variableName;
    const MCGUIDataBinding *dataBinding = nullptr;

    MCGUIVariable() {
        //
    }
    MCGUIVariable(T val) {
        set(val);
    }

    MCGUIVariable(QJsonValue val, T def) {
        setJSON(val, def);
    }
    T get(MCGUIContext *context) const {
        T retVal = val;
        if (context != nullptr && context->variables.contains(variableName)) {
            MCGUIVariable<T> tmpVar (context->variables[variableName], val);
            retVal = tmpVar.get(context);
        }
        if (dataBinding != nullptr && context != nullptr) {
            QJsonValue v = context->resolveBinding(*dataBinding);
            MCGUIVariable<T> tmpVar (v, retVal);
            return tmpVar.get(context);
        }
        return retVal;
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
    void setJSON(QJsonObject obj, QString name, T def, const QList<MCGUIDataBinding> &dataBindings) {
        setJSON(obj[name], def);
        dataBinding = _MCGUIVariable_getBinding(name, variableName, dataBindings);
    }
    void setJSON(QJsonObject obj, QString name, const QList<MCGUIDataBinding> &dataBindings) {
        setJSON(obj, name, val, dataBindings);
    }


};

template <typename T>
class MCGUIAnimatedVariable : public MCGUIVariable<T> {

public:
    MCGUIAnimatedVariable() : MCGUIVariable<T>() {
        //
    }
    MCGUIAnimatedVariable(T val) : MCGUIVariable<T>(val) {
    }
    MCGUIAnimatedVariable(QJsonValue val, T def) : MCGUIVariable<T>(val, def) {
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

    MCGUIComponent* get(MCGUIContext *context, MCGUIComponent *ownerComponent);
    void set(QString name) {
        componentName = name;
    }
    MCGUIControlVariable& operator=(QString name) {
        componentName = name;
        return *this;
    }

};
