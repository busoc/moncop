#ifndef DELEGATES_H
#define DELEGATES_H

#include <QStyledItemDelegate>

class ProgressDelegate: public QStyledItemDelegate
{
public:
    ProgressDelegate(QObject *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

class SizeDelegate: public QStyledItemDelegate
{
public:
    SizeDelegate(QObject *parent = nullptr);
    QString displayText(const QVariant &value, const QLocale &locale) const override;
};

class RateDelegate: public QStyledItemDelegate
{
public:
    RateDelegate(QObject *parent = nullptr);
    QString displayText(const QVariant &value, const QLocale &locale) const override;
};

#endif // DELEGATES_H
