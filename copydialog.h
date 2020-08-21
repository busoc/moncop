#ifndef COPYDIALOG_H
#define COPYDIALOG_H

#include <QDialog>
#include "control.h"

namespace Ui {
class CopyDialog;
}

class CopyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CopyDialog(const FileInfo &info, QWidget *parent = nullptr);
    ~CopyDialog();

private:
    Ui::CopyDialog *ui;
};

#endif // COPYDIALOG_H
