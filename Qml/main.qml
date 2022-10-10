import QtQuick 2.12
import QtQuick.Layouts 1.0
import com.biren.dataModel 1.0

Rectangle {
    id: root

    RowLayout{
        anchors.fill: parent
        spacing: 0

        PerfListView{
            id: listView

            Layout.fillHeight: true
            Layout.preferredWidth: 150
            Layout.minimumWidth: 150
        }

        PerfGraphView{
            id: graphView

            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }
}
