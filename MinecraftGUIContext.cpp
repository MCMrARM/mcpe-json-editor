#include "MinecraftGUIContext.h"
#include "MinecraftGUIComponent.h"

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
        if (it->isUndefined())
            variables.remove(it.key());
        else
            variables[it.key()] = *it;
    }
    prevVariables.pop_back();
    componentStack.pop_back();
}
