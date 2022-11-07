import QtQuick 2.12
import QtQuick.Controls 1.0
//import QtQml 2.15

Flickable {
    id: root

    implicitWidth: 400
    implicitHeight: 400
    clip: true

    property var model
    readonly property alias currentIndex: tree.selectedIndex
    readonly property alias currentItem: tree.currentItem
    property var currentData

    property alias handle: tree.handle
    property alias contentItem: tree.contentItem
    property Component highlight: Rectangle {
        color: root.selectedColor
    }

    property alias selectionEnabled: tree.selectionEnabled
    property alias hoverEnabled: tree.hoverEnabled

    property alias color: tree.color
    property alias hoverColor: tree.hoverColor
    property alias selectedColor: tree.selectedColor
    property alias selectedItemColor: tree.selectedItemColor

    property alias rowHeight: tree.rowHeight
    property alias rowPadding: tree.rowPadding
    property alias rowSpacing: tree.rowSpacing

    enum Handle {
        Triangle,
        TriangleSmall,
        TriangleOutline,
        TriangleSmallOutline,
        Chevron,
        Arrow
    }

    property int handleStyle: PerfTreeView.Handle.Triangle

    contentHeight: tree.height
    contentWidth: width
    boundsBehavior: Flickable.StopAtBounds
    //ScrollBar.vertical: ScrollBar {}

    onCurrentIndexChanged: {
        if(currentIndex) {
            currentData = model.data(currentIndex)
        }
    }

    PerfTreeViewItem {
        id: tree

        model: root.model
        parentIndex: model.rootIndex()
        childCount: model.rowCount(parentIndex)

        itemLeftPadding: 0
        color: root.color
        hoverColor: root.hoverColor
        selectedColor: root.selectedColor
        selectedItemColor: root.selectedItemColor
        defaultIndicator: indicatorToString(handleStyle)
        z: 1

        Connections {
           target: root.model
           ignoreUnknownSignals: true
           function onLayoutChanged() {
               tree.childCount = root.model ? root.model.rowCount(tree.parentIndex) : 0
           }
        }
    }

    Loader {
        id: highlightLoader
        sourceComponent: highlight

        width: root.width
        height: root.rowHeight
        z: 0
        visible: root.selectionEnabled && tree.currentItem !== null

        Binding {
            target: highlightLoader.item
            property: "y"
            value: tree.currentItem ? tree.currentItem.mapToItem(tree, 0, 0).y + tree.anchors.topMargin : 0
            when: highlightLoader.status === Loader.Ready
        }
    }

    function indicatorToString(handle){
        switch (handle){
            case PerfTreeView.Handle.Triangle: return "▶";
            case PerfTreeView.Handle.TriangleSmall: return "►";
            case PerfTreeView.Handle.TriangleOutline: return "▷";
            case PerfTreeView.Handle.TriangleSmallOutline: return "⊳";
            case PerfTreeView.Handle.Chevron: return "❱";
            case PerfTreeView.Handle.Arrow: return "➤";
            default: return "▶";
        }
    }
}
