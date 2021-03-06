#include "searcher.h"
#include <QString>
#include <QFileInfo>
#include <QFile>
#include <iostream>

const qint32 SHIFT = 2;
const qint32 MAX_CHAR = 256;
const qint64 BUFFER_SIZE = 128*1024;


Searcher::Searcher(QString const & pattern, FilesTrigrams * filesTrigrmas) {
    this->pattern = pattern;
    this->filesTrigrams = filesTrigrmas;
    this->needStop = false;
    normalPattern = new char[pattern.size() + 1];
    memcpy(normalPattern, pattern.toLatin1().data(), pattern.size());
    normalPattern[pattern.size()] = '\0';
    for (qint32 i = 0; i < (qint32)(pattern.size()) - SHIFT; i++) {
        patternTrigrams.push_back(getTrigram(normalPattern + i));
    }
}

Searcher::~Searcher() {
    delete[] normalPattern;
}

qint32 Searcher::getTrigram(char * pointer) {
    qint32 result = 0;
    for (int i = 0; i < 3; i++) {
        result = result * MAX_CHAR + qint32(pointer[i]);
    }
    return result;
}

void Searcher::searchPattern() {
    emit started();
    for (auto filePath: filesTrigrams->keys()) {
        if (needStop) break;
        QFileInfo fileInfo(filePath);
        sumSize += fileInfo.size();
    }
    if (sumSize == 0) {
        progress(100);
        return;
    }
    qint64 curSize = 0;
    for (auto fileIterator = filesTrigrams->begin(); fileIterator != filesTrigrams->end(); fileIterator++) {
        if (needStop) {
            break;
        }
        QFileInfo fileInfo(fileIterator.key());
        curSize += fileInfo.size();
        if (!checkTrigrams(fileIterator.value())) {
            continue;
        }
        QFile file(fileIterator.key());
        searchPatternInFile(file);
        updateProgress(curSize);
    }
    if (needStop) emit interrupted();
    else emit finished();
}

void Searcher::searchPatternInFile(QFile & file) {
    if (!file.open(QIODevice::ReadOnly)) {
        throw std::logic_error("Can't open file: " + file.fileName().toStdString());
    }
    qint64 patternShift = qint64(pattern.size()) - 1;
    char * buffer = new char[BUFFER_SIZE + 1];
    buffer[BUFFER_SIZE] = '\0';
    file.read(buffer, patternShift);
    while (!file.atEnd()) {
        if (needStop) break;
        qint64 size = patternShift + file.read(buffer + patternShift, BUFFER_SIZE - patternShift);
        buffer[size] = '\0';
        char* ptr = strstr(buffer, normalPattern);
        if (ptr) {
           emit foundFile(file.fileName());
           break;
        }
    }
    file.close();
    memset(buffer, 'z', BUFFER_SIZE);
    delete[] buffer;
}

bool Searcher::checkTrigrams(QSet<qint32> & fileTrigrams) {
    for (auto trigram: patternTrigrams) {
        if (!fileTrigrams.contains(trigram)) return false;
    }
    return true;
}

void Searcher::updateProgress(qint64 percent) {
    emit progress(percent * 100 / sumSize);
}

void Searcher::stop() {
    needStop = true;
}
