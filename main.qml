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

            screenWidth: Units.dp(320)
            screenHeight: Units.dp(240)

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

                onTriggered: displaySettingsDialog.show()
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

        positiveButtonText: "Done"
        negativeButton.visible: false
    }
}
