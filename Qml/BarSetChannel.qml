import QtQuick 2.12
import QtQuick.Layouts 1.0
import com.biren.dataModel 1.0

Item{
    id: root

    function getBarSetWidth(){
        return Math.max(0, Math.min(root.width, __barWidth * (channelController.bundle.length - channelController.rangeStartPos)))
    }

    readonly property real __barSetWidth: getBarSetWidth()
    readonly property real __barWidth: root.width / channelController.displayingDataCount
    readonly property SingleChannelController controller: channelController

    property var __dataGenerator: null
    property var __barColor: "red"

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

    SingleChannelController{
        id: channelController

        dataGenerator: __dataGenerator
    }

    Loader{
        id: rectangleViewLoader

        width: __barSetWidth
        anchors{
            top: parent.top
            bottom: parent.bottom
            left: parent.left
            leftMargin: __barWidth * (channelController.rectBaseOffset - channelController.rangeStartPos)
        }

        active: channelController.loaderType === SingleChannelController.Rectangle
        visible: active

        sourceComponent: Component{
            id: recViewComp

            Row{
                id: repeaterRow

                Repeater{
                    id: innerRepeater
                    model: channelController.barSetModel

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

        onLoaded: {
            console.log("rectangle view loaded")
        }
    }

    Loader{
        id: pointSetViewLoader

        width: __barSetWidth
        anchors{
            top: parent.top
            bottom: parent.bottom
            left: parent.left
        }

        visible: channelController.loaderType === SingleChannelController.PointSet
        sourceComponent: Component{
            id: ptComp

            PointSetView{
                id: pointSet

                stride: __barWidth
                pointSetModel: channelController.bundle
                lineColor: __barColor
                startPos: channelController.rangeStartPos
                numPoints: Math.min(channelController.displayingDataCount, pointSetModel.length - startPos)
            }
        }

        onLoaded: {
            console.log("point set loaded!")
            pointSetViewLoader.item.repaint();
        }
    }

    Loader{
        id: loadingHint

        active: channelController.loading && __barSetWidth < root.width

        x: __barSetWidth
        width: root.width - __barSetWidth

        anchors.top: parent.top
        anchors.bottom: parent.bottom

        sourceComponent: loadingHintComp
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
