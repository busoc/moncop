#include "statsmodel.h"

void StatsFilterProxyModel::update(QString src, QString dst)
{
    source = src;
    remote = dst;
    invalidateFilter();
}

bool StatsFilterProxyModel::filterAcceptsRow(int row, const QModelIndex &parent) const
{
    if (source == "" && remote == "") {
        return true;
    }
    auto model = sourceModel();
    auto ok1 = source == "";
    if (source != "") {
        auto i = model->index(row, StatsModel::StatsModelField::Local, parent);
        ok1 = model->data(i).toString().startsWith(source);
        if (remote == "") {
            return ok1;
        }
    }
    auto ok2 = remote == "";
    if (remote != "") {
        auto i = model->index(row, StatsModel::StatsModelField::Remote, parent);
        ok2 = model->data(i).toString().startsWith(remote);
        if (source == "") {
            return ok2;
        }
    }
    return ok1 && ok2;
}

StatsModel::StatsModel(QVector<Transfer> transfers, QObject *parent):
    QAbstractTableModel(parent),
    transfers(transfers)
{

}

QVariant StatsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
        case StatsModelField::Local:
            return "Local";
        case StatsModelField::Remote:
            return "Remote";
        case StatsModelField::Rate:
            return "Rate";
        case StatsModelField::Size:
            return "Size";
        case StatsModelField::File:
            return "File";
        }
    }
    return QVariant();
}

int StatsModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : transfers.size();
}

int StatsModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : 5;
}

QVariant StatsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0) {
        return QVariant();
    }
    if (role == Qt::DisplayRole) {
        auto t = transfers.at(index.row());
        switch (index.column()) {
        case StatsModelField::Local:
            return t.Src.toString();
        case StatsModelField::Remote:
            return t.Dst.toString();
        case StatsModelField::Rate:
            return t.Rate();
        case StatsModelField::Size:
            return t.Size;
        case StatsModelField::File:
            return t.File;
        }
    }
    if (role == Qt::TextAlignmentRole) {
        switch(index.column()) {
        case StatsModelField::Rate:
        case StatsModelField::Size:
        case StatsModelField::File:
            return Qt::AlignCenter;
        default:
            return Qt::AlignVCenter;
        }
    }
    return QVariant();
}
