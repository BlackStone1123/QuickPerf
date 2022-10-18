#pragma once
#include <QAbstractItemModel>
#include "TreeItem.h"

/*!
 * Generic Tree Model.
 */
class TreeModel : public QAbstractItemModel
{
   Q_OBJECT

public:
   explicit TreeModel(TreeItem* root, QObject* parent = nullptr);
   ~TreeModel() override;

public:
   int rowCount(const QModelIndex& index) const override;
   int columnCount(const QModelIndex& index) const override;

   QModelIndex index(int row, int column, const QModelIndex& parent) const override;
   QModelIndex parent(const QModelIndex& childIndex) const override;

   QVariant data(const QModelIndex& index, int role = 0) const override;
   bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

public:
   //! Add an item to the top level.
   void addTopLevelItem(TreeItem* child);

   //! Add the item child to the parent node.
   void addItem(TreeItem* parent, TreeItem* child);

   //! Remove the item and all its children.
   void removeItem(TreeItem* item);

   //! Return the root item.
   TreeItem* rootItem() const;

   //! Return the root Model Index. Needed for the QML side.
   Q_INVOKABLE QModelIndex rootIndex();

   //! Return the depth of the item for the selected index.
   Q_INVOKABLE int depth(const QModelIndex& index) const;

   //! Remove all the elements from the tree.
   Q_INVOKABLE void clear();

private:
   TreeItem* internalPointer(const QModelIndex& index) const;

private:
   TreeItem* _rootItem;
};
