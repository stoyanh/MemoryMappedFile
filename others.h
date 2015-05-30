#ifndef OTHERS_H_INCLUDED
#define OTHERS_H_INCLUDED

#include <memory>
#include <cstdio>

struct FileDeleter {
    void operator()(FILE* file) {
        close(file);
    }
};

struct Page {
    uint64_t start;
    uint64_t size;
    std::unique_ptr<char[]> memory;
};

struct FileInfo {
    void fillSize(FILE* file);
    uint64_t size;
};

#endif // OTHERS_H_INCLUDED
