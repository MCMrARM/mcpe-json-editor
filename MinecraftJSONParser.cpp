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
        int lineStart = 0;
        bool inString = false;
        bool hasSingleLineComment = false;
        for (int i = 0; i < rawData.length() - 1; i++) {
            if (rawData[i] == '\n') {
                lineStart = i;
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
            qDebug() << "JSON file list found.\n";
            for (auto val : it.value().toArray()) {
                qDebug() << "Loading:" << val.toString();
                loadJSONFile(val.toString());
            }
        } else {
            parseComponent(it.key(), mcNamespace, it->toObject());
        }
    }
}

MinecraftGUIComponent* MinecraftJSONParser::parseComponent(QString name, const QString &mcNamespace, const QJsonObject &object) {
    qDebug() << "Parsing component:" << mcNamespace << "." << name;

    MinecraftGUIComponent::Type type = MinecraftGUIComponent::getTypeFromString(object["type"].toString(""));

    int i = name.indexOf('@');
    if (i >= 0) {
        QString extends = name.mid(i + 1);
        name = name.mid(0, i);
    } else {
        if (type == MinecraftGUIComponent::Type::UNKNOWN) {
            qDebug() << "Unknown component type!";
            return nullptr;
        }
        qDebug() << "Component type:" << (int) type;
    }
}

Vec2 MinecraftJSONParser::getVec2(const QJsonValue &val, Vec2 def) {
    if (val.isArray()) {
        QJsonArray arr = val.toArray();
        if (arr.size() == 2) {
            return Vec2 { (float) arr[0].toDouble(), (float) arr[1].toDouble() };
        }
    }
    return def;
}
