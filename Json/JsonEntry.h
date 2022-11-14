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
    Q_PROPERTY(QString type MEMBER _type)

public:
   JsonEntry();
   JsonEntry(const QString& key, const QVariant& value, QJsonValue::Type type);

   QString key() const;
   void setKey(const QString& key);

   QVariant value() const;
   void setValue(const QVariant& value);

   QString type() const;
   void setType(const QString& type);

   Q_INVOKABLE bool isObject() const;
   Q_INVOKABLE bool isArray() const;
   Q_INVOKABLE bool isValue() const;

private:
   QString _key;
   QVariant _value;
   QString _type;
};

Q_DECLARE_METATYPE(JsonEntry)
