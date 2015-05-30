#include "memory_mapped_file.h"
#include <sys/stat.h>
#include <fcntl.h>

MemMappedFile::MemMappedFile(const char* filePath, uint64_t maxSize):
    file(fopen(filePath, "r+b"))
{
    if(!file) {
        throw std::invalid_argument("Cannot open the specified file !");
    }
    pages.reserve(DEFAULT_MAX_IN_MEM_SIZE / PAGE_SIZE);
    maxInMemSize = maxSize;

    info.fillSize(file);
}

MemMappedFile::~MemMappedFile()
{

}

bool MemMappedFile::inMemory(uint64_t pos) const
{
    uint64_t pageNumber = pos / PAGE_SIZE;
}

char& MemMappedFile::operator[](uint64_t pos)
{
    if(pos < 0 || pos >= info.size) {
        throw std::out_of_range("Attempt to access value out of the valid range !");
    }
    if(!inMemory(pos)) {
        loadPage(pos);
    }

}
