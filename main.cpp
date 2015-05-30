#include <iostream>
#include <memory>
#include <vector>
#include "memory_mapped_file.h"

using namespace std;

int main()
{
    MemMappedFile mfile("/home/stoyan/workspace/example_file");
    uint64_t size = mfile.size();
    cout << size << endl;

    for(int i = 0; i < size / 2; ++i) {
        mfile[i] = mfile[i + size / 2];
    }

    return 0;
}
