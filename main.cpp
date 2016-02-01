#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "MinecraftJSONParser.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    MinecraftJSONParser parser;
    parser.loadJSONFile("ui/_ui_defs.json");

    return app.exec();
}
