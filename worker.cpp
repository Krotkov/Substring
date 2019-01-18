#include "worker.h"
#include <iostream>

Worker::Worker(QString const& dir, QObject * parent)
    : indexer(nullptr), searcher(nullptr), mainWindow(parent), dir(dir)
{}

void Worker::indexDirectory() {
    if (indexer != nullptr) delete indexer;
    indexer = new Indexer(dir);
    indexer->indexDirectory(filesTrigrams);
    for (auto it = filesTrigrams.begin(); it != filesTrigrams.end(); it++) {
        std::cout << it->size() << std::endl;
    }
}

void Worker::searchSubstring() {
    if (searcher != nullptr) delete searcher;
    searcher = new Searcher(pattern, &filesTrigrams);
    connect(searcher, SIGNAL(foundFile(QString &)), mainWindow, SLOT(addFile(QString&)));

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
