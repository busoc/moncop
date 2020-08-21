#include "copydialog.h"
#include "ui_copydialog.h"

CopyDialog::CopyDialog(const FileInfo &info, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CopyDialog)
{
    setAttribute(Qt::WA_DeleteOnClose);

    ui->setupUi(this);
    ui->file->setText(info.File);
    ui->curr->setText(formatSize(info.Curr));
    ui->size->setText(formatSize(info.Size));

    ui->local->setText(info.Src.toString());
    ui->remote->setText(info.Dst.toString());

    ui->time->setText(QString::number(info.Elapsed()));
    ui->rate->setText(formatRate(info.Rate()));
}

CopyDialog::~CopyDialog()
{
    delete ui;
}
