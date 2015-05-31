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

CharProxy& CharProxy::operator=(const CharProxy& rhs)
{
    if(this != &rhs) {
        byte = rhs.byte;
        modified = true;
    }
    return *this;
}

CharProxy& CharProxy::operator=(const char& rhs)
{
    if(&byte != &rhs) {
        byte = rhs;
        modified = true;
    }
    return *this;
}

PagesManager::PagesManager():
    nextFreePos(0)
{
}

uint64_t PagesManager::pageToRemove() const
{
    if(nextFreePos >= MAX_PAGES) {
        return orderedPages[LAST_PAGE];
    }
    return INVALID_PAGE_NUMBER;
}

void PagesManager::reservePage(uint64_t pageNumber, uint64_t start, uint64_t size)
{
    int pos = (nextFreePos >= MAX_PAGES) ? LAST_PAGE : nextFreePos++;
    pages[pos].modified = false;
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
    pages[pos].modified = false;
    pages[pos].pageNumber = newNumber;
    pages[pos].start = newStart;
    pages[pos].size = (newSize != INVALID_PAGE_SIZE) ? newSize : pages[pos].size;
    if(pages[pos].size == newSize) {
        pages[pos].data = std::unique_ptr<char[]>(new char[newSize]);
    }

    orderedPages[LAST_PAGE] = newNumber;
    moveFront(LAST_PAGE);
}

Modified PagesManager::getAllModifiedPages() const
{
    Modified modifiedPages;
    modifiedPages.size = 0;
    for(int i = 0; i < nextFreePos; ++i) {
        if(pages[i].modified) {
            modifiedPages.pages[modifiedPages.size++] = orderedPages[i];
        }
    }

    return modifiedPages;
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
    if(nextFreePos <= 0) {
        return INVALID_PAGE_NUMBER;
    }
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

bool PagesManager::isModified(uint64_t pageNumber) const
{
    int index = findPageIndex(pageNumber);
    return pages[index].modified;
}

void PagesManager::setModifiedFalse(uint64_t pageNumber)
{
    int index = findPageIndex(pageNumber);
    pages[index].modified = false;
}

bool& PagesManager::getFlag(uint64_t pageNumber)
{
    int index = findPageIndex(pageNumber);
    return pages[index].modified;
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
