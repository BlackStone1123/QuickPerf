import QtQuick 2.12
import QtQuick.Layouts 1.0
import com.biren.dataModel 1.0

FocusScope{
    id: root

    PerfListViewController{
        id: controller
    }

    PerfTreeView {
        id: jsonView

        anchors.fill: parent
        anchors.topMargin: 90

        model: controller.listModel
        rowPadding: 10
        rowSpacing: 0
        rowHeight: 30

        selectionEnabled: true
        hoverEnabled: true

        contentItem: RowLayout{
            Text {
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignLeft
                text: currentRow.currentData.key
            }
        }

        GraphBorder{
            id: border
            visible: root.activeFocus
        }
    }
}
