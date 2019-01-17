#include "indexer.h"
#include <QDirIterator>
#include <QDir>

const qint64 BUFFER_SIZE = 128*1024;
const qint16 SHIFT = 2;
const qint32 MAX_CHAR = 256;
const qint32 MAGIC_TRIGRAMS = 20000;

Indexer::Indexer(QString const& directory) : directory(directory) {}

void Indexer::indexDirectory(FilesTrigrams & filesTrigrams) {
     QDirIterator dirIterator(directory, QDir::Files, QDirIterator::Subdirectories);
     while (dirIterator.hasNext()) {
        QFileInfo fileInfo(dirIterator.next());
        if (!fileInfo.permission(QFile::ReadUser)) {
            continue;
        }
        FileTrigrams fileTrigrams;
        QFile file(fileInfo.absolutePath());
        countFileTrigrams(file, fileTrigrams);
        if (fileTrigrams.size() >= MAGIC_TRIGRAMS) {
            continue;
        }
        filesTrigrams[fileInfo.absoluteFilePath()] = fileTrigrams;
     }
}

void Indexer::countFileTrigrams(QFile & file, FileTrigrams& fileTrigrams) {
    char* buffer = new char[BUFFER_SIZE];
    file.read(buffer, SHIFT);
    while (!file.atEnd()) {
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
}

qint32 Indexer::getTrigramHash(char* trigramPointer) {
    qint32 result = 0;
    for (int i = 0; i < 3; i++) {
        result =  result * MAX_CHAR + qint32(trigramPointer[i]);
    }
    return result;
}
