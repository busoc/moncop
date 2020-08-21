#ifndef FILEINFOMODEL_H
#define FILEINFOMODEL_H

#include <QAbstractTableModel>
#include <QVector>

#include "control.h"

enum FileInfoField {
    File,
    Current,
    Size,
    Progress,
//    Time,
//    Done,
    Local,
    Remote,
    Rate,
};

class FileInfoModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit FileInfoModel(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    void reset();
    QModelIndex update(FileInfo &fi);
    int files();
    double bytes();
    double rate();
    FileInfo at(const QModelIndex &index);

private:
    QVector<FileInfo> infos;
    double size;
    QDateTime when = QDateTime::currentDateTime();
};

#endif // FILEINFOMODEL_H
