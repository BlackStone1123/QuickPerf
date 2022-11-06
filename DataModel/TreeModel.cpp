#include "TreeModel.h"

TreeModel::TreeModel(TreeItem* root, QObject* parent)
   : QAbstractItemModel(parent),
     _rootItem{root}
{
}

TreeModel::~TreeModel()
{
   delete _rootItem;
}

int TreeModel::rowCount(const QModelIndex& parent) const
{
   if (!parent.isValid()){
      return _rootItem->childCount();
   }

   return internalPointer(parent)->childCount();
}

int TreeModel::columnCount(const QModelIndex&  /*parent*/) const
{
   // This is basically flatten as a list model
   return 1;
}

QModelIndex TreeModel::index(const int row, const int column, const QModelIndex& parent) const
{
   if (!hasIndex(row, column, parent)){
      return {};
   }

   TreeItem* item = _rootItem;
   if (parent.isValid()){
      item = internalPointer(parent);
   }

   if (auto child = item->child(row)){
      return createIndex(row, column, child);
   }

   return {};
}

QModelIndex TreeModel::parent(const QModelIndex& index) const
{
   if (!index.isValid()){
      return {};
   }

   TreeItem* childItem = internalPointer(index);
   TreeItem* parentItem = childItem->parentItem();

   if (!parentItem){
      return {};
   }

   if (parentItem == _rootItem){
      return {};
   }

   return createIndex(parentItem->row(), 0, parentItem);
}

QVariant TreeModel::data(const QModelIndex& index, const int role) const
{
   if (!index.isValid()) {
      return QVariant();
   }

   return internalPointer(index)->data();
}

bool TreeModel::setData(const QModelIndex& index, const QVariant& value, int /*role*/)
{
   if(!index.isValid()){
      return false;
   }

   if(auto item = internalPointer(index)){
      item->setData(value);
      emit dataChanged(index, index, {Qt::EditRole});
   }

   return false;
}

void TreeModel::addTopLevelItem(TreeItem* child)
{
   if(child){
      addItem(_rootItem, child);
   }
}

void TreeModel::addItem(TreeItem* parent, TreeItem* child)
{
   if(!child || !parent){
      return;
   }

   emit layoutAboutToBeChanged();

//   if (child->parentItem()) {
//      beginRemoveRows(QModelIndex(), child->parentItem()->childCount() - 1, child->parentItem()->childCount());
//      child->parentItem()->removeChild(child);
//      endRemoveRows();
//   }

   beginInsertRows(QModelIndex(), parent->childCount(), parent->childCount());
   child->setParentItem(parent);
   parent->appendChild(child);
   endInsertRows();

   emit layoutChanged();
}

void TreeModel::removeItem(TreeItem* item)
{
   if(!item){
      return;
   }

   emit layoutAboutToBeChanged();

   if (item->parentItem()) {
      beginRemoveRows(QModelIndex(), item->parentItem()->childCount() - 1, item->parentItem()->childCount());
      item->parentItem()->removeChild(item);
      endRemoveRows();
   }

   emit layoutChanged();
}

TreeItem* TreeModel::rootItem() const
{
   return _rootItem;
}

QModelIndex TreeModel::rootIndex()
{
   return {};
}

int TreeModel::depth(const QModelIndex& index) const
{
   int count = 0;
   auto anchestor = index;
   if(!index.isValid()){
      return 0;
   }
   while(anchestor.parent().isValid()){
      anchestor = anchestor.parent();
      ++count;
   }

   return count;
}

QString TreeModel::path(const QModelIndex& index) const
{
    QString res;
    QModelIndex anchestor = index;

    while (anchestor.isValid()) {
        res.append("_" + QString::number(anchestor.row()));
        anchestor = anchestor.parent();
    }
    return res;
}
void TreeModel::clear()
{
   emit layoutAboutToBeChanged();
   beginResetModel();
   delete _rootItem;
   _rootItem = new TreeItem();
   endResetModel();
   emit layoutChanged();
}

TreeItem* TreeModel::internalPointer(const QModelIndex& index) const
{
   return static_cast<TreeItem* >(index.internalPointer());
}
