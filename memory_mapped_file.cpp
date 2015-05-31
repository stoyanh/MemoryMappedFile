#include "memory_mapped_file.h"
#include <sys/stat.h>
#include <fcntl.h>

MemMappedFile::MemMappedFile(const char* filePath):
    file(fopen(filePath, "rb+"))
{
    if(!file) {
        throw std::invalid_argument("Cannot open the specified file !");
    }
    flock fl;
    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    if(fcntl(fileno(file.get()), F_SETLKW, &fl) == -1) {
        throw std::runtime_error("Cannot lock file !");
    }

    fileInfo.fillSize(file.get());
}

MemMappedFile::~MemMappedFile()
{
    flushAll();
    flock fl;
    fl.l_type = F_UNLCK;
    fl.l_whence = SEEK_SET;
    fcntl(fileno(file.get()), F_SETLKW, &fl);
}

uint64_t MemMappedFile::size() const
{
    return fileInfo.fileSize;
}

void MemMappedFile::flush(uint64_t pageNumber)
{
    if(pageNumber == PagesManager::INVALID_PAGE_NUMBER || !pmanager.isModified(pageNumber)) {
        return;
    }
    char* data = pmanager.getPageData(pageNumber);
    uint64_t pageSize = pmanager.getPageSize(pageNumber);
    fseek(file.get(), pageNumber * PagesManager::PAGE_SIZE, SEEK_SET);
    uint64_t bytesWritten = fwrite(data, sizeof(char), pageSize, file.get());
    if(bytesWritten < pageSize) {
        throw std::runtime_error("Could write to file !");
    }
    pmanager.setModifiedFalse(pageNumber);
}

void MemMappedFile::flushAll()
{
    PagesManager::Modified modifiedPages = pmanager.getAllModifiedPages();
    for(int i = 0; i < modifiedPages.size; ++i) {
        flush(modifiedPages.pages[i]);
    }
}

bool MemMappedFile::inMemory(uint64_t pos) const
{
    uint64_t pageNumber = pos / PagesManager::PAGE_SIZE;
    return pmanager.isLoaded(pageNumber);
}

void MemMappedFile::loadPage(uint64_t pos)
{
    uint64_t pageNumber = pos / PagesManager::PAGE_SIZE;
    uint64_t pageStart = pageNumber * PagesManager::PAGE_SIZE;
    uint64_t pageSize;
    if(pageStart + PagesManager::PAGE_SIZE >= fileInfo.fileSize) {
        pageSize = fileInfo.fileSize - pageStart;
    } else {
        pageSize = PagesManager::PAGE_SIZE;
    }
    uint64_t pageToFlush = pmanager.pageToRemove();
    if(pageToFlush != PagesManager::INVALID_PAGE_NUMBER) {
        flush(pageToFlush);
        uint64_t oldSize = pmanager.getPageSize(pageToFlush);
        pmanager.reusePage(pageToFlush, pageNumber, pageStart,
                            pageSize != oldSize ? pageSize : PagesManager::INVALID_PAGE_SIZE);
    } else {
        pmanager.reservePage(pageNumber, pageStart, pageSize);
    }

    char* data = pmanager.getPageData(pageNumber);
    fseek(file.get(), pageNumber * PagesManager::PAGE_SIZE, SEEK_SET);
    uint64_t bytesRead = fread(data, sizeof(char), pageSize, file.get());
    if(bytesRead < pageSize) {
        throw std::runtime_error("Could not read from file !");
    }
}

CharProxy MemMappedFile::operator[](uint64_t pos)
{
    if(pos < 0 || pos >= fileInfo.fileSize) {
        throw std::out_of_range("Attempt to access value out of the valid range !");
    }
    if(!inMemory(pos)) {
        loadPage(pos);
    }

    uint64_t pageNumber = pos / PagesManager::PAGE_SIZE;
    uint64_t posInPage = pos % PagesManager::PAGE_SIZE;
    return CharProxy(pmanager.getByte(pageNumber, posInPage), pmanager.getFlag(pageNumber));
}
