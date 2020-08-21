#include "delegates.h"

#include <QStyleOptionProgressBar>
#include <QApplication>

#include "control.h"

ProgressDelegate::ProgressDelegate(QObject *parent): QStyledItemDelegate(parent)
{
}

void ProgressDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    auto value = index.model()->data(index).toDouble();


    QStyleOptionProgressBar bar;
    bar.rect = option.rect;
    bar.minimum = 0;
    bar.maximum = 100;
    bar.progress = int(value*100);
    bar.textVisible = false;

    QApplication::style()->drawControl(QStyle::CE_ProgressBar, &bar, painter);
}

SizeDelegate::SizeDelegate(QObject *parent): QStyledItemDelegate(parent)
{
}

QString SizeDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    Q_UNUSED(locale)
    return formatSize(value.toDouble());
}

RateDelegate::RateDelegate(QObject *parent): QStyledItemDelegate(parent)
{
}

QString RateDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    Q_UNUSED(locale)
    return formatRate(value.toDouble());
}

