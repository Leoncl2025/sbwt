#include "sort.hpp"
#include <algorithm>
#include <cstdint>
#include <vector>
#include "../log.h"

using namespace std;

namespace utility {
    void SortSuffixArray(
        const char *seq,               /* sequence */
        uint32_t *seq_index,        /* suffix array */
        const uint32_t &step,       /* a.k.a period*/
        const uint32_t &length_ref      /* length of reference sequence */
    )
    {
        vector<pair<uint32_t, uint32_t>> ranges;
        vector<pair<uint32_t, uint32_t>> new_ranges;
        uint32_t beg = 0;
        uint32_t end = 0;
        ranges.push_back({0, length_ref});
        LOGINFO("Sorting suffix array of length " << length_ref << " with step " << step << "\n");
        for (uint32_t i = 0; i < length_ref && ranges.size() > 0; i += step) {
            if (i % 100 < step) {
                LOGINFO("Sorting at depth " << i << "\trange size\t" << ranges.size() << std::endl);
            }
            new_ranges.clear();
            for (auto &p : ranges) {
                beg = p.first;
                end = p.second;
                if (end - beg <= 1) continue;
                SortSuffixArrayOneStep(seq, seq_index, step, length_ref, i, beg, end);
                // find equal ranges in [beg, end)
                uint32_t j = beg + 1;
                while (j < end) {
                    char c0 = seq[(seq_index[j - 1] + i) % length_ref];
                    char c1 = seq[(seq_index[j] + i) % length_ref];
                    if (c0 == c1) {
                        uint32_t k = j + 1;
                        while (k < end) {
                            char c2 = seq[(seq_index[k] + i) % length_ref];
                            if (c1 != c2) break;
                            ++k;
                        }
                        new_ranges.push_back({j - 1, k});
                        j = k + 1;
                    } else {
                        ++j;
                    }
                }
            }
            ranges.swap(new_ranges);
        }
    }
}