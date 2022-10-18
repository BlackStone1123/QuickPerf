#pragma once
#include <QObject>
#include <QString>
#include <QVariant>
#include <QJsonValue>

class JsonEntry
{
    Q_GADGET
    Q_PROPERTY(QString key MEMBER _key)
    Q_PROPERTY(QVariant value MEMBER _value)

public:
   JsonEntry();
   JsonEntry(const QString& key, const QVariant& value, QJsonValue::Type type);

   QString key() const;
   void setKey(QString key);

   QVariant value() const;
   void setValue(QVariant value);

   QJsonValue::Type type() const;
   void setType(QJsonValue::Type type);

   Q_INVOKABLE bool isObject() const;
   Q_INVOKABLE bool isArray() const;
   Q_INVOKABLE bool isValue() const;

private:
   QString _key;
   QVariant _value;
   QJsonValue::Type _type;
};

Q_DECLARE_METATYPE(JsonEntry)
