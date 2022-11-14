#include "PerfListViewController.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

#include "../Json/JsonEntry.h"
#include "../DataModel/TreeItem.h"
#include "../DataModel/TreeModel.h"

namespace  {

//    void loadValue(const QJsonValue& value, TreeItem* parent)
//    {
//        if(value.isObject()) {
//           auto object = value.toObject();
//           for(auto it=object.begin(); it!=object.end(); ++it){
//              JsonEntry entry;
//              entry.setKey(it.key());
//              entry.setType(QJsonValue::Object);
//              if(it.value().isArray() || it.value().isObject()){
//                 auto child = new TreeItem(QVariant::fromValue(entry));
//                 parent->appendChild(child);
//                 loadValue(it.value(), child);
//              }
//              else {
//                 entry.setValue(it.value().toVariant());
//                 auto child = new TreeItem(QVariant::fromValue(entry));
//                 parent->appendChild(child);
//              }
//           }
//        }
//        else if(value.isArray()){
//           int index = 0;
//           auto array = value.toArray();
//           for(auto&& element: array){
//              JsonEntry entry;
//              entry.setKey("[" + QString::number(index) + "]");
//              entry.setType(QJsonValue::Array);
//              auto child = new TreeItem(QVariant::fromValue(entry));
//              parent->appendChild(child);
//              loadValue(element, child);
//              ++index;
//           }
//        }
//    }

//    void populateTree(TreeItem* rootItem)
//    {
//        QFile jsonFile;
//        jsonFile.setFileName("D:\\QuickPerf\\sample.json");

//        if(!jsonFile.open(QIODevice::ReadOnly | QIODevice::Text)){
//            qCritical() << "error: json file cannot be open";
//            return;
//        }

//        QJsonParseError error;
//        auto doc = QJsonDocument::fromJson(jsonFile.readAll(), &error);
//        qDebug() << "loading json file:" << error.errorString();

//        auto root = doc.isArray() ? QJsonValue(doc.array()) : QJsonValue(doc.object());
//        loadValue(root, rootItem);
//    }
}

PerfListViewController::PerfListViewController(QObject* parent)
    : QObject (parent)
{
//    TreeItem* root = new TreeItem;
//    populateTree(root);
//    mListModel = new TreeModel(root, this);
}
