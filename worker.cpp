#include "worker.h"
#include <iostream>
#include <QFileInfo>

Worker::Worker(QString const& dir, QObject * parent)
    : indexer(nullptr), searcher(nullptr), mainWindow(parent), dir(dir)
{
    connect(&watcher, SIGNAL(fileChanged(const QString&)), this, SLOT(updateFile(const QString&)));
}

void Worker::indexDirectory() {
    if (indexer != nullptr) delete indexer;
    indexer = new Indexer(dir, &watcher);
    indexer->indexDirectory(filesTrigrams);
}

void Worker::searchSubstring() {
    if (searcher != nullptr) delete searcher;
    searcher = new Searcher(pattern, &filesTrigrams);
    connect(searcher, SIGNAL(foundFile(const QString &)), mainWindow, SLOT(addFile(const QString&)));

    searcher->searchPattern();
}

Worker::~Worker() {
    if (indexer != nullptr) delete indexer;
    if (searcher != nullptr) delete searcher;
}

void Worker::changePattern(const QString& pattern) {
    if (pattern.isEmpty()) {
        return;
    }
    if (this->pattern != pattern) {
        this->pattern = pattern;
    }
    searchSubstring();
}

void Worker::updateFile(const QString & filePath) {
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists() || !fileInfo.permission(QFile::ReadUser)) {
        filesTrigrams.remove(filePath);
        watcher.removePath(filePath);
        return;
    }
    if (indexer == nullptr) indexer = new Indexer(dir, &watcher);
    filesTrigrams[filePath].clear();
    QFile file(filePath);
    FileTrigrams fileTrigrams;
    indexer->countFileTrigrams(file, fileTrigrams);
    filesTrigrams[filePath] = fileTrigrams;
}
