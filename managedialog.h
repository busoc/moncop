#ifndef MANAGEDIALOG_H
#define MANAGEDIALOG_H

#include <QDialog>
#include <QVector>
#include <QAbstractTableModel>
#include <QSortFilterProxyModel>
#include "control.h"

namespace Ui {
class ManageDialog;
}

class ManageGroupInfoModel;
class ManageProxyModel;

class ManageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ManageDialog(QVector<GroupInfo*> groups, QWidget *parent = nullptr);
    ~ManageDialog();

public slots:
    void selectionChanged(const QModelIndex &curr, const QModelIndex &prev);
    void joinGroup(bool checked = false);
    void leaveGroup(bool checked = false);
    void removeGroup(bool checked = false);
    void closeDialog(bool checked = false);

    void onHostChanged(const QString &text);
    void onJoinChanged(int state);

private:
    Ui::ManageDialog *ui;
    ManageGroupInfoModel *model;
    ManageProxyModel *proxy;

    void toggleButtons(GroupInfo *gi);
};

class ManageGroupInfoModel: public QAbstractTableModel
{
public:
    ManageGroupInfoModel(QVector<GroupInfo*> groups, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    GroupInfo* at(const QModelIndex &index);
    void remove(const QModelIndex &index);

private:
    QVector<GroupInfo*> groups;
};

class ManageProxyModel: public QSortFilterProxyModel
{
public:
    void update(QString str, Qt::CheckState st);
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:
    QString host = "";
    Qt::CheckState state = Qt::PartiallyChecked;
};


#endif // MANAGEDIALOG_H
