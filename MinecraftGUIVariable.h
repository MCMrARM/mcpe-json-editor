#pragma once

#include <QString>
#include <QJsonValue>

class MCGUIContext;

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
    MCGUIVariable(QJsonValue val) {
        set(val);
    }

    T get(MCGUIContext* context);
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
