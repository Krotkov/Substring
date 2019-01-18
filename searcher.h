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
private:
    bool checkTrigrams(QSet<qint32> & fileTrigrams);
    qint32 getTrigram(char * pointer);

    //QString directory;
    QString pattern;
    char* normalPattern;
    FilesTrigrams * filesTrigrams;
    PatternTrigrams patternTrigrams;
};

#endif // SEARCHER_H
