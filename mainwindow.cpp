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
#include <iostream>

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
    setWindowTitle(QString("Please select directory to scan"));


    ui->statusBar->addPermanentWidget(ui->stopButton);
    ui->statusBar->addPermanentWidget(ui->deleteButton);
    ui->statusBar->addPermanentWidget(ui->progressBar);

    ui->progressBar->setHidden(true);
    ui->deleteButton->setHidden(true);
    ui->progressBar->setRange(0, 100);
    ui->stopButton->setHidden(true);

    connect(ui->lineEdit, &QLineEdit::returnPressed, this, &main_window::searchSubstring);
    connect(ui->actionScan_Directory, &QAction::triggered, this, &main_window::select_directory);
    connect(ui->deleteButton, &QPushButton::clicked, this, &main_window::deleteFiles);
    connect(ui->treeWidget,
                SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)),
                this,
                SLOT(openFile(QTreeWidgetItem*, int)));
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

    QFileInfo selectedDirInfo = QFileInfo(dir);
    if (!selectedDirInfo.exists()) {
        return;
    }
    setupInterface();
    setWindowTitle(dir);
    if (thread != nullptr) {
        thread->exit();
        thread->wait();
        delete thread;
    }
    worker = new Worker(dir, this);
    thread = new QThread();
    worker->moveToThread(thread);

    connect(thread, SIGNAL(started()), worker, SLOT(indexDirectory()));
    connect(this, SIGNAL(doSearch(const QString&)), worker, SLOT(changePattern(const QString&)));
    thread->start();
}

void main_window::show_about_dialog()
{
    QMessageBox::aboutQt(this);
}

void main_window::addFile(const QString & fileName) {
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0, fileName);
    ui->treeWidget->addTopLevelItem(item);
}

void main_window::searchSubstring() {
    QString pattern = ui->lineEdit->text();
    emit doSearch(pattern);
}

void main_window::deleteFiles() {
    auto selectedItems = ui->treeWidget->selectedItems();
    auto answer = QMessageBox::question(this, "Deleting",
                                        "Do you want to delete " + QString::number(selectedItems.size())
                                        + " selected files?");
    if (answer == QMessageBox::No) {
        return;
    }
    for (auto & selectedItem: selectedItems) {
        QFile file(selectedItem->text(0));
        if (!file.exists() || file.remove()) {
            delete selectedItem;
        }
    }
}

void main_window::openFile(QTreeWidgetItem* item, int) {
    QFile file(item->text(0));
    if (file.exists()) {
        QDesktopServices::openUrl(item->text(0));
    }
}

void main_window::preIndexInterface() {
    ui->progressBar->setHidden(false);
    ui->stopButton->setHidden(false);
    ui->progressBar->setValue(0);
}

void main_window::postIndexInterface() {
    ui->stopButton->setHidden(true);
    ui->progressBar->setValue(100);
}

void main_window::preSearchInterface() {
    ui->stopButton->setHidden(false);
    ui->deleteButton->setHidden(true);
    ui->treeWidget->clear();
    ui->progressBar->setValue(0);
}

void main_window::postSearchInterface() {
    ui->stopButton->setHidden(true);
    ui->deleteButton->setHidden(false);
    ui->progressBar->setValue(100);
}

void main_window::setupInterface() {
    ui->treeWidget->clear();
    ui->stopButton->setHidden(true);
    ui->deleteButton->setHidden(true);
}

void main_window::setProgress(int percent) {
    ui->progressBar->setValue(percent);
}
