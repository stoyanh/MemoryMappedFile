#ifndef MEMORY_MAPPED_FILE_H_INCLUDED
#define MEMORY_MAPPED_FILE_H_INCLUDED

#include "others.h"
#include <vector>
#include <memory>

const uint64_t DEFAULT_MAX_IN_MEM_SIZE = 1 << 27; /// 128mb
const uint64_t DEFAULT_PAGE_SIZE = 1 << 24; /// 16mb

class MemMappedFile
{
public:
    MemMappedFile(const char* filePath, uint64_t maxSize = DEFAULT_MAX_IN_MEM_SIZE);
    ~MemMappedFile();
    MemMappedFile& operator=(const MemMappedFile&) = delete;
    MemMappedFile(const MemMappedFile&) = delete;

    char& operator[](uint64_t pos);

private:
    bool inMemory(uint64_t pos) const;
    bool loadPage(uint64_t pos);

    uint64_t maxInMemSize;
    FileInfo info;
    std::unique_ptr<FILE, FileDeleter> file;
    std::vector<Page> pages;
};

#endif // MEMORY_MAPPED_FILE_H_INCLUDED

