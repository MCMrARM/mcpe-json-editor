#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDebug>
#include <QQuickView>
#include <QQmlContext>

#include "MinecraftJSONParser.h"
#include "QMinecraftGUIEditor.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<QMinecraftGUIEditor>("MinecraftJSONEditor", 1, 0, "MinecraftGUIEditor");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    QObject *rootObject = engine.rootObjects().first();
    QQmlContext *qctx = engine.rootContext();

    MinecraftJSONParser parser;
    parser.loadJSONFile("ui/_ui_defs.json");
    parser.checkForMissingComponents();
    qctx->setContextProperty("componentListModel", QVariant::fromValue(parser.getComponentNames()));

    QMinecraftGUIEditor* editor = rootObject->findChild<QMinecraftGUIEditor*>("gui_editor");
    editor->setMinecraftJSONParser(&parser);

    return app.exec();
}
