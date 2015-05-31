#ifndef OTHERS_H_INCLUDED
#define OTHERS_H_INCLUDED

#include <memory>
#include <exception>
#include <cstdio>

const uint64_t DEFAULT_MAX_IN_MEM_SIZE = 1 << 27; /// 128mb
const uint64_t PAGE_SIZE = 1 << 24; /// 16mb
const uint64_t MAX_PAGES = DEFAULT_MAX_IN_MEM_SIZE /PAGE_SIZE;
const uint64_t LAST_PAGE = MAX_PAGES - 1;
const uint64_t INVALID_PAGE_NUMBER = -1;
const uint64_t INVALID_PAGE_SIZE = -1;

struct FileDeleter {
    void operator()(FILE* file) {
        fclose(file);
    }
};

struct FileInfo {
    void fillSize(FILE* file);
    uint64_t fileSize;
};

struct Page {
    uint64_t start;
    uint64_t size;
    uint64_t pageNumber;
    bool modified;
    std::unique_ptr<char[]> data;
};

class CharProxy
{
public:
    CharProxy(char& byte, bool* modified):
         byte(byte), modified(modified){}
    operator char();
    char& operator=(const char& rhs);
private:
    char& byte;
    bool* modified;
};

class PagesManager
{
public:
    PagesManager();
    bool isLoaded(uint64_t pageNumber) const;
    uint64_t pageToRemove(uint64_t pageNumber) const;
    void reservePage(uint64_t pageNumber, uint64_t start, uint64_t size);
    void reusePage(
        uint64_t pageNumber,
        uint64_t newNumber,
        uint64_t newStart,
        uint64_t newSize = INVALID_PAGE_SIZE);
    uint64_t lastAccessed() const;
    char& getByte(uint64_t pageNumber, uint64_t posInPage);
    char* getPageData(uint64_t pageNumber);
    uint64_t getPageSize(uint64_t pageNumber) const;
private:
    void moveFront(int index);
    int findPageIndex(uint64_t pageNumber) const;

    int nextFreePos;
    uint64_t orderedPages[MAX_PAGES];
    Page pages[MAX_PAGES];
};

#endif // OTHERS_H_INCLUDED
