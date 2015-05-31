#include "others.h"

void FileInfo::fillSize(FILE* file)
{
    fseek(file, 0, SEEK_SET);
    uint64_t start = ftell(file);
    fseek(file, 0, SEEK_END);
    uint64_t end = ftell(file);
    fileSize = end - start;
}

CharProxy::operator char()
{
    return byte;
}

char& CharProxy::operator=(const char& rhs)
{
    if(&byte == &rhs) {
        return byte;
    }
    byte = rhs;
    *modified = true;
}

PagesManager::PagesManager():
    nextFreePos(0)
{
}

uint64_t PagesManager::pageToRemove(uint64_t pageNumber) const
{
    if(nextFreePos >= MAX_PAGES) {
        return pages[LAST_PAGE].pageNumber;
    }
    return -1;
}

void PagesManager::reservePage(uint64_t pageNumber, uint64_t start, uint64_t size)
{
    int pos = (nextFreePos >= MAX_PAGES) ? LAST_PAGE : nextFreePos++;
    pages[pos].pageNumber = pageNumber;
    pages[pos].start = start;
    pages[pos].size = size;
    pages[pos].data = std::unique_ptr<char[]>(new char[size]);

    orderedPages[pos] = pageNumber;
    moveFront(pos);
}

void PagesManager::reusePage(
    uint64_t pageNumber,
    uint64_t newNumber,
    uint64_t newStart,
    uint64_t newSize)
{
    int pos = findPageIndex(pageNumber);
    pages[pos].pageNumber = newNumber;
    pages[pos].start = newStart;
    pages[pos].size = (newSize != INVALID_PAGE_SIZE) ? newSize : pages[pos].size;
    if(pages[pos].size == newSize) {
        pages[pos].data = std::unique_ptr<char[]>(new char[newSize]);
    }

    orderedPages[pos] = newNumber;
    moveFront(pos);
}

char& PagesManager::getByte(uint64_t pageNumber, uint64_t posInPage)
{
    int index = findPageIndex(pageNumber);
    return pages[index].data[posInPage];
}

bool PagesManager::isLoaded(uint64_t pageNumber) const
{
    for(int i = 0; i < nextFreePos; ++i) {
        if(pageNumber == orderedPages[i]) {
            return true;
        }
    }
    return false;
}

void PagesManager::moveFront(int index)
{
    for(int i = index; i >= 1; --i) {
        std::swap(orderedPages[i], orderedPages[i - 1]);
    }
}

uint64_t PagesManager::lastAccessed() const
{
    return orderedPages[0];
}

char* PagesManager::getPageData(uint64_t pageNumber)
{
   int index = findPageIndex(pageNumber);
   return pages[index].data.get();
}

uint64_t PagesManager::getPageSize(uint64_t pageNumber) const
{
    int index = findPageIndex(pageNumber);
    return pages[index].size;
}

int PagesManager::findPageIndex(uint64_t pageNumber) const
{
    for(int i = 0; i < nextFreePos; ++i) {
        if(pages[i].pageNumber == pageNumber) {
            return i;
        }
    }
    throw std::runtime_error("Internal error !");
}
