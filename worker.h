#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QString>
#include "indexer.h"
#include "searcher.h"

class Worker : public QObject
{
    Q_OBJECT
public:
    Worker(QString const & dir, QObject * parent);
    ~Worker();
public slots:
    void indexDirectory();
    void searchSubstring();
    void changePattern(const QString&);
private:
    Indexer * indexer;
    Searcher * searcher;
    QObject * mainWindow;
    QString dir;
    QString pattern;
    FilesTrigrams filesTrigrams;
};

#endif // WORKER_H
