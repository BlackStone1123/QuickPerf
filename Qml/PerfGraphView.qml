import QtQuick 2.12
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.0
import com.biren.dataModel 1.0

FocusScope{
    id: root

    //property int currentChannelIndex: graphListView.currentIndex
    property SingleChannelController firshChannelController: null

    PerfGraphViewController{
        id: graphController
    }

    RowLayout{
        id: horLayout

        anchors.fill: parent

        Rectangle{
            id: leftArea

            Layout.fillHeight: true
            Layout.preferredWidth: 120
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

                    Layout.fillWidth: true
                }

                Item{
                    id: panelLayer

                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    Repeater{
                        id: verSplitLineRepeater
                        anchors.fill: parent

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

            selectionEnabled: true
            hoverEnabled: true

            model: graphController.graphModel

            contentItem: RowLayout{

                Rectangle{
                    id: leftComp

                    color: "transparent"

                    Layout.fillHeight: true
                    Layout.preferredWidth: 120 - 15 - graphTreeView.rowPadding * currentRow.depth

                    Text {
                        anchors.verticalCenter: leftComp.verticalCenter
                        anchors.left: leftComp.left
                        text: currentRow.currentData.key
                    }
                }

                BarSetChannel{
                    id: barset

                    __dataGenerator: graphController.getDataGenerator(currentRow.currentData.key)
                    __barColor: currentRow.depth == 0 ? "red" : currentRow.depth == 1 ? "green" : currentRow.depth ==  2 ? "blue" : "black"

                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    Component.onCompleted: {
                        graphController.registerSingleChannelController(barset.controller);
                        if(firshChannelController === null)
                        {
                            firshChannelController = barset.controller;
                        }
                    }
                }
            }
        }

        horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff
    }

    GraphBorder{
        id: border
        visible: root.activeFocus

        border.color: firshChannelController.loaderType === SingleChannelController.Rectangle ? "cyan" : "black"
    }

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
