import QtQuick 2.3
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1
import Material 0.2
import QtQuick.Controls 1.4 as Controls
import Material.ListItems 0.1 as ListItem
import MinecraftJSONEditor 1.0

ApplicationWindow {
    visible: true

    title: "GUI Editor"

    theme {
        primaryColor: "blue"
        accentColor: "red"
        tabHighlightColor: "white"
    }

    initialPage : TabbedPage {
        id: page
        title: "Minecraft: PE GUI Editor"

        rightSidebar: PageSidebar {
            title: "Code"

            showing: false

            width: Units.dp(320)

            actions: [
                Action {
                    iconName: "action/code"
                    text: "Hide code"

                    onTriggered: {
                        page.rightSidebar.showing = false
                        showAction.visible = true
                    }
                }
            ]
        }

        Sidebar {
            id: sidebar
            objectName: "componentList"

            Column {
                width: parent.width

                Repeater {
                    id: componentList
                    model: componentListModel

                    property string selectedComponent: ""

                    delegate: ListItem.Standard {
                        text: modelData
                        selected: modelData === componentList.selectedComponent
                        onClicked: {
                            componentList.selectedComponent = modelData
                            guiEditor.editComponent = modelData
                        }
                    }
                }
            }
        }

        MinecraftGUIEditor {
            id: guiEditor
            objectName: "gui_editor"

            screenWidth: 320
            screenHeight: 240

            anchors {
                left: sidebar.right
                right: parent.right
                top: parent.top
                bottom: parent.bottom
            }
        }

        actions: [
            Action {
                id: settingsAction
                iconName: "action/settings"
                text: "Display settings"

                onTriggered: {
                    guiEditorWidth.text = guiEditor.screenWidth
                    guiEditorHeight.text = guiEditor.screenHeight
                    guiEditorPixelSize.text = guiEditor.dpSize
                    displaySettingsDialog.show()
                }
            },

            Action {
                id: showAction
                iconName: "action/code"
                text: "Show code"

                onTriggered: {
                    page.rightSidebar.showing = true
                    showAction.visible = false
                }
            }
        ]
    }

    Dialog {
        id: displaySettingsDialog
        title: "Display settings"

        Grid {
            columns: 2
            spacing: Units.dp(8)

            Rectangle {
                width: Units.dp(120)
                height: Units.dp(40)
                TextField {
                    id: guiEditorWidth
                    width: parent.width
                    anchors.bottom: parent.bottom
                    placeholderText: "Screen width"
                    floatingLabel: true
                }
            }
            Rectangle {
                width: Units.dp(120)
                height: Units.dp(40)
                TextField {
                    id: guiEditorHeight
                    width: parent.width
                    anchors.bottom: parent.bottom
                    placeholderText: "Screen height"
                    floatingLabel: true
                }
            }
        }
        Rectangle {
            width: Units.dp(240)
            height: Units.dp(40)
            TextField {
                id: guiEditorPixelSize
                width: parent.width
                anchors.bottom: parent.bottom
                placeholderText: "Pixel size"
                floatingLabel: true
            }
        }

        positiveButtonText: "Done"
        negativeButton.visible: false

        onAccepted: {
            guiEditor.screenWidth = parseInt(guiEditorWidth.text)
            guiEditor.screenHeight = parseInt(guiEditorHeight.text)
            guiEditor.dpSize = parseInt(guiEditorPixelSize.text)
        }
    }
}
