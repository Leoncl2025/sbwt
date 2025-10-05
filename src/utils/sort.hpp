#ifndef SBWT_UTILITY_SORT_H
#define SBWT_UTILITY_SORT_H

#include <stdlib.h>
#include <cstdint>
#include <algorithm>

namespace utility {
    // Sort suffix array
    inline void SortSuffixArray(
        const char *seq,           /* sequence */
        uint32_t *seq_index,       /* suffix array */
        const uint32_t &step,      /* a.k.a period*/
        const uint32_t &length_ref /* length of reference sequence */
    )
    {
        std::sort(seq_index, seq_index + length_ref,
            [&](uint32_t a, uint32_t b)
            {
                for (uint32_t d = 0; d < length_ref; d += step)
                {
                    char ca = seq[(a + d) % length_ref];
                    char cb = seq[(b + d) % length_ref];
                    if (ca != cb)
                        return ca < cb;
                }
                return a < b;
            });
        return;
    }
} /* namespace utility */
#endif /* SBWT_UTILITY_SORT_H */
