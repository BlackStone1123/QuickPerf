import QtQuick 2.0
import QtQuick.Layouts 1.0

Rectangle {
    id: root

    property var secondTickCount: 9
    property var tickBlockCount: 10
    property int beginIndex: 0
    property real totalCount: 20000
    property real displayingCount: 100

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

    Component{
        id: splitterComp

        Item {
            id: splitter

            implicitHeight: 40
            implicitWidth: 5

            ColumnLayout{
                anchors.fill: parent
                spacing: 0

                Rectangle{
                    id: top
                    Layout.fillWidth: true
                    Layout.preferredHeight: parent.height / 4

                    color: "gray"
                }

                Rectangle{
                    id: bottom

                    Layout.fillHeight: true
                    Layout.preferredWidth: 1
                    Layout.alignment: Qt.AlignHCenter

                    color: "gray"
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
                spacing: 0

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
            color: "#dcdcdc"

            Layout.fillWidth: true
            Layout.fillHeight: true

            Rectangle{
                id: slider

                color: "transparent"

                x: beginIndex * root.width / totalCount
                anchors.top: parent.top
                anchors.bottom: parent.bottom

                width: root.width * displayingCount / totalCount

                Loader{
                    id: leftSplitter
                    z: 1
                    anchors.left: parent.left
                    anchors.leftMargin: -2
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom

                    sourceComponent: splitterComp
                }

                Rectangle{
                    id: viewPort

                    anchors.left: leftSplitter.right
                    anchors.right: rightSpliter.left
                    anchors.leftMargin: -2
                    anchors.rightMargin: -2
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                }

                Loader{
                    id: rightSpliter

                    anchors.right: parent.right
                    anchors.rightMargin: -2
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    z: 1

                    sourceComponent: splitterComp
                }
            }
        }
    }
}
