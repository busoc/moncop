#ifndef STATSDIALOG_H
#define STATSDIALOG_H

#include <QDialog>
#include <QVector>
#include "statsmodel.h"
#include "control.h"

namespace Ui {
class StatsDialog;
}

class StatsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StatsDialog(QVector<Transfer> transfers, QWidget *parent = nullptr);
    ~StatsDialog();

private slots:
    void onChanged(const QString &text);
    void onReset(bool checked = false);

private:
    Ui::StatsDialog *ui;
    StatsModel *model;
    StatsFilterProxyModel *proxy;

    QVector<Transfer> alls;

    void setupTable();
    void setupFilters();
};

#endif // STATSDIALOG_H
