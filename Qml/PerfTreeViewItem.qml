import QtQuick 2.12
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

Item {
   id: root

   // model properties

   property var model
   property var parentIndex
   property var childCount

   property var currentItem: null
   property var selectedIndex: null
   property var hoveredIndex: null

   // layout properties

   property bool selectionEnabled: false
   property bool hoverEnabled: false

   property int itemLeftPadding: 0
   property int rowHeight: 24
   property int rowPadding: 30
   property int rowSpacing: 6

   property color color: "black"
   property color handleColor: color
   property color hoverColor: "lightgray"
   property color selectedColor: "silver"
   property color selectedItemColor: color

   property string defaultIndicator: "▶"

   implicitWidth: parent.width
   implicitHeight: childrenRect.height

   // Components

   property Component handle: Rectangle {
      id: handle

      implicitWidth: 15
      implicitHeight: 15
      Layout.leftMargin: parent.spacing
      rotation: currentRow.expanded ? 90 : 0
      opacity: currentRow.hasChildren
      color: "transparent"

      Text {
         anchors.centerIn: parent
         text: defaultIndicator
         antialiasing: true
         color: currentRow.isSelectedIndex ? root.selectedItemColor : root.handleColor
      }
   }

   property Component contentItem: Text {
      id: contentData

      anchors.verticalCenter: parent.verticalCenter
      verticalAlignment: Text.AlignVCenter

      color: currentRow.isSelectedIndex ? root.selectedItemColor : root.color
      text: currentRow.currentData
      font: root.font
   }

   property Component hoverComponent: Rectangle {
      width: parent.width
      height: parent.height
      color: currentRow.isHoveredIndex && !currentRow.isSelectedIndex ? root.hoverColor : "transparent"
   }

   // Body

   ColumnLayout {
      id: parentColumn

      width: parent.width
      spacing: 0

      Repeater {
         id: repeater
         model: childCount
         Layout.fillWidth: true

         delegate: ColumnLayout {
            id: itemColumn

            Layout.leftMargin: itemLeftPadding
            spacing: 0

            QtObject {
               id: _prop

               property var currentIndex: root.model.index(index, 0, parentIndex)
               property var currentData: root.model.data(currentIndex)
               property Item currentItem: repeater.itemAt(index)
               property bool expanded: false
               property bool selected: false
               property int itemChildCount: root.model.rowCount(currentIndex)
               readonly property int depth: root.model.depth(currentIndex)
               readonly property bool hasChildren: itemChildCount > 0
               readonly property bool isSelectedIndex: root.selectionEnabled && currentIndex === root.selectedIndex
               readonly property bool isHoveredIndex: root.hoverEnabled && currentIndex === root.hoveredIndex
               readonly property bool isSelectedAndHoveredIndex: hoverEnabled && selectionEnabled && isHoveredIndex && isSelectedIndex

               function toggle(){ if(_prop.hasChildren) _prop.expanded = !_prop.expanded }

            }

            Connections {
               target: root.model
               ignoreUnknownSignals: true
               function onLayoutChanged() {
                   const parent = root.model.index(model.index, 0, parentIndex)
                  _prop.itemChildCount = root.model.rowCount(parent)
               }
            }

            Item {
               id: column

               Layout.fillWidth: true

               width: row.implicitWidth
               height: Math.max(row.implicitHeight, root.rowHeight)

               RowLayout {
                  id: row

                  anchors.fill: parent
                  Layout.fillHeight: true

                  z: 1
                  spacing: root.rowSpacing

                  // handle
                  Loader {
                     id: indicatorLoader

                     property QtObject currentRow: _prop
                     Layout.leftMargin: parent.spacing

                     sourceComponent: handle
                     TapHandler { onSingleTapped: _prop.toggle() }
                  }

                  //  Content
                  Loader {
                     id: contentItemLoader

                     property QtObject currentRow: _prop

                     Layout.fillWidth: true
                     height: rowHeight

                     sourceComponent: contentItem

                     Connections {
                        target: root.model
                        ignoreUnknownSignals: true
                        function onDataChanged(modelIndex) {
                           const changedId = modelIndex.internalId
                           const currentId = _prop.currentIndex.internalId
                           if(changedId === currentId){
                               contentItemLoader.currentRow.currentData = root.model.data(modelIndex);
                           }
                        }
                     }
                  }

                  TapHandler {
                     onDoubleTapped: _prop.toggle()
                     onSingleTapped: {
                        root.currentItem = _prop.currentItem
                        root.selectedIndex = _prop.currentIndex
                     }
                  }
               }

               Loader {
                  id: hoverLoader
                  sourceComponent: hoverComponent

                  width: row.width + (1 + _prop.depth * rowPadding)
                  height: parent.height

                  x: -(_prop.depth * rowPadding)
                  z: 0

                  clip: false

                  property QtObject currentRow: _prop

                  HoverHandler {
                     onHoveredChanged: {
                        if(root.hoverEnabled){
                           if(hovered && root.hoveredIndex !== _prop.currentIndex)
                              root.hoveredIndex = _prop.currentIndex
                           if(!hovered && root.hoveredIndex === _prop.currentIndex)
                              root.hoveredIndex = null
                        }
                     }
                  }
               }

            }

            // loader to populate the children row for each node
            Loader {
               id: loader

               Layout.fillWidth: true

               source: "PerfTreeViewItem.qml"
               visible: _prop.expanded

               onLoaded: {
                  item.model = root.model
                  item.parentIndex = _prop.currentIndex
                  item.childCount = _prop.itemChildCount
               }

               Connections {
                  target: root.model
                  ignoreUnknownSignals: true
                  function onLayoutChanged() {
                     const parent = root.model.index(index, 0, parentIndex)
                     loader.item.childCount = root.model.rowCount(parent)
                  }
               }

               Binding { target: loader.item; property: "model"; value: root.model; when: loader.status == Loader.Ready }
               Binding { target: loader.item; property: "handle"; value: root.handle; when: loader.status == Loader.Ready }
               Binding { target: loader.item; property: "contentItem"; value: root.contentItem; when: loader.status == Loader.Ready }

               Binding { target: loader.item; property: "currentItem"; value: root.currentItem; when: loader.status == Loader.Ready }
               Binding { target: loader.item; property: "selectedIndex"; value: root.selectedIndex; when: loader.status == Loader.Ready }
               Binding { target: root; property: "currentItem"; value: loader.item.currentItem; when: loader.status == Loader.Ready }
               Binding { target: root; property: "selectedIndex"; value: loader.item.selectedIndex; when: loader.status == Loader.Ready }

               Binding { target: loader.item; property: "color"; value: root.color; when: loader.status == Loader.Ready }
               Binding { target: loader.item; property: "handleColor"; value: root.handleColor; when: loader.status == Loader.Ready }
               Binding { target: loader.item; property: "hoverEnabled"; value: root.hoverEnabled; when: loader.status == Loader.Ready }
               Binding { target: loader.item; property: "hoverColor"; value: root.hoverColor; when: loader.status == Loader.Ready }
               Binding { target: loader.item; property: "selectionEnabled"; value: root.selectionEnabled; when: loader.status == Loader.Ready }
               Binding { target: loader.item; property: "selectedColor"; value: root.selectedColor; when: loader.status == Loader.Ready }
               Binding { target: loader.item; property: "selectedItemColor"; value: root.selectedItemColor; when: loader.status == Loader.Ready }

               Binding { target: loader.item; property: "itemLeftPadding"; value: root.rowPadding; when: loader.status == Loader.Ready }
               Binding { target: loader.item; property: "rowHeight"; value: root.rowHeight; when: loader.status == Loader.Ready }
               Binding { target: loader.item; property: "rowPadding"; value: root.rowPadding; when: loader.status == Loader.Ready }
               Binding { target: loader.item; property: "rowSpacing"; value: root.rowSpacing; when: loader.status == Loader.Ready }
            }
         }
      }
   }
}
