import QtQuick 2.3
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1
import Material 0.2
import QtQuick.Controls 1.4 as Controls
import Material.ListItems 0.1 as ListItem

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

            Column {
                width: parent.width

                ListItem.Standard {
                    text: "Test"
                }
            }
        }
/*
        Rectangle {
            id: elements_container

            x: 5
            y: 5
            width: Units.dp(320)
            height: Units.dp(240)
        }
*/
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
