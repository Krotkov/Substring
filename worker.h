#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QFileSystemWatcher>
#include <QString>
#include "indexer.h"
#include "searcher.h"

class Worker : public QObject
{
    Q_OBJECT
public:
    Worker(QString const & dir, QObject * parent);
    ~Worker();

public:
signals:
    void stopAll();
public slots:
    void updateFile(const QString&);
    void indexDirectory();
    void searchSubstring();
    void changePattern(const QString&);
    void stop();
private:
    QObject * mainWindow;
    QString dir;
    QString pattern;
    FilesTrigrams filesTrigrams;
    QFileSystemWatcher *watcher;
};

#endif // WORKER_H
