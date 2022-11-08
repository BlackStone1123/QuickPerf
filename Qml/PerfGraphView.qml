import QtQuick 2.12
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import com.biren.dataModel 1.0
import QtQml 2.12

FocusScope{
    id: root

    property var handleComp: null
    property var handleWidth: 1
    readonly property int channelHeight: 30

    PerfGraphViewController{
        id: graphController
    }

    Row{
        id: backLayer

        anchors.fill: parent
        spacing: handleComp.width

        Rectangle{
            id: leftBack

            anchors.top: parent.top
            anchors.bottom: parent.bottom

            width: leftArea.width
        }

        Item{
            id: rightBack

            anchors.top: parent.top
            anchors.bottom: parent.bottom

            width: rightArea.width

            property var positions: []
            Repeater{
                id: verSplitLineRepeater

                model: rightBack.positions

                delegate: Rectangle{
                    id: splitLine

                    x: modelData
                    y: 0
                    width: 1
                    height: parent.height
                    color: "#d7d7d7"
                }
            }

            Connections{
                target: axisItem
                onBottomTickPositionChanged:{
                    rightBack.positions = pos;
                }
            }
        }
    }

    ColumnLayout {
        id: middleLayer

        anchors.fill: parent
        spacing: 0

        ListView{
            id: listView

            Layout.preferredHeight: channelHeight * count
            Layout.fillWidth: true
            Layout.topMargin: axisItem.height
            focus: true

            model: graphController.listModel
            interactive: false

            delegate: Channel{
                id: listChannel

                property var padding: 10

                leftWidth: leftArea.width - padding
                rightWidth: rightArea.width
                spacing: handleComp.width

                rightCompVisible: true
                barColor: "orangered"
                listChannel: true

                key: model.Label
                value: model.ColumnName

                x: padding
                width: parent.width - padding
                height: channelHeight

                onPinButtonToggled: {
                    graphController.onPinButtonToggled(listChannel.key, listChannel.value, checked, false)
                }
            }
        }

        Rectangle{
            Layout.fillWidth: true
            Layout.preferredHeight: 2
            color: "black"
        }

        ScrollView{
            id: scrollView

            Layout.fillWidth: true
            Layout.fillHeight: true

            PerfTreeView{
                id: graphTreeView

                focus: true

                rowPadding: 10
                rowSpacing: 0
                rowHeight: channelHeight
                hoverColor: "#80add8e6"
                selectedColor: "#80b0c4de"

                selectionEnabled: true
                hoverEnabled: true

                model: graphController.graphModel
                contentItem: Item {
                    id: content

                    property var leftPadding: graphTreeView.rowPadding * currentRow.depth + 15
                    property var pixelDepth: 0
                    property bool inPort: (pixelDepth + channelHeight > 0) && (pixelDepth < graphTreeView.height)
                    property bool showPlaceHolder: currentRow.depth === 0 && currentRow.expanded

                    onShowPlaceHolderChanged: {
                        currentRow.handleColor = showPlaceHolder ? "white" : "black"
                    }

                    Rectangle{
                        id: placeHolder

                        x: -leftPadding
                        width: root.width
                        color: content.showPlaceHolder ? "black" : "transparent"

                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                    }

                    Channel{
                        id: channel

                        leftWidth: leftArea.width - leftPadding
                        rightWidth: rightArea.width
                        spacing: handleComp.width

                        listChannel: false
                        rightCompVisible: !currentRow.expanded && content.inPort
                        pinButtonVisible: !currentRow.hasChildren && currentRow.isHoveredIndex
                        labelColor: content.showPlaceHolder ? "white" : "black"
                        barColor: currentRow.depth === 0 ? "lightseagreen" : currentRow.depth === 1 ? "mediumpurple" : currentRow.depth ===  2 ? "orange" : "orangered"

                        key: currentRow.currentData.key + "//Path" + currentRow.path
                        value: currentRow.currentData.value

                        anchors.fill: parent
                        onPinButtonToggled: {
                            graphController.onPinButtonToggled(channel.key, channel.value, checked, true)
                        }
                    }

                    Timer{
                        id: changeDepthTimer
                        interval: 500
                        triggeredOnStart: false
                        repeat: false
                        onTriggered: {
                            pixelDepth = currentRow.currentItem.mapToItem(graphTreeView, 0, 0).y
                        }
                    }

                    Connections{
                        target: graphTreeView
                        onContentHeightChanged:{
                            changeDepthTimer.start()
                        }
                    }

                    Connections{
                        target: graphTreeView
                        onContentYChanged:{
                            changeDepthTimer.start()
                        }
                    }
                }
            }

            horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff
        }
    }

    SplitView{
        id: topLayer

        anchors.fill: parent
        orientation: Qt.Horizontal

        Rectangle{
            id: leftArea

            Layout.fillHeight: true
            Layout.preferredWidth: 120
            Layout.minimumWidth: 120

            color: "transparent"
        }

        Item {
            id: rightArea

            Layout.fillHeight: true
            Layout.fillWidth: true

            ColumnLayout{
                id: verLayout

                anchors.fill: parent
                spacing: 0

                AxisItem{
                    id: axisItem

                    property SingleChannelController firstChannelController: graphController.topController

                    totalCount: firstChannelController.getTotalDataCount()
                    displayingCount: firstChannelController.displayingDataCount
                    beginIndex: firstChannelController.rangeStartPos

                    Layout.fillWidth: true

                    onSliderBeginIndexChanged:{
                        graphController.onSliderPositionChanged(position)
                    }

                    onLeftSplitterMoved: {
                        graphController.onSplitterDragging(range, true, forward)
                    }

                    onRightSplitterMoved: {
                        graphController.onSplitterDragging(range, false, forward)
                    }
                }

                Item{
                    id: manipulateArea

                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    MouseArea{
                        id: scrollArea
                        anchors.fill: parent

                        onWheel: {
                            if(root.activeFocus && (wheel.modifiers & Qt.ControlModifier)){
                                graphController.onWheelScaled(wheel.x / parent.width, wheel.angleDelta)
                                wheel.accepted = true
                            }
                            else{
                                wheel.accepted = false
                            }
                        }

                        onPressed: {
                            mouse.accepted = false
                        }
                    }
                }
            }
        }

        handleDelegate: Rectangle{
            id: handle

            width: styleData.hovered || styleData.pressed? 3 * handleWidth : handleWidth
            height: root.height
            color: "black"

            Component.onCompleted: {
                handleComp = handle;
            }
        }
    }

    GraphBorder{
        id: border
        visible: root.activeFocus

        border.color: "black"
    }

    Keys.onPressed: {
        if (event.key === Qt.Key_A) {
            graphController.onLeftKeyPressed()
        }
        else if(event.key === Qt.Key_D){
            graphController.onRightKeyPressed()
        }
    }
}
