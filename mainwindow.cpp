#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QToolBar>
#include <QSettings>

#include "control.h"
#include "delegates.h"
#include "copydialog.h"
#include "joindialog.h"
#include "managedialog.h"
#include "filterdialog.h"
#include "statsdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupTable();
    setupStatus();
    setupActions();
    //setupToolbars();

    readSettings();
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    Q_UNUSED(e)
    writeSettings();
}

void MainWindow::setupStatus()
{
    totalRate = new QLabel;
    totalBytes = new QLabel;
    totalFiles = new QLabel;

    totalRate->setText("N/A");
    statusBar()->addPermanentWidget(totalRate);
    totalBytes->setText("N/A");
    statusBar()->addPermanentWidget(totalBytes);
    totalFiles->setText("N/A");
    statusBar()->addPermanentWidget(totalFiles);
}

void MainWindow::setupActions()
{
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::onExit);
    connect(ui->actionReset, &QAction::triggered, this, &MainWindow::onReset);
    connect(ui->actionJoin, &QAction::triggered, this, &MainWindow::onJoin);
    connect(ui->actionManage, &QAction::triggered, this, &MainWindow::onManage);
    connect(ui->actionFilter, &QAction::triggered, this, &MainWindow::onFilter);
    connect(ui->actionStats, &QAction::triggered, this, &MainWindow::onStats);
    connect(ui->actionFullscreen, &QAction::triggered, this, &MainWindow::onFullscreen);

    ui->actionScroll->setChecked(true);
}

void MainWindow::onDetail(const QModelIndex &index)
{
    CopyDialog *dialog = new CopyDialog(fimodel->at(index), this);
    dialog->exec();
}

void MainWindow::onReset()
{
    fimodel->reset();
    transfers.clear();
    updateStatus();
}

void MainWindow::onExit()
{
    writeSettings();
    QApplication::exit();
}

void MainWindow::onJoin()
{
    auto dialog = new JoinDialog;
    if (dialog->exec() == QDialog::DialogCode::Rejected) {
        return ;
    }
    auto host = dialog->host();
    auto port = dialog->port();
    auto nic = dialog->interface();
    auto join = dialog->join();

    auto group = QHostAddress(host);
    auto got = [group, port](GroupInfo *gi) {
        return group == gi->Host() && port == gi->Port();
    };

    auto it = std::find_if(groups.begin(), groups.end(), got);
    if (it != groups.end()) {
        QMessageBox::warning(this, "network error!", "given host:port already registered!");
        return ;
    }

    GroupInfo *g;
    try {
        g = new GroupInfo(host, port, nic);
        connect(g, &GroupInfo::recv, this, &MainWindow::onRecv);
    } catch (GroupException &e) {
        QMessageBox box;
        box.setIcon(QMessageBox::Critical);
        box.setWindowTitle("network error!");
        box.setText("unable to subscribe to given multicast group");
        box.setDetailedText(e.errorSring());
        box.setStandardButtons(QMessageBox::Ok);
        box.exec();
        return ;
    }

    groups.push_back(g);
    if (!join) {
        return ;
    }
    if (!g->Join()) {
        QMessageBox::warning(this, "network error!", "fail to join multicast group!");
    }
}

void MainWindow::onRecv(FileInfo &fi)
{
    auto ix = fimodel->update(fi);
    if (ui->actionScroll->isChecked() && ix.isValid()) {
        ui->table->scrollTo(ix);
    }
    update(fi);
    if (!ix.isValid()) {
        return;
    }
    updateStatus();
}

void MainWindow::onManage()
{
    auto dialog = new ManageDialog(groups);
    dialog->exec();
}

void MainWindow::onStats()
{
    QDialog *stats = new StatsDialog(transfers, this);
    stats->exec();
}

void MainWindow::onFilter()
{
    auto dialog = new FilterDialog;
    dialog->exec();
}

void MainWindow::onFullscreen()
{

}

