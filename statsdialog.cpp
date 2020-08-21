#include "statsdialog.h"
#include "ui_statsdialog.h"

#include "delegates.h"
#include "statsmodel.h"
#include <QSet>

StatsDialog::StatsDialog(QVector<Transfer> transfers, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StatsDialog),
    alls(transfers)
{
    setFixedSize(QSize(720, 540));
    ui->setupUi(this);
    setupTable();
    setupFilters();
}

void StatsDialog::setupFilters()
{
    ui->local->addItem("");
    ui->remote->addItem("");

    QSet<QString> set;
    for (auto t: alls) {
        if (set.contains(t.Src.Hostname())) {
            continue;
        }
        set += t.Src.Hostname();
        ui->local->addItem(t.Src.Hostname());
    }
    set.clear();
    for (auto t: alls) {
        if (set.contains(t.Dst.Hostname())) {
            continue;
        }
        set += t.Dst.Hostname();
        ui->remote->addItem(t.Dst.Hostname());
    }

    ui->local->setEnabled(alls.size() > 1);
    ui->remote->setEnabled(alls.size() > 1);

    connect(ui->reset, &QPushButton::clicked, this, &StatsDialog::onReset);
    connect(ui->local, &QComboBox::currentTextChanged, this, &StatsDialog::onChanged);
    connect(ui->remote, &QComboBox::currentTextChanged, this, &StatsDialog::onChanged);
}

void StatsDialog::setupTable()
{
    model = new StatsModel(alls);
    proxy = new StatsFilterProxyModel;
    proxy->setSourceModel(model);
    ui->table->setModel(proxy);

    ui->table->setItemDelegateForColumn(StatsModel::StatsModelField::Size, new SizeDelegate);
    ui->table->setItemDelegateForColumn(StatsModel::StatsModelField::Rate, new RateDelegate);
    ui->table->horizontalHeader()->setSectionResizeMode(StatsModel::StatsModelField::Local, QHeaderView::Stretch);
    ui->table->horizontalHeader()->setSectionResizeMode(StatsModel::StatsModelField::Remote, QHeaderView::Stretch);
}

void StatsDialog::onReset(bool checked)
{
    Q_UNUSED(checked);
    proxy->update("", "");
    ui->local->setCurrentIndex(0);
    ui->remote->setCurrentIndex(0);
}

void StatsDialog::onChanged(const QString &text)
{
    Q_UNUSED(text);
    proxy->update(ui->local->currentText(), ui->remote->currentText());
}

StatsDialog::~StatsDialog()
{
    delete ui;
}
