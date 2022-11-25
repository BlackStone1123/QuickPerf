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

    // Controller of the whole tree view
    PerfGraphViewController{
        id: graphController
    }

    // background layer merely show some split lines
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
                target: rightArea
                onBottomTickPositionChanged:{
                    rightBack.positions = pos;
                }
            }
        }
    }

    // middle layer serves as the main layer that shows the data
    // of corresponding data model. Which is composed of two component
    // (1) The list view is the one served as showing pined channels which
    //     are listed Sequentially.
    // (2) The scrollView is a wrapper of the graphTreeView, and the tree view
    //     is the real content item, the scrollView will provide a scrollbar if
    //     the size of tree view out of view port range.
    // (3) The graphTreeView is used to display the hierarchy data structure of its
    //     underlying tree model. Which will provide indent as the depth of hierarchy goes
    //     further and collapse all of the child items if a parent item is collapsed.
    ColumnLayout {
        id: middleLayer

        anchors.fill: parent
        spacing: 0

        ListView{
            id: listView

            Layout.preferredHeight: channelHeight * count
            Layout.fillWidth: true
            Layout.topMargin: 100

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
                type: model.DataType

                x: padding
                width: parent.width - padding
                height: channelHeight

                onPinButtonToggled: {
                    graphController.onPinButtonToggled(listChannel.key, listChannel.value, listChannel.type, checked, false)
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

                    // leftPadding calculate the width of indent of current row and the width of collapse sign
                    property var leftPadding: graphTreeView.rowPadding * currentRow.depth + 15
                    property var pixelDepth: 0
                    property bool inPort: (pixelDepth + channelHeight > 0) && (pixelDepth < graphTreeView.height)
                    property bool showPlaceHolder: currentRow.depth === 0 && currentRow.expanded

                    // change the color of the collapse sign of current row.
                    onShowPlaceHolderChanged: {
                        currentRow.handleColor = showPlaceHolder ? "white" : "black"
                    }

                    // This place holder only visible when a top level item is expanded
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
                        type: currentRow.currentData.type

                        anchors.fill: parent
                        onPinButtonToggled: {
                            graphController.onPinButtonToggled(channel.key, channel.value, channel.type, checked, true)
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

    // The top layer is a split view which lies atop middle layer and split the view into two
    // parts, the width of left area binds to the left content of middle layer width, and the
    // width of right area binds to the right content middle layer width. So that the content of
    // middle layer can be adjusted when dragging the handle of the split view. So the top layer
    // merely provide features as:
    // (1) adjust the left and right content width of the middle layer with handle.
    // (2) fill the right area of the split view with a axis item, which shows the displaying data
    //     range, and provide a slider to control the displaying start index and displaying count of
    //     underlying datas.
    SplitView{
        id: topLayer

        anchors.fill: parent
        orientation: Qt.Horizontal

        Rectangle{
            id: leftArea

            Layout.fillHeight: true
            Layout.preferredWidth: 200
            Layout.minimumWidth: 200

            color: "transparent"
        }

        AxisItem{
            id: rightArea

            totalCount: graphController.totalCycleRange
            displayingCount: graphController.rangeConverter.displayingDataCount
            beginIndex: graphController.rangeConverter.rangeStartPos

            Layout.fillHeight: true
            Layout.fillWidth: true
            focus: true

            manipulateComp: Item {
                id: scrollItem

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
