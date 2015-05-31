#include "memory_mapped_file.h"
#include <sys/stat.h>
#include <fcntl.h>

MemMappedFile::MemMappedFile(const char* filePath, uint64_t maxSize):
    file(fopen(filePath, "rb+"))
{
    if(!file) {
        throw std::invalid_argument("Cannot open the specified file !");
    }
    maxInMemSize = maxSize;

    fileInfo.fillSize(file.get());
}

MemMappedFile::~MemMappedFile()
{
    flush(pmanager.lastAccessed());
}

uint64_t MemMappedFile::size() const
{
    return fileInfo.fileSize;
}

void MemMappedFile::flush(uint64_t pageNumber)
{
    char* data = pmanager.getPageData(pageNumber);
    uint64_t pageSize = pmanager.getPageSize(pageNumber);
    fseek(file.get(), pageNumber * PAGE_SIZE, SEEK_SET);
    uint64_t bytesWritten = fwrite(data, sizeof(char), pageSize, file.get());
    if(bytesWritten < pageSize) {
        throw std::runtime_error("Could write to file !");
    }
}

bool MemMappedFile::inMemory(uint64_t pos) const
{
    uint64_t pageNumber = pos / PAGE_SIZE;
    return pmanager.isLoaded(pageNumber);
}

void MemMappedFile::loadPage(uint64_t pos)
{
    uint64_t pageNumber = pos / PAGE_SIZE;
    uint64_t pageStart = pageNumber * PAGE_SIZE;
    uint64_t pageSize;
    if(pageStart + PAGE_SIZE >= fileInfo.fileSize) {
        pageSize = fileInfo.fileSize - pageStart;
    } else {
        pageSize = pageStart + PAGE_SIZE;
    }
    uint64_t pageToFlush = pmanager.pageToRemove(pageNumber);
    if(pageToFlush != INVALID_PAGE_NUMBER) {
        flush(pageToFlush);
        uint64_t oldSize = pmanager.getPageSize(pageToFlush);
        pmanager.reusePage(pageToFlush, pageNumber, pageStart,
                            pageSize != oldSize ? pageSize : INVALID_PAGE_SIZE);
    } else {
        pmanager.reservePage(pageNumber, pageStart, pageSize);
    }

    char* data = pmanager.getPageData(pageNumber);
    fseek(file.get(), pageNumber * PAGE_SIZE, SEEK_SET);
    uint64_t bytesRead = fread(data, sizeof(char), pageSize, file.get());
    if(bytesRead < pageSize) {
        throw std::runtime_error("Could not read from file !");
    }
}

char& MemMappedFile::operator[](uint64_t pos)
{
    if(pos < 0 || pos >= fileInfo.fileSize) {
        throw std::out_of_range("Attempt to access value out of the valid range !");
    }
    if(!inMemory(pos)) {
        loadPage(pos);
    }

    uint64_t pageNumber = pos / PAGE_SIZE;
    uint64_t posInPage = pos % PAGE_SIZE;
    return pmanager.getByte(pageNumber, posInPage);
}
