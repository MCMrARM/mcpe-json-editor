TEMPLATE = app

QT += qml quick
CONFIG += c++11

SOURCES += main.cpp \
    MinecraftJSONParser.cpp \
    MinecraftGUIComponent.cpp \
    MinecraftGUIVariable.cpp \
    MinecraftGUIContext.cpp \
    MinecraftGUIButtonId.cpp \
    QMinecraftGUIEditor.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    MinecraftJSONParser.h \
    MinecraftGUIComponent.h \
    Vec2.h \
    MinecraftGUIBindings.h \
    MinecraftGUIVariable.h \
    MinecraftGUIContext.h \
    MinecraftGUIButtonId.h \
    QMinecraftGUIEditor.h
