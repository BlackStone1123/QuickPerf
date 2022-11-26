import QtQuick 2.12
import QtQuick.Layouts 1.0
import com.biren.dataModel 1.0

Item{
    id: root

    readonly property real __barWidth: __cycleWidth * channelController.rangeConverter.scale
    readonly property real __cycleWidth: root.width / channelController.rangeConverter.displayingCycleCount
    readonly property real __rangeStartOffset: __cycleWidth * channelController.rangeConverter.rangeStartOffset
    readonly property real __rangeStartPos: channelController.rangeConverter.rangeStartPos
    readonly property real __displayingDataCount: Math.max(0, Math.min(channelController.rangeConverter.displayingDataCount, channelController.bundle.length - __rangeStartPos))
    readonly property real __barSetWidth: __barWidth * __displayingDataCount

    property SingleChannelController channelController: undefined
    property var barColor: "red"
    property bool arrowChannel: false

    clip: true

    Component{
        id: indicator

        Item{
            id: root

            property string indicatorText: "text"

            implicitHeight: textComp.contentHeight
            implicitWidth: textComp.contentWidth

            Text{
                id: textComp
                text: indicatorText

                font.pixelSize: 20
                font.family: "Times"

                anchors.centerIn: parent
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
        id: pointSetViewLoader

        width: __barSetWidth
        anchors{
            top: parent.top
            bottom: parent.bottom
            left: parent.left
            leftMargin: -__rangeStartOffset
        }

        property var hoveredIndex: barsetHover.hovered ? Math.floor((barsetHover.point.position.x + __rangeStartOffset) / __barWidth) : -1

        HoverHandler{
            id: barsetHover
        }

        sourceComponent: Component{
            id: ptComp

            PointSetView{
                id: pointSet

                stride: __barWidth
                pointSetModel: channelController.bundle
                lineColor: barColor
                startPos: __rangeStartPos
                numPoints: __displayingDataCount
                drawArrow: root.arrowChannel
                hoveredIndex: pointSetViewLoader.hoveredIndex
            }
        }

        onLoaded: {
            pointSetViewLoader.item.repaint();
        }
    }

    Loader{
        id: loadingHint

        active: channelController.loading

        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: pointSetViewLoader.right
        anchors.right: parent.right

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

    Loader{
        id: indicatorLoader

        x: barsetHover.point.position.x + 10
        anchors.verticalCenter: parent.verticalCenter

        active: barsetHover.hovered
        sourceComponent: indicator

        Binding{
            target: indicatorLoader.item
            property: "indicatorText"
            value: channelController.bundle[pointSetViewLoader.hoveredIndex + channelController.rangeConverter.rangeStartPos]
            when: indicatorLoader.active
        }

    }
}
