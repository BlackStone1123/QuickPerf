import QtQuick 2.12
import QtQuick.Layouts 1.0

Item {
    id: root

    property int beginIndex: 0
    property real totalCount: 2000
    property real displayingCount: 100

    property bool dragging: sliderArea.drag.active
    property bool leftSplitterDragging: leftSplitter.dragging
    property bool rightSplitterDragging: rightSpliter.dragging

    property Component manipulateComp: Rectangle{
        id: manipulateArea
        color: "black"
        width: 100
        height: 100
    }

    signal sliderBeginIndexChanged(int position)
    signal leftSplitterMoved(int range, bool forward)
    signal rightSplitterMoved(int range, bool forward)
    signal bottomTickPositionChanged(var pos)
    signal bottomTagPositionChanged(var pos)

    QtObject{
        id: _prop

        readonly property var unitStr: ["ps", "ns", "us", "ms", "s"]
        function getTickNumber(index)
        {
            var num = index * 400;
            var scale = 0;

            while(num >= 1000)
            {
                num = num / 1000;
                scale++;
            }

            return num.toFixed(1) + unitStr[scale];
        }
    }

    Component{
        id: tickBlockComp

        Item {
            id: tickBlock

            anchors.fill: parent
            readonly property int secondTickCount: 9

            Rectangle{
                id: major

                x: 0
                width: 1
                height: tickBlock.height
                color: "black"
            }

            Text {
                id: numLabel
                text: tickNumber
                anchors{
                    left: major.right
                    bottom: parent.bottom
                    leftMargin: 2
                }
            }

            Repeater{
                id: secondaryRepeater

                model: tickBlock.secondTickCount
                delegate: Rectangle{
                    id: secondTick

                    x: (index+1) * (tickBlock.width) /(tickBlock.secondTickCount + 1)
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

    Component{
        id: tagComp

        Item {
            id: tag

            implicitWidth: tagTop.width

            property alias tagHeight: tagTop.height
            property var pos: -1

            Rectangle{
                id: tagTop

                width: 10
                height: 10
                color: "red"

                anchors.top: parent.top
            }

            Rectangle{
                id: tagLine

                color: "black"
                width: 2
                height: manipulateLoader.height

                anchors.top: tagTop.bottom
                anchors.horizontalCenter: tagTop.horizontalCenter
            }

            Binding{
                target: tag
                property: "x"
                value: (pos - root.beginIndex) * root.width / root.displayingCount - 5
                when: pos >= 0
            }
        }
    }

    ColumnLayout{
        id: verLayout

        anchors.fill: parent
        spacing: 0

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

                        property var tickNumber: _prop.getTickNumber(index * root.totalCount / tickBlockRepeater.count)

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
            Layout.preferredHeight: 50

            states: [
                State{
                    name: "dragging"

                    AnchorChanges{target: viewPort;anchors.left: undefined;anchors.right: undefined}
                    AnchorChanges{target: leftSplitter; anchors.right: viewPort.left}
                    AnchorChanges{target: rightSpliter;anchors.left: viewPort.right}

                    PropertyChanges{target: leftSplitter;anchors.rightMargin: -leftSplitter.width / 2}
                    PropertyChanges{target: rightSpliter;anchors.leftMargin: -rightSpliter.width / 2}
                    when: dragging
                },

                State{
                    name: "noDragging"

                    AnchorChanges{target: viewPort;anchors.left: leftSplitter.right; anchors.right: rightSpliter.left}
                    AnchorChanges{target: leftSplitter; anchors.right: undefined}
                    AnchorChanges{target: rightSpliter; anchors.left: undefined}

                    PropertyChanges{target: leftSplitter; x: beginIndex * root.width / totalCount - leftSplitter.width / 2}
                    PropertyChanges{target: rightSpliter; x: (root.beginIndex + root.displayingCount) * root.width / totalCount - rightSpliter.width / 2}
                    PropertyChanges {target: viewPort; anchors.leftMargin: -leftSplitter.width / 2; anchors.rightMargin: -rightSpliter.width / 2}
                    when: !leftSplitterDragging && !rightSplitterDragging &&　!dragging
                },

                State{
                    name: "splitterDragging"

                    AnchorChanges{target: viewPort;anchors.left: leftSplitter.right;anchors.right: rightSpliter.left}
                    AnchorChanges{target: leftSplitter; anchors.right: undefined}
                    AnchorChanges{target: rightSpliter;anchors.left: undefined}

                    PropertyChanges{target: leftSplitter; x: undefined}
                    PropertyChanges{target: rightSpliter; x: undefined}
                    PropertyChanges {target: viewPort; anchors.leftMargin: -leftSplitter.width / 2; anchors.rightMargin: -rightSpliter.width / 2}
                    when: leftSplitterDragging || rightSplitterDragging
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

                onXChanged: {
                    if(leftSplitterDragging){
                        root.leftSplitterMoved(Math.abs( leftSplitter.x- leftSplitter.currentX ) * totalCount / root.width, x > currentX)
                    }
                    leftSplitter.currentX = x;
                }
            }

            Rectangle{
                id: viewPort

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

                onXChanged: {
                    if(rightSplitterDragging){
                        root.rightSplitterMoved(Math.abs( rightSpliter.x- rightSpliter.currentX ) * totalCount / root.width, x > currentX)
                    }

                    rightSpliter.currentX = x;
                }
            }
        }

        Item{
            id: bottomTick

            Layout.fillWidth: true
            Layout.preferredHeight: 30

            readonly property int batchSize: root.displayingCount / 10
            readonly property real batchBlockSize: batchSize * root.width / displayingCount

            function getTickPosition(index)
            {
                var beginPos = beginIndex * root.width / root.displayingCount
                var firstTickPos = bottomTick.batchBlockSize * Math.ceil(root.beginIndex / bottomTick.batchSize)

                if(index === 0)
                    return firstTickPos - beginPos;

                return bottomTickRepeater.itemAt(index-1).x + bottomTick.batchBlockSize
            }

            Text {
                id: baseTick
                text: _prop.getTickNumber(root.beginIndex)
                x: -50
                anchors.verticalCenter: parent.verticalCenter
            }

            Repeater{
                id: bottomTickRepeater
                model: Math.ceil(root.width / bottomTick.batchBlockSize)

                delegate: Item{
                    id: tickItem

                    width: 18
                    height: 30
                    x: bottomTick.getTickPosition(index)
                    y: 0

                    Rectangle{
                        id: tick

                        width: 1
                        anchors{
                            left: parent.left
                            top: parent.top
                            bottom: parent.bottom
                        }
                        color: "black"
                    }

                    Text {
                        id: numLabel
                        text: "+" + _prop.getTickNumber(bottomTick.batchSize * (Math.ceil(root.beginIndex / bottomTick.batchSize) + index) - root.beginIndex)
                        anchors{
                            left: tick.right
                            right: parent.right
                            leftMargin: 2
                        }
                    }

                    onXChanged: {
                        if(index === bottomTickRepeater.count - 1){
                            var pos = [];
                            for(var i = 0; i<bottomTickRepeater.count; i++)
                            {
                                pos.push(bottomTickRepeater.itemAt(i).x)
                            }

                            root.bottomTickPositionChanged(pos);
                        }
                    }
                }
            }

            Loader{
                id: hoverTag

                x: bottomHoverHdl.point.position.x - 5

                anchors.top: parent.bottom
                anchors.topMargin: -10
                active: bottomHoverHdl.hovered

                sourceComponent: tagComp
            }

            HoverHandler{
                id: bottomHoverHdl
            }

            TapHandler{
                id: bottomTopHdl

                onTapped: {

                    var stride = root.width / root.displayingCount;
                    var index = root.beginIndex + bottomHoverHdl.point.position.x / stride

                    tagComp.createObject(parent, {pos: index, anchors: {top: parent.bottom, topMargin: -10}})
                }
            }
        }

        Loader{
            id: manipulateLoader

            Layout.fillWidth: true
            Layout.fillHeight: true

            sourceComponent: manipulateComp
        }
    }
}
