#include "filterdialog.h"
#include "ui_filterdialog.h"

FilterDialog::FilterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FilterDialog)
{
    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);
}

FilterDialog::~FilterDialog()
{
    delete ui;
}
