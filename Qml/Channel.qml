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
    property alias barColor: barset.barColor
    property alias pinButtonChecked: pinButton.checked

    property var key: null
    property var value: null
    property bool pinButtonVisible: false

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

                text: key
                font.pixelSize: 12
                font.family: "Times"

                color: barset.visible ? "black" : "white"
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

        BarSetChannel{
            id: barset

            dataGenerator: graphController.getDataGenerator(value)

            anchors.top: parent.top
            anchors.bottom: parent.bottom

            Component.onCompleted: {
                graphController.registerSingleChannelController(key, barset.controller);
            }

            Component.onDestruction: {
                graphController.unRegisterSingleChannelController(key);
            }
        }
    }
}
