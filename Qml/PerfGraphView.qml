import QtQuick 2.12
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.0
import com.biren.dataModel 1.0

FocusScope{
    id: root

    property int currentChannelIndex: graphListView.currentIndex

    PerfGraphViewController{
        id: controller
    }

    Component{
        id: indicator

        Item{
            id: root
            readonly property int indicatorHeight: 30

            implicitHeight: indicatorHeight

            Text{
                id: textComp
                text: indicatorText
                anchors.centerIn: parent
            }

            Rectangle{
                id: underlineComp

                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: root.bottom
                height: 3
                color: "black"
            }
        }

    }

    Component{
        id: loadingHintComp

        Rectangle {
            id: hintRec
            color: "transparent"

            Text {
                id: hintText
                text: qsTr("loading...")
                font.pixelSize: 20
                anchors.centerIn: parent
            }
        }
    }

    Component{
        id: barSetComp

        Item{
            id: root

            function getBarSetWidth(){
                return Math.max(0, Math.min(root.width, __barWidth * (__dataSource.bundle.length - controller.rangeStartPos)))
            }

            readonly property real __barSetWidth: getBarSetWidth()
            readonly property real __barWidth: root.width / controller.displayingDataCount

            RowLayout{
                id: innerRow

                anchors.fill: parent

                Loader{
                    id: rectangleViewLoader

                    Layout.preferredWidth: __barSetWidth
                    Layout.fillHeight: true

                    active: __dataSource.loaderType === BarSetModel.Rectangle
                    visible: active

                    sourceComponent: Component{
                        id: recViewComp

                        Row{
                            id: repeaterRow

                            Repeater{
                                id: innerRepeater
                                model: __dataSource

                                anchors.fill: parent
                                delegate: Rectangle{
                                    id: bar

                                    anchors.bottom: parent.bottom
                                    implicitWidth: __barWidth
                                    implicitHeight: model.Amplitude
                                    color: hoverHandler.hovered ? Qt.darker(__barColor, 2.0) : __barColor

                                    HoverHandler{
                                        id: hoverHandler
                                    }

                                    Loader{
                                        id: indicatorLoader

                                        property string indicatorText: bar.height

                                        anchors.bottom: bar.top
                                        width: bar.width
                                        active: hoverHandler.hovered
                                        sourceComponent: indicator

                                    }
                                }
                            }
                        }
                    }
                }

                Loader{
                    id: pointSetViewLoader

                    Layout.preferredWidth: __barSetWidth
                    Layout.fillHeight: true

                    //active: __dataSource.loaderType === BarSetModel.PointSet
                    visible: __dataSource.loaderType === BarSetModel.PointSet

                    sourceComponent: Component{
                        id: ptComp

                        PointSetView{
                            id: pointSet

                            stride: __barWidth
                            pointSetModel: __dataSource.bundle
                            lineColor: __barColor
                            startPos: controller.rangeStartPos
                            numPoints: Math.min(controller.displayingDataCount, pointSetModel.length - startPos)
                        }
                    }

                    onLoaded: {
                        console.log("point set loaded!")
                        pointSetViewLoader.item.repaint();
                    }
                    onWidthChanged: {
                        pointSetViewLoader.item.repaint();
                    }
                    onHeightChanged: {
                        pointSetViewLoader.item.repaint();
                    }
                }

                Loader{
                    id: loadingHint

                    active: __dataSource.loading && __barSetWidth < root.width
                    Layout.preferredWidth: root.width - __barSetWidth
                    Layout.fillHeight: true

                    sourceComponent: loadingHintComp
                }
            }

            Rectangle {
                id: endingLine

                anchors.top: root.bottom
                anchors.left: root.left
                anchors.right: root.right

                height: 1
                color: "#d7d7d7"
            }
        }
    }

    ScrollView{
        id: scrollView

        anchors.fill: parent
        focus: true

        ListView{
            id: graphListView

            focus: true
            model: controller.graphModel

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
//            contentX:  controller.rangeStartPos * root.width / controller.displayingDataCount

            delegate: Loader{
                id: barSetLoader

                readonly property var __dataSource: model.BarSetModel
                readonly property color __barColor: model.Color

                anchors.left: parent.left
                anchors.right: parent.right

                height: 100
                sourceComponent: barSetComp
            }
        }
        horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff
    }

    GraphBorder{
        id: border
        visible: root.activeFocus
    }

    MouseArea{
        id: scrollArea
        anchors.fill: parent

        onWheel: {
            if(wheel.modifiers & Qt.ControlModifier){
                controller.onWheelScaled(wheel.angleDelta)
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
            controller.onLeftKeyPressed()
        }
        else if(event.key === Qt.Key_D){
            console.log("move right");
            controller.onRightKeyPressed()
        }
        else if(event.key === Qt.Key_W){
            graphListView.decrementCurrentIndex()
        }
        else if(event.key === Qt.Key_S){
            graphListView.incrementCurrentIndex()
        }
    }
}
