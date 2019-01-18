#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include <worker.h>
#include <QThread>
#include <QTreeWidgetItem>

namespace Ui {
class MainWindow;
}

class main_window : public QMainWindow
{
    Q_OBJECT

public:
    explicit main_window(QWidget *parent = nullptr);
    ~main_window();
public slots:
    void select_directory();
    void show_about_dialog();
    void searchSubstring();
    void addFile(const QString& fileName);

public:
signals:
    void setPattern();
    void doSearch(const QString&);
private:
    std::unique_ptr<Ui::MainWindow> ui;
    Worker * worker;
    QThread * thread;
};

#endif // MAINWINDOW_H
