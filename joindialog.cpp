#include "joindialog.h"
#include "ui_joindialog.h"

#include <QNetworkInterface>

JoinDialog::JoinDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::JoinDialog)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);

    ui->nic->addItem("");
    for (auto ifi: QNetworkInterface::allInterfaces()) {
        ui->nic->addItem(ifi.humanReadableName());
    }
}

JoinDialog::~JoinDialog()
{
    delete ui;
}

QString JoinDialog::host() const
{
    return ui->host->text();
}

QString JoinDialog::interface() const
{
    return ui->nic->currentText();
}

quint16 JoinDialog::port() const {
    return ui->port->value();
}

bool JoinDialog::join() const
{
    return ui->join->isChecked();
}
