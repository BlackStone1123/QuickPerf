import QtQuick 2.12
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.0
import com.biren.dataModel 1.0

FocusScope{
    id: root

    property int currentChannelIndex: graphListView.currentIndex
    property SingleChannelController firshChannelController: null

    PerfGraphViewController{
        id: graphController
    }

    ScrollView{
        id: scrollView

        anchors.fill: parent
        focus: true

        ListView{
            id: graphListView

            focus: true
            model: graphController.graphModel

            add: Transition{
                NumberAnimation { properties: "opacity"; from: 0; to: 1.0; duration: 200}
            }

            highlight: Rectangle{
                color: "lightsteelblue"
                anchors{
                    left: parent.left
                    right: parent.right
                }
            }

            contentWidth: root.width
            delegate: Loader{
                id: barSetLoader

                anchors.left: parent.left
                anchors.right: parent.right

                height: 100
                source: "BarSetChannel.qml"

                Binding {
                  target: barSetLoader.item
                  property: "__dataGenerator"
                  value: model.Generator
                  when: barSetLoader.status === Loader.Ready
                }

                Binding {
                  target: barSetLoader.item
                  property: "__barColor"
                  value: model.Color
                  when: barSetLoader.status === Loader.Ready
                }

                onLoaded: {
                    graphController.registerSingleChannelController(item.controller);
                    if(firshChannelController === null)
                    {
                        firshChannelController = item.controller;
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
