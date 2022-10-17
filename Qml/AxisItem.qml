import QtQuick 2.0
import QtQuick.Layouts 1.0

Rectangle {
    id: root

    property var secondTickCount: 9
    property var tickBlockCount: 10

    implicitHeight: 90

    Component{
        id: tickBlockComp

        Item {
            id: tickBlock

            anchors.fill: parent
            Rectangle{
                id: major

                x: 0
                width: 1
                height: tickBlock.height
                color: "black"
            }

            Repeater{
                id: secondaryRepeater

                model: secondTickCount
                delegate: Rectangle{
                    id: secondTick

                    x: (index+1) * (tickBlock.width) /(secondTickCount + 1)
                    width: 1
                    height: tickBlock.height / 3
                    anchors.top: parent.top
                    color: "black"
                }
            }
        }
    }

    ColumnLayout{
        id: verLayout

        anchors.fill: parent

        Item{
            id: topTickItem

            Layout.fillWidth: true
            Layout.preferredHeight: 20

            RowLayout{
                id: horLayout

                anchors.fill: parent
                Repeater{
                    id: tickBlockRepeater

                    model: tickBlockCount
                    delegate: Loader{
                        id: tickBlockLoader

                        Layout.fillHeight: true
                        Layout.fillWidth: true

                        sourceComponent: tickBlockComp
                    }
                }
            }
        }

        Rectangle{
            color: "gray"

            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
}
