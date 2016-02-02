#include "MinecraftJSONParser.h"
#include "MinecraftGUIComponent.h"

#include <QFile>
#include <QDirIterator>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

MinecraftJSONParser::MinecraftJSONParser()
{

}

void MinecraftJSONParser::loadJSONFile(const QString &fileName) {
    qDebug() << "Loading JSON: " << fileName;
    QFile file (fileName);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray rawData = file.readAll();
    {
        bool inString = false;
        bool hasSingleLineComment = false;
        for (int i = 0; i < rawData.length() - 1; i++) {
            if (rawData[i] == '\n') {
                hasSingleLineComment = false;
                continue;
            }
            if (hasSingleLineComment)
                rawData[i] = ' ';
            if (rawData[i] == '"') {
                if (inString) {
                    if (rawData[i - 1] == '\\')
                        continue;
                    inString = false;
                } else {
                    inString = true;
                }
            }
            if (rawData[i] == '/' && !inString) {
                if (rawData[i + 1] == '/') {
                    hasSingleLineComment = true;
                    rawData[i] = ' ';
                    rawData[i + 1] = ' ';
                    i++;
                    continue;
                }
                if (rawData[i + 1] == '*') {
                    rawData[i] = ' ';
                    rawData[i + 1] = ' ';
                    i+=2;
                    for (; i < rawData.length() - 1; i++) {
                        if (rawData[i] == '*' && rawData[i + 1] == '/') {
                            rawData[i] = ' ';
                            rawData[i + 1] = ' ';
                            break;
                        }
                        rawData[i] = ' ';
                    }
                    continue;
                }
            }
        }
    }
    file.close();
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(rawData, &error);
    if (error.error != QJsonParseError::NoError) {
        qDebug() << "Failed to parse JSON:" << error.errorString();
        return;
    }
    QJsonObject json = doc.object();
    QString mcNamespace = json["namespace"].toString("");
    for (auto it = json.begin(); it != json.end(); it++) {
        if (it.key() == "ui_defs") {
            qDebug() << "JSON file list found.";
            for (auto val : it.value().toArray()) {
                qDebug() << "Loading:" << val.toString();
                loadJSONFile(val.toString());
            }
        } else if (it.key() == "namespace") {
            continue;
        } else {
            parseComponent(it.key(), mcNamespace, it->toObject());
        }
    }
}

void MinecraftJSONParser::parseComponent(QString name, const QString &mcNamespace, const QJsonObject &object) {
    qDebug() << "Parsing component:" << mcNamespace << "." << name;

    MCGUIComponent::Type type = MCGUIComponent::getTypeFromString(object["type"].toString(""));

    MCGUIComponent* component = nullptr;

    int i = name.indexOf('@');
    if (i >= 0) {
        QString extendNamespace = mcNamespace;
        QString extends = name.mid(i + 1);
        name = name.mid(0, i);

        i = extends.indexOf(".");
        if (i >= 0) {
            extendNamespace = extends.mid(0, i);
            extends = extends.mid(i + 1);
        }

        QString nameWithNamespace = mcNamespace + "." + name;

        qDebug() << "Component extends:" << extendNamespace << "." << extends;
        requireComponent(extendNamespace + "." + extends, [nameWithNamespace, extendNamespace, extends](MCGUIComponent* component) {
            qDebug() << "Continuing parse of component" << nameWithNamespace << ":" << (extendNamespace+"."+extends);
        });
    } else {
        qDebug() << "Component type:" << (int) type << "-" << object["type"].toString("");

        QString nameWithNamespace = mcNamespace + "." + name;

        component = MCGUIComponent::createComponentOfType(type, mcNamespace, name, object);
        if (component != nullptr) {
            resolvedComponents[nameWithNamespace] = component;

            if (resolveCallbacks.contains(nameWithNamespace)) {
                qDebug() << "Calling" << resolveCallbacks[nameWithNamespace].size() << "callbacks";
                for (ComponentCallback const& c : resolveCallbacks[nameWithNamespace]) {
                    c(component);
                }
                resolveCallbacks.remove(nameWithNamespace);
            }
        } else {
            qDebug() << "Failed to create component of type" << (int) type << "-" << object["type"].toString("");
        }
    }
}

void MinecraftJSONParser::requireComponent(const QString &name, ComponentCallback callback) {
    if (resolvedComponents.contains(name)) {
        callback(resolvedComponents[name]);
        return;
    }
    resolveCallbacks[name].append(callback);
}

void MinecraftJSONParser::checkForMissingComponents() {
    for (auto it = resolveCallbacks.begin(); it != resolveCallbacks.end(); it++) {
        qDebug() << "Unresolved callbacks for" << it.key() << "(" << it->count() << ")";
    }
    abort();
}
