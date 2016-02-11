#pragma once

#include <QString>
#include <QMap>
#include <QJsonValue>
#include "Vec2.h"

struct MCGUIComponent;
struct MCGUIDataBinding;

class MCGUIContext {

public:
    struct CollectionBinding {
        int i = 0;
        QList<QJsonValue> vals;
    };

    QMap<QString, QJsonValue> variables;
    QList<QMap<QString, QJsonValue>> prevVariables;
    QMap<QString, QJsonValue> globalBindings;
    QMap<QString, CollectionBinding> collectionBindings;
    QMap<QString, MCGUIComponent*>& components;
    QList<MCGUIComponent*> componentStack;
    Vec2 screenSize;

    MCGUIContext(QMap<QString, MCGUIComponent*>& components) : components(components) {
        //
    }

    void enter(MCGUIComponent* component);
    void exit();

    Vec2 getParentComponentSize();

    QJsonValue resolveBinding(const MCGUIDataBinding &binding, bool incrementCollectionBindings = true);
    void resetCollectionBindingsNum() {
        for (CollectionBinding &b : collectionBindings)
            b.i = 0;
    }

};
