import QtQuick 2.12
import QtQuick.Layouts 1.0

Rectangle {
    id: root

    property var secondTickCount: 9
    property int beginIndex: 0
    property real totalCount: 2000
    property real displayingCount: 100
    property bool dragging: sliderArea.drag.active
    property bool leftSplitterDragging: leftSplitter.dragging
    property bool rightSplitterDragging: rightSpliter.dragging

    signal sliderBeginIndexChanged(int position)
    signal leftSplitterMoved(int range, bool forward)
    signal rightSplitterMoved(int range, bool forward)

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
            implicitWidth: 6

            property bool dragging: mouseArea.drag.active

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

            MouseArea{
                id: mouseArea

                anchors.fill: parent
                hoverEnabled: true
                cursorShape: containsMouse ? Qt.SizeHorCursor : Qt.ArrowCursor

                drag{
                    target: splitter.parent
                    axis: Drag.XAxis
                    minimumX: -splitter.width / 2
                    maximumX: root.width - splitter.width / 2
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

                    model: 10
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
            id: sliderChannel

            color: "#dcdcdc"

            Layout.fillWidth: true
            Layout.fillHeight: true

            states: [
                State{
                    name: "anchorChange"

                    AnchorChanges{
                        target: viewPort
                        anchors.left: undefined
                        anchors.right: undefined
                    }

                    AnchorChanges{
                        target: leftSplitter
                        anchors.right: viewPort.left
                    }

                    AnchorChanges{
                        target: rightSpliter
                        anchors.left: viewPort.right
                    }

                    PropertyChanges{
                        target: leftSplitter
                        anchors.rightMargin: -leftSplitter.width / 2
                    }

                    PropertyChanges{
                        target: rightSpliter
                        anchors.leftMargin: -rightSpliter.width / 2
                    }
                    when: dragging
                }

            ]

            Loader{
                id: leftSplitter

                property bool dragging: item.dragging
                property real currentX: 0

                z: 1

                anchors.top: parent.top
                anchors.bottom: parent.bottom

                sourceComponent: splitterComp

                Binding{
                    target: leftSplitter
                    property: "x"
                    value: beginIndex * root.width / totalCount - leftSplitter.width / 2
                    when: /*!leftSplitterDragging&& !rightSplitterDragging && !*/!dragging
                }

                onXChanged: {
                    if(leftSplitterDragging){
                        root.leftSplitterMoved(Math.abs( leftSplitter.x- leftSplitter.currentX ) * totalCount / root.width, x > currentX)
                    }
                    leftSplitter.currentX = x;
                }
            }

            Rectangle{
                id: viewPort

                anchors.left: leftSplitter.right
                anchors.right: rightSpliter.left
                anchors.leftMargin: -leftSplitter.width/2
                anchors.rightMargin: - rightSpliter.width/2
                anchors.top: parent.top
                anchors.bottom: parent.bottom

                MouseArea{
                    id: sliderArea

                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: containsMouse ? Qt.SizeAllCursor : Qt.ArrowCursor

                    drag{
                        target: viewPort
                        axis: Drag.XAxis
                        minimumX: 0
                        maximumX: sliderChannel.width - viewPort.width
                    }
                }

                onXChanged: {
                    if(dragging){
                        root.sliderBeginIndexChanged(viewPort.x * root.totalCount / root.width);
                    }
                }
            }

            Loader{
                id: rightSpliter

                property bool dragging: item.dragging
                property real currentX: 0

                anchors.top: parent.top
                anchors.bottom: parent.bottom

                z: 1

                sourceComponent: splitterComp

                Binding{
                    target: rightSpliter
                    property: "x"
                    value: (root.beginIndex + root.displayingCount) * root.width / totalCount - rightSpliter.width / 2
                    when: !leftSplitterDragging && !rightSplitterDragging &&　!dragging
                }

                onXChanged: {
                    if(rightSplitterDragging){
                        root.rightSplitterMoved(Math.abs( rightSpliter.x- rightSpliter.currentX ) * totalCount / root.width, x > currentX)
                    }

                    rightSpliter.currentX = x;
                }
            }
        }
    }
}
