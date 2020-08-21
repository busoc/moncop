#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkDatagram>
#include <QLabel>
#include <QVector>
#include <QCloseEvent>

#include "fileinfomodel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    virtual void closeEvent(QCloseEvent *e);

private slots:
    void onJoin();
    void onManage();
    void onReset();
    void onExit();
    void onFilter();
    void onStats();
    void onFullscreen();
    void onDetail(const QModelIndex &index);
    void onRecv(FileInfo &fi);

private:
    Ui::MainWindow *ui;
    QLabel *totalFiles;
    QLabel *totalBytes;
    QLabel *totalRate;

    FileInfoModel *fimodel;

    QVector<GroupInfo*> groups;
    QVector<Transfer> transfers;

    void setupTable();
    void setupActions();
    void setupStatus();
    void setupToolbars();
    void updateStatus();

    void update(FileInfo &info);

    void readSettings();
    void writeSettings();
};
#endif // MAINWINDOW_H
