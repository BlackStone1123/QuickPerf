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
        id: barSetComp

        Item{
            id: root

            implicitHeight: __channelHeight
            implicitWidth: __barWidth * innerRepeater.count

            Row{
                id: innerRow
                anchors.fill: parent

                Repeater{
                    id: innerRepeater
                    model: __dataSource
                    anchors.fill: parent

                    delegate: Rectangle{
                        id: bar

                        anchors.bottom: parent.bottom
                        width: __barWidth
                        height: model.Amplitude
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
                        onWidthChanged: {
                            console.log("bar width changed:", width)
                        }
                    }
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
                console.log(wheel.x, wheel.pixelDelta, wheel.angleDelta)
//                console.log(
//                    scrollView.flickableItem.contentX,
//                    scrollView.flickableItem.contentY,
//                    scrollView.flickableItem.contentWidth,
//                    scrollView.flickableItem.contentHeight
//                    )

                //scrollView.flickableItem.contentX += (wheel.angleDelta.y > 0 ? 50 : -50)
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
}
