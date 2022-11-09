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

    property SingleChannelController channelController: undefined
    property var barColor: "red"

    clip: true

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

    Loader{
        id: rectangleViewLoader

        width: __barSetWidth
        anchors{
            top: parent.top
            bottom: parent.bottom
            left: parent.left
            leftMargin: __barWidth * (channelController.barSetModel.rectBaseOffset - channelController.rangeStartPos)
        }

        active: visible
        visible: channelController.loaderType === SingleChannelController.Rectangle

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
                        implicitHeight: Math.max(1, model.Amplitude * root.height / 100)
                        color: hoverHandler.hovered ? Qt.darker(barColor, 2.0) : barColor

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
                lineColor: barColor
                startPos: channelController.rangeStartPos
                numPoints: Math.min(channelController.displayingDataCount, pointSetModel.length - startPos)
            }
        }

        onLoaded: {
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

        anchors.bottom: root.bottom
        anchors.left: root.left
        anchors.right: root.right

        height: 1
        color: "#d7d7d7"
    }
}
