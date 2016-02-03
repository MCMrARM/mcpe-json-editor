#include "MinecraftGUIContext.h"
#include "MinecraftGUIComponent.h"

void MCGUIContext::enter(MCGUIComponent *component) {
    prevVariables.push_back(QMap<QString, QJsonValue>());
    QMap<QString, QJsonValue>& map = prevVariables.last();
    for (auto it = component->variables.begin(); it != component->variables.end(); it++) {
        if (variables.contains(it.key()))
            map[it.key()] = variables[it.key()];
        else
            map[it.key()] = QJsonValue();
        variables[it.key()] = *it;
    }
}

void MCGUIContext::exit() {
    QMap<QString, QJsonValue>& map = prevVariables.last();
    for (auto it = map.begin(); it != map.end(); it++) {
        if (it->isUndefined())
            variables.remove(it.key());
        else
            variables[it.key()] = *it;
    }
    prevVariables.pop_back();
}
