#pragma once

#include <algorithm>
#include <chrono>
#include <execution>
#include <format>
#include <iostream>
#include <string>
#include <vector>

#include "parallel_replace_if.h"

const int RUNS = 5;
const int NEW_VALUE = 0;  // not in [MIN_VALUE, MAX_VALUE], so replaced elements are easy to see

// Median time of RUNS runs in ms. replace_if changes the data, so every run
// gets a fresh copy of source; copying is not timed.
template <typename Func>
double measure(const std::vector<int>& source, const std::vector<int>& expected, Func func,
               bool& correct) {
    std::vector<double> times;

    for (int run = 0; run < RUNS; run++) {
        std::vector<int> data = source;

        auto start = std::chrono::steady_clock::now();
        func(data);
        auto finish = std::chrono::steady_clock::now();
        times.push_back(std::chrono::duration<double, std::milli>(finish - start).count());

        if (data != expected) {
            correct = false;
        }
    }

    std::sort(times.begin(), times.end());
    return times[RUNS / 2];
}

std::vector<size_t> makeKValues(size_t threads);
void printKTable(const std::vector<size_t>& ks, const std::vector<double>& times, size_t threads);

template <typename Policy, typename Pred>
double timeWithPolicy(Policy policy, const std::vector<int>& source,
                      const std::vector<int>& expected, Pred pred, bool& correct) {
    return measure(source, expected, [&](std::vector<int>& data) {
        std::replace_if(policy, data.begin(), data.end(), pred, NEW_VALUE);
    }, correct);
}

template <typename Pred>
void testLibrary(const std::vector<int>& source, const std::vector<int>& expected, Pred pred,
                 bool& correct) {
    double noPolicy = measure(source, expected, [&](std::vector<int>& data) {
        std::replace_if(data.begin(), data.end(), pred, NEW_VALUE);
    }, correct);
    std::cout << std::format("  no policy: {:.3f}\n", noPolicy);

    double seq = timeWithPolicy(std::execution::seq, source, expected, pred, correct);
    double unseq = timeWithPolicy(std::execution::unseq, source, expected, pred, correct);
    double par = timeWithPolicy(std::execution::par, source, expected, pred, correct);
    double parUnseq = timeWithPolicy(std::execution::par_unseq, source, expected, pred, correct);
    std::cout << std::format("  seq: {:.3f}, unseq: {:.3f}, par: {:.3f}, par_unseq: {:.3f}\n",
                             seq, unseq, par, parUnseq);
}

template <typename Pred>
void testOwnAlgorithm(const std::vector<int>& source, const std::vector<int>& expected,
                      Pred pred, size_t threads, bool& correct) {
    std::vector<size_t> ks = makeKValues(threads);
    std::vector<double> times;

    for (size_t k : ks) {
        times.push_back(measure(source, expected, [&](std::vector<int>& data) {
            parallelReplaceIf(data.begin(), data.end(), pred, NEW_VALUE, k);
        }, correct));
    }
    printKTable(ks, times, threads);
}

template <typename Pred>
void runExperiments(const std::string& predName, const std::vector<int>& source, Pred pred,
                    size_t threads) {
    std::vector<int> expected = source;
    std::replace_if(expected.begin(), expected.end(), pred, NEW_VALUE);

    bool correct = true;
    std::cout << "\n  " << predName << "\n";
    testLibrary(source, expected, pred, correct);
    testOwnAlgorithm(source, expected, pred, threads, correct);
    std::cout << "  results " << (correct ? "correct" : "WRONG") << "\n";
}
