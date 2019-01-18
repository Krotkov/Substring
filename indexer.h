#ifndef INDEXER_H
#define INDEXER_H


#include <QObject>
#include <QString>
#include <QSet>
#include <QMap>
#include <QFile>
#include <QFileSystemWatcher>

using FileTrigrams = QSet<qint32>;
using FilesTrigrams = QMap<QString, FileTrigrams>;

class Indexer : public QObject {
    Q_OBJECT
public:
    explicit Indexer(QString const& dir, QFileSystemWatcher * watcher);
public:
    void indexDirectory(FilesTrigrams & filesTrigrams);
    void countFileTrigrams(QFile & file, FileTrigrams & fileTrigrams);
public:
signals:
    void started();
    void finished();
    void progress(int);


private:
    void updateProgress(qint64);
    qint32 getTrigramHash(char* trigramPointer);
    QFileSystemWatcher * watcher;
    QString directory;
    qint64 sumSize = 0;
};

#endif // INDEXER_H
