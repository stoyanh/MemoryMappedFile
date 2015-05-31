#ifndef MEMORY_MAPPED_FILE_H_INCLUDED
#define MEMORY_MAPPED_FILE_H_INCLUDED

#include "others.h"
#include <memory>
#include <exception>

class MemMappedFile
{
public:
    MemMappedFile(const char* filePath);
    ~MemMappedFile();
    MemMappedFile& operator=(const MemMappedFile&) = delete;
    MemMappedFile(const MemMappedFile&) = delete;

    CharProxy operator[](uint64_t pos);
    uint64_t size() const;

private:
    bool inMemory(uint64_t pos) const;
    void loadPage(uint64_t pos);
    void flush(uint64_t pageNumber);
    void flushAll();

    uint64_t maxInMemSize;
    FileInfo fileInfo;
    std::unique_ptr<FILE, FileDeleter> file;

    PagesManager pmanager;
};

#endif // MEMORY_MAPPED_FILE_H_INCLUDED

