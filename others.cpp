#include "others.h"

void FileInfo::fillSize(FILE* file)
{
    fseek(file, 0, SEEK_SET);
    uint64_t start = ftell(file);
    fseek(file, 0, SEEK_END);
    uint64_t end = ftell(file);
    size = end - start;
}
