#pragma once
#include <QObject>

class TreeModel;
class PerfListViewController: public QObject
{
    Q_OBJECT
    Q_PROPERTY(TreeModel* listModel MEMBER mListModel NOTIFY listModelChanged)

public:
    PerfListViewController(QObject* parent = nullptr);

signals:
    void listModelChanged();

private:
    TreeModel* mListModel {nullptr};
};