void MainWindow::update(FileInfo &info)
{
    if (!info.Complete()) {
        return ;
    }
    auto got = [info](const Transfer t) {
        return t.Dst == info.Dst && t.Src == info.Src;
    };
    auto it = std::find_if(transfers.begin(), transfers.end(), got);
    int ix = 0;
    if (it != transfers.end()) {
        ix = it - transfers.begin();
    } else {
        Transfer t{
            .File = 0,
            .Size = 0,
            .Src = info.Src,
            .Dst = info.Dst,
            .Starts = QDateTime::currentDateTime(),
            .Ends = QDateTime::currentDateTime(),
        };
        ix = transfers.size();
        transfers.push_back(t);
    }
    transfers[ix].Update(info.Size);
}

void MainWindow::setupToolbars()
{
    QToolBar *bar1 = new QToolBar;
    bar1->addAction(ui->actionJoin);
    bar1->addAction(ui->actionManage);
    bar1->addSeparator();
    bar1->addAction(ui->actionFilter);
    bar1->addAction(ui->actionStats);
    addToolBar(Qt::ToolBarArea::TopToolBarArea, bar1);

    QToolBar *bar2 = new QToolBar;
    bar2->addAction(ui->actionScroll);
    bar2->addAction(ui->actionFullscreen);
    addToolBar(Qt::ToolBarArea::TopToolBarArea, bar2);

}

void MainWindow::setupTable()
{
    fimodel = new FileInfoModel;
    ui->table->setModel(fimodel);

    ui->table->setItemDelegateForColumn(FileInfoField::Progress, new ProgressDelegate);
    ui->table->setItemDelegateForColumn(FileInfoField::Size, new SizeDelegate);
    ui->table->setItemDelegateForColumn(FileInfoField::Current, new SizeDelegate);
    ui->table->setItemDelegateForColumn(FileInfoField::Rate, new RateDelegate);

    ui->table->horizontalHeader()->setSectionResizeMode(FileInfoField::File, QHeaderView::Stretch);
//    ui->table->horizontalHeader()->setSectionResizeMode(FileInfoField::Local, QHeaderView::Stretch);
//    ui->table->horizontalHeader()->setSectionResizeMode(FileInfoField::Remote, QHeaderView::Stretch);
//    ui->table->horizontalHeader()->setSectionResizeMode(FileInfoField::Rate, QHeaderView::Stretch);

    connect(ui->table, &QTableView::doubleClicked, this, &MainWindow::onDetail);
}

void MainWindow::updateStatus()
{
    totalRate->setText(formatRate(fimodel->rate()));
    totalBytes->setText(formatSize(fimodel->bytes()));
    totalFiles->setText(QString("%1 files").arg(fimodel->files()));
}

void MainWindow::readSettings()
{
    QSettings settings;
    ui->actionScroll->setChecked(settings.value("scroll").toBool());
    int size = settings.beginReadArray("groups");
    for (int i = 0; i < size; i++) {
        settings.setArrayIndex(i);

        QString host = settings.value("host").toString();
        quint16 port = settings.value("port").toUInt();
        bool join = settings.value("join").toBool();

        GroupInfo *gi;
        try {
            gi = new GroupInfo(host, port, "");
            connect(gi, &GroupInfo::recv, this, &MainWindow::onRecv);
        } catch (GroupException &e) {
            continue;
        }
        if (join) {
            gi->Join();
        }
        groups.push_back(gi);
    }
    settings.endArray();
}

void MainWindow::writeSettings()
{
    QSettings settings;
    settings.setValue("scroll", ui->actionScroll->isChecked());
    settings.beginWriteArray("groups", groups.size());
    for (int i = 0; i < groups.size(); i++) {
        settings.setArrayIndex(i);

        auto gi = groups.at(i);
        settings.setValue("host", gi->Host().toString());
        settings.setValue("port", gi->Port());
        settings.setValue("join", gi->Enabled());
    }
    settings.endArray();
}

MainWindow::~MainWindow()
{
    delete ui;
}

