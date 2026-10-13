// Compiler: MSVC 19.44 (Visual Studio 2022 17.14.13), /std:c++20, x64, Release

#include <format>
#include <iostream>
#include <thread>
#include <vector>

#include "benchmark.h"
#include "sequence.h"

using namespace std;

// LAB_NO_OPT is added by hand in the /Od build (Preprocessor Definitions)
#ifdef LAB_NO_OPT
const string OPT_LEVEL = "/Od";
#else
const string OPT_LEVEL = "/O2";
#endif

// Cheap: one comparison, so the speed is limited by memory
struct IsSmall {
    bool operator()(int x) const {
        return x < MAX_VALUE / 2;
    }
};

// Expensive: trial division, a lot of computation per element
struct IsPrime {
    bool operator()(int x) const {
        if (x < 2) {
            return false;
        }
        if (x % 2 == 0) {
            return x == 2;
        }
        for (int d = 3; d * d <= x; d += 2) {
            if (x % d == 0) {
                return false;
            }
        }
        return true;
    }
};

int main() {
    generateFiles();

    size_t threads = thread::hardware_concurrency();
    if (threads == 0) {
        threads = 1;
    }

    cout << format("replace_if, {}, hardware threads: {}, time in ms (median of {} runs)\n",
                   OPT_LEVEL, threads, RUNS);

    for (size_t size : SIZES) {
        vector<int> data = readFromFile(fileName(size));
        cout << "\nN = " << data.size() << "\n";

        runExperiments("predicate: x < 500000 (cheap)", data, IsSmall(), threads);
        runExperiments("predicate: x is prime (expensive)", data, IsPrime(), threads);
    }
    return 0;
}
