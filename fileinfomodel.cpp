#include <algorithm>
#include <QDateTime>
#include <QDebug>

#include "fileinfomodel.h"

FileInfoModel::FileInfoModel(QObject *parent): QAbstractTableModel(parent)
{
}

QVariant FileInfoModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
        case FileInfoField::File:
            return "File";
        case FileInfoField::Current:
            return "Current";
        case FileInfoField::Size:
            return "Size";
        case FileInfoField::Progress:
            return "Progress";
        case FileInfoField::Local:
            return "Local";
        case FileInfoField::Remote:
            return "Remote";
        case FileInfoField::Rate:
            return "Rate";
        default:
            return QVariant();
        }
    }
    return QVariant();
}

int FileInfoModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : infos.size();
}

int FileInfoModel::columnCount(const QModelIndex &parent) const
{
     return parent.isValid() ? 0 : 7;
}

QVariant FileInfoModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= infos.size() || index.row() < 0) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        auto fi = infos.at(index.row());
        switch (index.column()) {
        case FileInfoField::File:
            return fi.File;
        case FileInfoField::Current:
            return fi.Curr;
        case FileInfoField::Size:
            return fi.Size;
        case FileInfoField::Progress:
            return fi.Percent();
        case FileInfoField::Local:
            return fi.Src.toString();
        case FileInfoField::Remote:
            return fi.Dst.toString();
        case FileInfoField::Rate:
            return fi.Rate();
        }
    }
    if (role == Qt::TextAlignmentRole) {
        switch(index.column()) {
        case FileInfoField::Current:
        case FileInfoField::Size:
        case FileInfoField::Rate:
            return Qt::AlignCenter;
        default:
            return Qt::AlignVCenter;
        }
    }
    return QVariant();
}

bool FileInfoModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole){
        auto fi = infos.at(index.row());
        switch (index.column()) {
        case FileInfoField::File:
            fi.File = value.toString();
            break;
        case FileInfoField::Current:
            fi.Update(value.toUInt());
            break;
        case FileInfoField::Size:
            fi.Size = value.toInt();
            break;
        case FileInfoField::Local:
            fi.Src = value.value<Addr>();
            break;
        case FileInfoField::Remote:
            fi.Dst = value.value<Addr>();
            break;
        }

        infos.replace(index.row(), fi);
        emit dataChanged(index, index.siblingAtColumn(6), {Qt::DisplayRole, Qt::EditRole});

        return true;
    }
    return false;
}

bool FileInfoModel::insertRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent)
    beginInsertRows(QModelIndex(), row, row + count - 1);
    for (int i = 0; i < count; i++) {
        FileInfo fi;
        infos.push_back(fi);
    }

    endInsertRows();
    return true;
}

bool FileInfoModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent)
    beginRemoveRows(QModelIndex(), row, row + count - 1);
    for (int i = row; i < count; i++) {
        infos.removeAt(row);
    }
    endRemoveRows();
    return true;
}

void FileInfoModel::reset()
{
    if (!infos.size()) {
        return ;
    }
    removeRows(0, infos.size(), QModelIndex());
}

QModelIndex FileInfoModel::update(FileInfo &fi)
{
    if (fi.Size == 0) {
        return QModelIndex();
    }
    bool old = false;
    int row = 0;
    if (infos.size() > 0 && infos.last() == fi) {
        row = infos.size()-1;
        old = true;
    } else {
        size += fi.Size;
        row = infos.size();
        insertRows(row, 1, QModelIndex());
    }

    QModelIndex ix;
    ix = index(row, FileInfoField::File, QModelIndex());
    setData(ix, fi.File, Qt::EditRole);
    ix = index(row, FileInfoField::Current, QModelIndex());
    setData(ix, fi.Curr, Qt::EditRole);
    ix = index(row, FileInfoField::Size, QModelIndex());
    setData(ix, fi.Size, Qt::EditRole);
    ix = index(row, FileInfoField::Local, QModelIndex());
    setData(ix, QVariant::fromValue(fi.Src), Qt::EditRole);
    ix = index(row, FileInfoField::Remote, QModelIndex());
    setData(ix, QVariant::fromValue(fi.Dst), Qt::EditRole);

    return old ? QModelIndex() : ix;
}

FileInfo FileInfoModel::at(const QModelIndex &index)
{
    return infos.at(index.row());
}

int FileInfoModel::files() {
    return infos.size();
}

double FileInfoModel::bytes() {
    return size;
}

double FileInfoModel::rate()
{
    auto now = QDateTime::currentDateTime();
    auto secs = when.secsTo(now);
    if (!secs) {
        return 0;
    }
    return size / secs;
}
