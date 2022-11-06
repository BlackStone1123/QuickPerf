#pragma once
#include <QAbstractItemModel>
#include <QString>
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
   void addTopLevelItem(TreeItem* child);
   void addItem(TreeItem* parent, TreeItem* child);
   void removeItem(TreeItem* item);

   TreeItem* rootItem() const;
   Q_INVOKABLE QModelIndex rootIndex();
   Q_INVOKABLE int depth(const QModelIndex& index) const;
   Q_INVOKABLE QString path(const QModelIndex& index) const;
   Q_INVOKABLE void clear();

private:
   TreeItem* internalPointer(const QModelIndex& index) const;

private:
   TreeItem* _rootItem;
};
