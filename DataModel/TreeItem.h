#pragma once
#include <QVariant>
#include <QVector>

class TreeItem
{
   friend class TreeModel;

public:
   //! Instance a tree item with empty data.
   TreeItem();

   //! Instance a tree with the input data.
   explicit TreeItem(const QVariant& data);

   //! Destroy the item and all its children.
   ~TreeItem();

   //! Return the internal data.
   const QVariant& data() const;

   //! Set the internal data.
   void setData(const QVariant& data);

   //! Return the number of children of the item.
   int childCount() const;

   //! Return the number of the row referred to the parent item.
   int row() const;

   //! Return true if the item has no children.
   bool isLeaf() const;

   //! Return the depth of the item in the hierarchy.
   int depth() const;

   void appendChild(TreeItem* item);
   void removeChild(TreeItem* item);

private:
   TreeItem* parentItem();
   void setParentItem(TreeItem* parentItem);

   TreeItem* child(int row);

private:
   QVariant _itemData;
   TreeItem* _parentItem;
   QVector<TreeItem*> _childItems;
};
