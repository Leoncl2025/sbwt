#ifndef SBWT_UTILITY_SORT_H
#define SBWT_UTILITY_SORT_H

#include <stdlib.h>
#include <cstdint>
#include <algorithm>

namespace utility {
    /// Sort suffix array one step. The sorting is done by depth-th character.
    /// If two suffixes are the same, the one with smaller index comes first.
    inline void SortSuffixArrayOneStep(
        const char *seq,          /* sequence */
        uint32_t *seq_index,      /* suffix array */
        const uint32_t &step,     /* a.k.a period (kept for interface) */
        const uint32_t &length_ref, /* length of reference sequence */
        uint32_t depth,           /* depth of sorting */
        uint32_t begin,           /* start position of sorting */
        uint32_t end              /* end position of sorting */
    ) {
        (void)step; // currently unused in this function
        if (depth >= length_ref || begin + 1 >= end) return;
        std::sort(seq_index + begin, seq_index + end,
            [seq, length_ref, depth](uint32_t a, uint32_t b) {
                char ca = seq[(a + depth) % length_ref];
                char cb = seq[(b + depth) % length_ref];
                if (ca != cb) return ca < cb;
                return a < b;
            });
    }

    // Sort suffix array
    void SortSuffixArray(
        const char *seq,               /* sequence */
        uint32_t *seq_index,        /* suffix array */
        const uint32_t &step,       /* a.k.a period*/
        const uint32_t &length_ref  /* length of reference sequence */
    );
} /* namespace utility */
#endif /* SBWT_UTILITY_SORT_H */
