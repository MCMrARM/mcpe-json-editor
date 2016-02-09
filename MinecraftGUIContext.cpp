#include "MinecraftGUIContext.h"
#include "MinecraftGUIComponent.h"
#include <QDebug>

void MCGUIContext::enter(MCGUIComponent *component) {
    componentStack.push_back(component);
    prevVariables.push_back(QMap<QString, QJsonValue>());
    QMap<QString, QJsonValue>& map = prevVariables.last();
    for (MCGUIComponent::Variables varSet : component->variables) {
        if (varSet.requires.length() > 0) {
            if (variables.count(varSet.requires) <= 0)
                continue;
        }
        auto& vars = varSet.vars;
        for (auto it = vars.begin(); it != vars.end(); it++) {
            qDebug() << "Set variable:" << it.key() << "=" << it->toString("");
            if (variables.contains(it.key()))
                map[it.key()] = variables[it.key()];
            else
                map[it.key()] = QJsonValue();
            variables[it.key()] = *it;
        }
    }
}

void MCGUIContext::exit() {
    QMap<QString, QJsonValue>& map = prevVariables.last();
    for (auto it = map.begin(); it != map.end(); it++) {
        qDebug() << "Undo set variable:" << it.key();
        if (it->isUndefined())
            variables.remove(it.key());
        else
            variables[it.key()] = *it;
    }
    prevVariables.pop_back();
    componentStack.pop_back();
}


Vec2 MCGUIContext::getParentComponentSize() {
    Vec2 componentSize;
    if (componentStack.size() > 0) {
        MCGUIComponent *component = componentStack.last();
        componentStack.pop_back();
        componentSize = componentStack.last()->calculateSize(this);
        componentStack.push_back(component);
    } else {
        componentSize = screenSize;
    }
    return componentSize;
}
