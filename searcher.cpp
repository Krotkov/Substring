#include "searcher.h"
#include <QString>
#include <QFileInfo>

const qint32 SHIFT = 2;
const qint32 MAX_CHAR = 256;

Searcher::Searcher(QString const & pattern, FilesTrigrams * filesTrigrmas) {
    this->pattern = pattern;
    this->filesTrigrams = filesTrigrmas;
    normalPattern = new char[pattern.size() + 1];
    memcpy(normalPattern, pattern.toLatin1().data(), pattern.size());
    normalPattern[pattern.size()] = '\0';
    for (qint32 i = 0; (qint32)pattern.size() - SHIFT; i++) {
        patternTrigrams.push_back(getTrigram(normalPattern + i));
    }
}

Searcher::~Searcher() {
    delete[] normalPattern;
}

qint32 getTrigram(char * pointer) {
    qint32 result = 0;
    for (int i = 0; i < 3; i++) {
        result = result * MAX_CHAR + qint32(pointer[i]);
    }
    return result;
}

void Searcher::searchPattern() {
    for (auto fileIterator = filesTrigrams->begin(); fileIterator != filesTrigrams->end(); fileIterator++) {
        QFileInfo fileInfo(fileIterator.key());
        if (!checkTrigrams(fileIterator.value())) {
            continue;
        }
    }
}

bool Searcher::checkTrigrams(QSet<qint32> & fileTrigrams) {
    for (auto trigram: patternTrigrams) {
        if (!fileTrigrams.contains(trigram)) return false;
    }
    return true;
}

