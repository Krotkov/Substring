#include "indexer.h"
#include <QDirIterator>
#include <QDir>
#include <iostream>

const qint64 BUFFER_SIZE = 128*1024;
const qint16 SHIFT = 2;
const qint32 MAX_CHAR = 256;
const qint32 MAGIC_TRIGRAMS = 20000;

Indexer::Indexer(QString const& directory, QFileSystemWatcher * watcher)
    : watcher(watcher), directory(directory), needStop(false) {}

void Indexer::indexDirectory(FilesTrigrams & filesTrigrams) {
    emit started();
     QDirIterator it(directory, QDir::Files, QDirIterator::Subdirectories);
     while (it.hasNext()) {
        QFileInfo fileInfo(it.next());
        sumSize += fileInfo.size();
     }
     if (sumSize == 0) {
         emit progress(100);
         return;
     }
     QDirIterator dirIterator(directory, QDir::Files, QDirIterator::Subdirectories);
     qint64 curSize = 0;
     while (dirIterator.hasNext()) {
         if (needStop) {
             break;
         }
        QFileInfo fileInfo(dirIterator.next());
        curSize += fileInfo.size();
        if (!fileInfo.permission(QFile::ReadUser)) {
            continue;
        }
        FileTrigrams fileTrigrams;
        QFile file(fileInfo.absoluteFilePath());
        try {
            countFileTrigrams(file, fileTrigrams);
            if (fileTrigrams.size() >= MAGIC_TRIGRAMS) {
                continue;
            }
            watcher->addPath(fileInfo.absoluteFilePath());
            filesTrigrams[fileInfo.absoluteFilePath()] = fileTrigrams;
            }
        catch(std::logic_error) {

        }
        updateProgress(curSize);
     }
     if (needStop) emit interrupted();
     else emit finished();
}

void Indexer::countFileTrigrams(QFile & file, FileTrigrams& fileTrigrams) {
    //std:: << "t" << std::endl;
    if (!file.open(QIODevice::ReadOnly)) {
        throw std::logic_error("Can't open file: " + file.fileName().toStdString());
    }
    char* buffer = new char[BUFFER_SIZE];
    file.read(buffer, SHIFT);
    while (!file.atEnd()) {
        if (needStop) {
            break;
        }
        qint64 size = SHIFT + file.read(buffer + SHIFT, BUFFER_SIZE - SHIFT);
        if (fileTrigrams.size() >= MAGIC_TRIGRAMS) {
            break;
        }
        for (qint64 i = 0; i < size - SHIFT; i++) {
            fileTrigrams.insert(getTrigramHash(buffer + i));
        }
    }
    delete[] buffer;
    file.close();
    //std::cout << "ff" << std::endl;
}

qint32 Indexer::getTrigramHash(char* trigramPointer) {
    qint32 result = 0;
    for (int i = 0; i < 3; i++) {
        result =  result * MAX_CHAR + qint32(trigramPointer[i]);
    }
    return result;
}

void Indexer::updateProgress(qint64 curSize) {
    emit progress((curSize * 100 / sumSize));
}

void Indexer::stop() {
    needStop = true;
}
