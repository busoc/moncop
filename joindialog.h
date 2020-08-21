#ifndef JOINDIALOG_H
#define JOINDIALOG_H

#include <QDialog>

namespace Ui {
class JoinDialog;
}

class JoinDialog : public QDialog
{
    Q_OBJECT

public:
    explicit JoinDialog(QWidget *parent = nullptr);
    ~JoinDialog();

    QString host() const;
    QString interface() const;
    quint16 port() const;
    bool join() const;

private:
    Ui::JoinDialog *ui;
};

#endif // JOINDIALOG_H
