import QtQuick 2.12
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.4
import com.biren.dataModel 1.0

FocusScope{
    id: root

    //property int currentChannelIndex: graphListView.currentIndex
    property SingleChannelController firstChannelController: null
    property var handleComp: null
    property var handleWidth: 1

    PerfGraphViewController{
        id: graphController
    }

    Row{
        id: backGround

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

            Repeater{
                id: verSplitLineRepeater

                model: 20
                delegate: Rectangle{
                    id: splitLine

                    x: index * (parent.width) / 20
                    y: 0
                    width: 1
                    height: parent.height
                    color: "#d7d7d7"
                }
            }
        }
    }

    ScrollView{
        id: scrollView

        anchors.fill: parent
        anchors.topMargin: 90

        focus: true

        PerfTreeView{
            id: graphTreeView

            focus: true

            rowPadding: 10
            rowSpacing: 0
            rowHeight: 30
            hoverColor: "#80add8e6"
            selectedColor: "#80b0c4de"

            selectionEnabled: true
            hoverEnabled: true

            model: graphController.graphModel

            contentItem: Row{
                id: contentRow
                spacing: handleComp.width

                Rectangle{
                    id: leftComp

                    color: "transparent"

                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    width: leftArea.width - 15 - graphTreeView.rowPadding * currentRow.depth

                    Text {
                        anchors.verticalCenter: leftComp.verticalCenter
                        anchors.left: leftComp.left
                        anchors.right: leftComp.right

                        text: currentRow.currentData.key
                        font.pixelSize: 12
                        font.family: "Times"
                        elide: Text.ElideRight
                    }
                }

                BarSetChannel{
                    id: barset

                    __dataGenerator: graphController.getDataGenerator(currentRow.currentData.value)
                    __barColor: currentRow.depth == 0 ? "red" : currentRow.depth == 1 ? "green" : currentRow.depth ==  2 ? "blue" : "black"

                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    width: rightArea.width

                    Component.onCompleted: {
                        barset.controller.setColumnName(currentRow.currentData.value);
                        graphController.registerSingleChannelController(barset.controller);
                        if(firstChannelController === null)
                        {
                            firstChannelController = barset.controller;
                        }
                    }
                }
            }
        }

        horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff
    }

    SplitView{
        id: splitView

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

                    beginIndex: firstChannelController.rangeStartPos
                    displayingCount: firstChannelController.displayingDataCount
                    totalCount: firstChannelController.getTotalDataCount()

                    Layout.fillWidth: true

                    onSliderBeginIndexChanged: {
                        graphController.onSliderPositionChanged(position)
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
                                graphController.onWheelScaled(wheel.angleDelta)
                                wheel.accepted = true
                            }
                            else
                                wheel.accepted = false
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

        border.color: firstChannelController.loaderType === SingleChannelController.Rectangle ? "cyan" : "black"
    }

    Keys.onPressed: {
        if (event.key === Qt.Key_A) {
            console.log("move left");
            graphController.onLeftKeyPressed()
        }
        else if(event.key === Qt.Key_D){
            console.log("move right");
            graphController.onRightKeyPressed()
        }
        else if(event.key === Qt.Key_W){
            graphListView.decrementCurrentIndex()
        }
        else if(event.key === Qt.Key_S){
            graphListView.incrementCurrentIndex()
        }
    }
}
