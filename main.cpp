#include <iostream>
#include <memory>
#include <vector>
#include "memory_mapped_file.h"
#include <chrono>
#include <ratio>
#include <ctime>

using namespace std;

int main()
{
    using namespace chrono;
    MemMappedFile mfile("/home/stoyan/workspace/example_file");
    uint64_t size = mfile.size();
    cout << size << endl;

    auto start = high_resolution_clock::now();
    for(uint64_t i = 0; i < size; ++i) {
        mfile[i] = 'a';
    }
    auto end = high_resolution_clock::now();
    auto interval = duration_cast<duration<double> >(end - start);
    cout << fixed << interval.count() << endl;
    return 0;
}
