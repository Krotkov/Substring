#ifndef SEARCHER_H
#define SEARCHER_H

#include <QObject>
#include <QVector>
#include "indexer.h"

using PatternTrigrams = QVector<qint32>;

class Searcher : public QObject {
    Q_OBJECT
public:
    Searcher(QString const & pattern, FilesTrigrams * filesTrigrmas);
    ~Searcher();

    void searchPattern();
public:
signals:
    void foundFile(const QString&);
    void started();
    void finished();
    void progress(int);

private:
    qint64 sumSize = 0;
    void updateProgress(qint64);
    bool checkTrigrams(QSet<qint32> & fileTrigrams);
    qint32 getTrigram(char * pointer);
    void searchPatternInFile(QFile & file);

    //QString directory;
    QString pattern;
    char* normalPattern;
    FilesTrigrams * filesTrigrams;
    PatternTrigrams patternTrigrams;
};

#endif // SEARCHER_H
