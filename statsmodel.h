#ifndef STATSMODEL_H
#define STATSMODEL_H

#include <QAbstractTableModel>
#include <QSortFilterProxyModel>
#include <QVector>

#include "control.h"

class StatsFilterProxyModel: public QSortFilterProxyModel
{
public:
    bool filterAcceptsRow(int row, const QModelIndex &parent) const override;
    void update(QString src, QString dst);
private:
    QString source;
    QString remote;
};

class StatsModel: public QAbstractTableModel
{
public:
    enum StatsModelField {
        Local,
        Remote,
        Rate,
        File,
        Size,
    };

    explicit StatsModel(QVector<Transfer> transfers, QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    //bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    // Add data:
    //bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    //bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

private:
    QVector<Transfer> transfers;
};

#endif // STATSMODEL_H
