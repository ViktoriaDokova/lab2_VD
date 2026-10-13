#pragma once

#include <algorithm>
#include <cstddef>
#include <thread>
#include <vector>

template <typename It, typename Pred, typename T>
void parallelReplaceIf(It first, It last, Pred pred, const T& newValue, size_t k) {
    size_t n = static_cast<size_t>(last - first);
    size_t partSize = n / k;
    size_t rest = n % k;  // the first `rest` parts get one extra element

    std::vector<std::thread> threads;
    It begin = first;
    for (size_t i = 0; i < k; i++) {
        size_t length = partSize + (i < rest ? 1 : 0);
        It end = begin + static_cast<ptrdiff_t>(length);
        threads.push_back(std::thread([begin, end, pred, &newValue]() {
            std::replace_if(begin, end, pred, newValue);
        }));
        begin = end;
    }

    // replace_if works in place and the parts don't overlap,
    // so after join the whole range is already processed
    for (std::thread& t : threads) {
        t.join();
    }
}
