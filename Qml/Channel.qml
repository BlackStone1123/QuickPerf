import QtQuick 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import com.biren.dataModel 1.0

Item {
    id: root

    property alias leftWidth: leftComp.width
    property alias rightWidth: barset.width
    property alias spacing: contentRow.spacing
    property alias rightCompVisible: barset.visible

    property string key: null
    property string value: null
    property string type: null

    property color barColor: "red"
    property color labelColor: "black"

    property bool pinButtonVisible: false
    property bool listChannel: false

    signal pinButtonToggled(bool checked)

    Row{
        id: contentRow
        anchors.fill: parent

        RowLayout{
            id: leftComp

            anchors.top: parent.top
            anchors.bottom: parent.bottom

            Text {
                id: leftlabel

                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft

                text: listChannel ? value : key.substring(0, key.indexOf("//"))
                font.pixelSize: 12
                font.family: "Times"

                color: labelColor
                elide: Text.ElideRight
            }

            Button{
                id: pinButton
                Layout.preferredWidth: 23
                Layout.preferredHeight: 23
                Layout.rightMargin: 2
                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight

                checkable: true
                visible: pinButtonVisible || checked

                style: ButtonStyle{
                    background: Rectangle{
                        color: "transparent"

                        Image {
                            id: img
                            source: pinButton.checked ? "qrc:/Image/filledStar.png" : "qrc:/Image/hollowStar.png"
                            smooth: true

                            width: 23
                            height: 23
                        }
                    }
                }

                onClicked: {
                    root.pinButtonToggled(checked)
                }
            }
        }

        SingleChannelController{
            id: singleChannelController

            dataGenerator: graphController.getDataGenerator(key, type, value)
            rangeConverter: graphController.getRangeConverter(type)

            onPindingUpdated:{
                pinButton.checked = singleChannelController.pinding
            }
        }

        BarSetChannel{
            id: barset

            channelController: singleChannelController
            barColor: root.barColor

            anchors.top: parent.top
            anchors.bottom: parent.bottom

            arrowChannel: type === "Event"

            Component.onCompleted: {
                graphController.registerSingleChannelController(key, channelController, listChannel);
            }

            Component.onDestruction: {
                graphController.unRegisterSingleChannelController(key, listChannel);
            }
        }
    }
}
