#ifndef OTHERS_H_INCLUDED
#define OTHERS_H_INCLUDED

#include <memory>
#include <exception>
#include <cstdio>

struct FileDeleter {
    void operator()(FILE* file) {
        fclose(file);
    }
};

struct FileInfo {
    void fillSize(FILE* file);
    uint64_t fileSize;
};

class CharProxy
{
public:
    CharProxy(char& byte, bool& modified):
         byte(byte), modified(modified){}
    operator char();
    CharProxy& operator=(const CharProxy& rhs);
    CharProxy& operator=(const char& rhs);
private:
    char& byte;
    bool& modified;
};

struct Page {
    uint64_t start;
    uint64_t size;
    uint64_t pageNumber;
    bool modified;
    std::unique_ptr<char[]> data;
};


class PagesManager
{
    friend class MemMappedFile;
    const static uint64_t DEFAULT_MAX_IN_MEM_SIZE = 1 << 27; /// 128mb
    const static uint64_t PAGE_SIZE = 1 << 24; /// 16mb
    const static uint64_t MAX_PAGES = DEFAULT_MAX_IN_MEM_SIZE / PAGE_SIZE;
    const static uint64_t LAST_PAGE = MAX_PAGES - 1;
    const static uint64_t INVALID_PAGE_NUMBER = -1;
    const static uint64_t INVALID_PAGE_SIZE = -1;

    struct Modified {
        int size;
        uint64_t pages[MAX_PAGES];
    };
public:
    PagesManager();
    bool isLoaded(uint64_t pageNumber) const;
    uint64_t pageToRemove() const;
    void reservePage(uint64_t pageNumber, uint64_t start, uint64_t size);
    void reusePage(
        uint64_t pageNumber,
        uint64_t newNumber,
        uint64_t newStart,
        uint64_t newSize = INVALID_PAGE_SIZE);
    uint64_t lastAccessed() const;
    char& getByte(uint64_t pageNumber, uint64_t posInPage);
    bool& getFlag(uint64_t pageNumber);
    char* getPageData(uint64_t pageNumber);
    uint64_t getPageSize(uint64_t pageNumber) const;
    bool isModified(uint64_t pageNumber)const;
    void setModifiedFalse(uint64_t pageNumber);
    Modified getAllModifiedPages() const;
private:
    void moveFront(int index);
    int findPageIndex(uint64_t pageNumber) const;

    int nextFreePos;
    uint64_t orderedPages[MAX_PAGES];
    Page pages[MAX_PAGES];
};

#endif // OTHERS_H_INCLUDED
