#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "worker.h"

#include <QCommonStyle>
#include <QDesktopWidget>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QDesktopServices>

main_window::main_window(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      worker(nullptr),
      thread(nullptr)
{
    ui->setupUi(this);
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), qApp->desktop()->availableGeometry()));

    ui->treeWidget->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->treeWidget->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->treeWidget->setUniformRowHeights(1);

    QCommonStyle style;
    ui->actionScan_Directory->setIcon(style.standardIcon(QCommonStyle::SP_DialogOpenButton));
    ui->actionExit->setIcon(style.standardIcon(QCommonStyle::SP_DialogCloseButton));
    ui->actionAbout->setIcon(style.standardIcon(QCommonStyle::SP_DialogHelpButton));

    ui->statusBar->addPermanentWidget(ui->progressBar);

    ui->progressBar->setHidden(true);
    ui->progressBar->setRange(0, 100);
    ui->stopButton->setHidden(true);

    connect(ui->lineEdit, &QLineEdit::returnPressed, this, &main_window::searchSubstring);
    connect(ui->actionScan_Directory, &QAction::triggered, this, &main_window::select_directory);
}

main_window::~main_window()
{
    if (thread != nullptr) {
        thread->exit();
        thread->wait();
    }
}

void main_window::select_directory()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Select Directory for Scanning",
                                                    QString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    ui->treeWidget->clear();

    worker = new Worker(dir, this);
    thread = new QThread();
    worker->moveToThread(thread);

    connect(thread, SIGNAL(started()), worker, SLOT(indexDirectory()));
    connect(this, SIGNAL(DoSearch(const QString&)), worker, SLOT(changePattern(const QString&)));
    thread->start();
}

void main_window::show_about_dialog()
{
    QMessageBox::aboutQt(this);
}

void main_window::addFile(QString & fileName) {
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0, fileName);
    ui->treeWidget->addTopLevelItem(item);
}

void main_window::searchSubstring() {
    QString pattern = ui->lineEdit->text();
    emit doSearch(pattern);
}
