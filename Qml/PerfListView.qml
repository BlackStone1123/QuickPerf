import QtQuick 2.12
import QtQml.Models 2.12

FocusScope{
    id: root

    ListModel {
        id: fruitModel

        ListElement {
            name: "Apple"
            cost: 2.45
        }
        ListElement {
            name: "Orange"
            cost: 3.25
        }
        ListElement {
            name: "Banana"
            cost: 1.95
        }
        ListElement {
            name: "Fuckker"
            cost: 20
        }
        ListElement {
            name: "ddddd"
            cost: 40
        }
    }

    Component {
        id: fruitDelegate

        Row {
            anchors.left: parent.left
            anchors.right: parent.right

            height: 40
            id: fruit
            Text { text: " Fruit: " + name }
            Text { text: " Cost: $" + cost }
        }
    }

    ListView {
        id: view

        anchors.fill: parent
        focus: true

        model: fruitModel
        delegate: fruitDelegate
        highlight: Rectangle { color: "green"; radius: 2 }
    }

    GraphBorder{
        id: border
        visible: root.activeFocus
    }
}
