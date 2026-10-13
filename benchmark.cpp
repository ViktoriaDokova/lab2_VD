#include "benchmark.h"

using namespace std;

const size_t MAX_K = 1024;

// K = 1..2*threads, then doubling up to MAX_K to see what happens with too many threads
vector<size_t> makeKValues(size_t threads) {
    vector<size_t> ks;
    for (size_t k = 1; k <= 2 * threads; k++) {
        ks.push_back(k);
    }
    for (size_t k = 4 * threads; k <= MAX_K; k *= 2) {
        ks.push_back(k);
    }
    return ks;
}

void printKTable(const vector<size_t>& ks, const vector<double>& times, size_t threads) {
    cout << "  own algorithm:\n";
    cout << "     K   time, ms\n";
    for (size_t i = 0; i < ks.size(); i++) {
        cout << format("  {:>4}   {:.3f}\n", ks[i], times[i]);
    }

    size_t best = 0;
    for (size_t i = 1; i < times.size(); i++) {
        if (times[i] < times[best]) {
            best = i;
        }
    }
    cout << format("  best K = {}, threads = {}, K / threads = {:.2f}\n", ks[best], threads,
                   static_cast<double>(ks[best]) / static_cast<double>(threads));

    // The last K is twice the previous one, so a ratio close to 2 means linear growth
    size_t last = ks.size() - 1;
    double extraThreads = static_cast<double>(ks[last] - ks[last - 1]);
    double perThreadUs = (times[last] - times[last - 1]) * 1000 / extraThreads;
    cout << format("  K {} -> {}: time x{:.2f}, ~{:.0f} us per extra thread\n", ks[last - 1],
                   ks[last], times[last] / times[last - 1], perThreadUs);
}
