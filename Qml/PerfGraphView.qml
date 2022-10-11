import QtQuick 2.12
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.0
import com.biren.dataModel 1.0

Item{
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
            anchors.fill: parent

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

            implicitHeight: __channelHeight
            implicitWidth: innerRepeater.implicitWidth > __ViewWidth ? innerRepeater.implicitWidth : __ViewWidth

            Row{
                id: innerRow
                anchors.fill: parent

                Repeater{
                    id: innerRepeater
                    model: __dataSource

                    implicitWidth: __barWidth * count
                    implicitHeight: __channelHeight

                    delegate: Rectangle{
                        id: bar

                        anchors.bottom: parent.bottom
                        implicitWidth: __barWidth
                        implicitHeight: model.Amplitude
                        color: __barColor

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

                Loader{
                    id: loadingHint

                    active: innerRepeater.width < root.width
                    width: root.width - innerRepeater.width
                    height: __channelHeight
                    sourceComponent: loadingHintComp
                }
            }
        }
    }

    ScrollView{
        id: scrollView
        anchors.fill: parent

        ListView{
            id: graphListView
            model: controller.graphModel
            focus: true

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

            contentWidth: contentItem.childrenRect.width

            delegate: Loader{
                id: barSetLoader

                readonly property var __dataSource: model.BarSetModel
                readonly property color __barColor: model.Color
                readonly property int __channelHeight: 100
                readonly property real __barWidth: root.width / controller.displayingDataCount
                readonly property real __ViewWidth: root.width
                sourceComponent: barSetComp

                HoverHandler{
                    onHoveredChanged: {
                        if(hovered)
                        {
                            graphListView.currentIndex = model.index
                        }
                    }
                }
            }
        }

        horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff
    }


    MouseArea{
        id: scrollArea
        anchors.fill: parent

        onWheel: {
            if(wheel.modifiers & Qt.ControlModifier){
//                console.log(wheel.x, wheel.pixelDelta, wheel.angleDelta)
//                console.log(
//                    scrollView.flickableItem.contentX,
//                    scrollView.flickableItem.contentY,
//                    scrollView.flickableItem.contentWidth,
//                    scrollView.flickableItem.contentHeight
//                    )

                controller.onWheelScaled(wheel.angleDelta)
                wheel.accepted = true
            }
            else
                wheel.accepted = false
        }

        onPressed: {
            controller.insertRowBefore(currentChannelIndex)
            mouse.accepted = false
        }
    }
}
