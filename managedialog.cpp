#include "managedialog.h"
#include "ui_managedialog.h"

#include <QMessageBox>
#include <QLineEdit>

ManageDialog::ManageDialog(QVector<GroupInfo*> groups, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ManageDialog)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);

    ui->joined->setCheckState(Qt::PartiallyChecked);
    ui->host->addItem("");
    for (auto gi: groups) {
        ui->host->addItem(gi->Host().toString());
    }

    model = new ManageGroupInfoModel(groups);
    proxy = new ManageProxyModel;
    proxy->setSourceModel(model);
    ui->groups->setModel(proxy);
    ui->groups->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);

    connect(ui->host, &QComboBox::currentTextChanged, this, &ManageDialog::onHostChanged);
    connect(ui->joined, &QCheckBox::stateChanged, this, &ManageDialog::onJoinChanged);

    connect(ui->join, &QPushButton::clicked, this, &ManageDialog::joinGroup);
    connect(ui->leave, &QPushButton::clicked, this, &ManageDialog::leaveGroup);
    connect(ui->remove, &QPushButton::clicked, this, &ManageDialog::removeGroup);
    connect(ui->close, &QPushButton::clicked, this, &ManageDialog::closeDialog);
    connect(ui->groups->selectionModel(), &QItemSelectionModel::currentChanged, this, &ManageDialog::selectionChanged);
}

ManageDialog::~ManageDialog()
{
    delete ui;
}

void ManageDialog::onHostChanged(const QString &str)
{
    proxy->update(str, ui->joined->checkState());
}

void ManageDialog::onJoinChanged(int state)
{
    Q_UNUSED(state)
    proxy->update(ui->host->currentText(), ui->joined->checkState());
}

void ManageDialog::closeDialog(bool checked)
{
    Q_UNUSED(checked);
    done(QDialog::Accepted);
}

void ManageDialog::removeGroup(bool checked)
{
    Q_UNUSED(checked)
    auto gi = model->at(ui->groups->selectionModel()->currentIndex());
    gi->Leave();

    model->remove(ui->groups->selectionModel()->currentIndex());
}

void ManageDialog::joinGroup(bool checked)
{
    Q_UNUSED(checked)
    auto index = ui->groups->selectionModel()->currentIndex();
    auto gi = model->at(index);
    if (!gi->Join()) {
        QMessageBox::warning(this, "network error!", "fail to join multicast group");
        return ;
    }
    toggleButtons(gi);
}

void ManageDialog::leaveGroup(bool checked)
{
    Q_UNUSED(checked)
    auto index = ui->groups->selectionModel()->currentIndex();
    auto gi = model->at(index);
    if (!gi->Leave()) {
        QMessageBox::warning(this, "network error!", "fail to leavemulticast group");
        return ;
    }
    toggleButtons(gi);
}

void ManageDialog::selectionChanged(const QModelIndex &curr, const QModelIndex &prev)
{
    Q_UNUSED(prev)
    toggleButtons(model->at(curr));
}

void ManageDialog::toggleButtons(GroupInfo *gi)
{
    ui->join->setEnabled(!gi->Enabled());
    ui->leave->setEnabled(gi->Enabled());
}

ManageGroupInfoModel::ManageGroupInfoModel(QVector<GroupInfo*> groups, QObject *parent):
    QAbstractTableModel(parent),
    groups(groups) {}

int ManageGroupInfoModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : groups.size();
}


int ManageGroupInfoModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : 2;
}

Qt::ItemFlags ManageGroupInfoModel::flags(const QModelIndex &index) const
{
    if (index.column() == 1) {
        return Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
    }
    return QAbstractTableModel::flags(index);
}

QVariant ManageGroupInfoModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    auto gi = groups[index.row()];
    if (role == Qt::DisplayRole && index.column() == 0) {
        return QString("%1:%2").arg(gi->Host().toString()).arg(gi->Port());
    }
    if (role == Qt::CheckStateRole && index.column() == 1) {
        return gi->Enabled() ? Qt::Checked : Qt::Unchecked;
    }
    return QVariant();
}

QVariant ManageGroupInfoModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0:
            return "group";
        case 1:
            return "joined";
        }
    }
    return QVariant();
}

GroupInfo* ManageGroupInfoModel::at(const QModelIndex &index)
{
    return groups.at(index.row());
}

void ManageGroupInfoModel::remove(const QModelIndex &index)
{

}

void ManageProxyModel::update(QString str, Qt::CheckState st)
{
    host = str;
    state = st;
    invalidateFilter();
}

bool ManageProxyModel::filterAcceptsRow(int row, const QModelIndex &parent) const
{
    auto ix1 = sourceModel()->index(row, 0, parent);
    auto ix2 = sourceModel()->index(row, 1, parent);

    auto data = sourceModel();
    auto ok1 = data->data(ix1).toString() == host || host == "";
    if (state == Qt::PartiallyChecked) {
        return ok1;
    }
    auto ok2 = data->data(ix2, Qt::CheckStateRole) == state;
    return ok1 && ok2;
}


