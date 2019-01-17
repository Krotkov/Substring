#ifndef INDEXER_H
#define INDEXER_H


#include <QObject>
#include <QString>
#include <QSet>
#include <QMap>
#include <QFile>

using FileTrigrams = QSet<qint32>;
using FilesTrigrams = QMap<QString, FileTrigrams>;

class Indexer : public QObject {
    Q_OBJECT
public:
    explicit Indexer(QString const& dir);
public:
    void indexDirectory(FilesTrigrams & filesTrigrams);

private:
    void countFileTrigrams(QFile & file, FileTrigrams & fileTrigrams);
    qint32 getTrigramHash(char* trigramPointer);

    QString directory;
    //FilesTrigrams filesTrigrams;

};

#endif // INDEXER_H
