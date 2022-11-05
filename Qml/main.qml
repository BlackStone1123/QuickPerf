import QtQuick 2.12
import QtQuick.Layouts 1.0
import com.biren.dataModel 1.0

Rectangle {
    id: root

    RowLayout{
        anchors.fill: parent
        spacing: 0

        PerfGraphView{
            id: graphView

            Layout.fillHeight: true
            Layout.fillWidth: true

            //KeyNavigation.tab: listView
            focus: true
        }
    }
}
