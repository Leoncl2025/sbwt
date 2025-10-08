#ifndef SBWT_UTILITY_SORT_H
#define SBWT_UTILITY_SORT_H

#include <stdlib.h>
#include <cstdint>
#include <algorithm>
#include <vector>

#include "../log.h"

using namespace std;

namespace utility
{
    inline char GetCharSA(uint32_t index, const char *seq, const uint32_t *seq_index, const uint32_t &depth, const uint32_t &length_ref)
    {
        uint32_t idx = seq_index[index] + depth;
        return seq[idx < length_ref ? idx : (idx % length_ref)];
    }

    inline void SwapSuffixArray(uint32_t *seq_index, uint32_t i, uint32_t j)
    {
        if (i == j)
            return;
        auto temp = seq_index[i];
        seq_index[i] = seq_index[j];
        seq_index[j] = temp;
    }

    // get count of $, A, C, G, T in [begin, end)
    inline void CountCharsInRange(
        const char *seq,            /* sequence */
        const uint32_t *seq_index,  /* suffix array */
        const uint32_t &step,       /* a.k.a period*/
        const uint32_t &length_ref, /* length of reference sequence */
        const uint32_t &depth,      /* current depth */
        const uint32_t &begin,      /* begin index in suffix array */
        const uint32_t &end,        /* end index in suffix array */
        uint32_t &dollarCount,
        uint32_t &aCount,
        uint32_t &cCount,
        uint32_t &gCount,
        uint32_t &tCount)
    {
        dollarCount = 0;
        aCount = 0;
        cCount = 0;
        gCount = 0;
        tCount = 0;
        for (uint32_t i = begin; i < end; ++i)
        {
            char c = GetCharSA(i, seq, seq_index, depth, length_ref);
            switch (c)
            {
            case '$':
                ++dollarCount;
                break;
            case 'A':
                ++aCount;
                break;
            case 'C':
                ++cCount;
                break;
            case 'G':
                ++gCount;
                break;
            case 'T':
                ++tCount;
                break;
            default:
                LOGERROR("Invalid character in CountCharsInRange: " << c);
                throw std::runtime_error("Invalid character in CountCharsInRange");
            }
        }
    }

    inline void SortSuffixArrayOneStep(
        const char *seq,            /* sequence */
        uint32_t *seq_index,        /* suffix array */
        const uint32_t &step,       /* a.k.a period*/
        const uint32_t &length_ref, /* length of reference sequence */
        const uint32_t &depth,      /* current depth */
        const uint32_t &begin,      /* begin index in suffix array */
        const uint32_t &end,        /* end index in suffix array */
        const uint32_t &count,      /* count of current character */
        char chr                    /* current character */
    )
    {
        if (count == 0 || begin >= end || depth >= length_ref)
            return;
        uint32_t curIdx = begin;
        for (uint32_t i = begin; i < end && (curIdx - begin) < count; ++i)
        {
            char c = GetCharSA(i, seq, seq_index, depth, length_ref);
            if (c == chr)
            {
                SwapSuffixArray(seq_index, i, curIdx);
                curIdx++;
            }
        }

        if (chr == '$')
        {
            std::sort(seq_index + begin, seq_index + begin + count,
                      [&](uint32_t a, uint32_t b)
                      {
                          return a < b;
                      });
        }
    }

    inline void SortSuffixArrayOneStep(
        const char *seq,            /* sequence */
        uint32_t *seq_index,        /* suffix array */
        const uint32_t &step,       /* a.k.a period*/
        const uint32_t &length_ref, /* length of reference sequence */
        const uint32_t &depth,      /* current depth */
        const uint32_t &begin,      /* begin index in suffix array */
        const uint32_t &end         /* end index in suffix array */
    )
    {
        if (begin + 1 >= end || depth >= length_ref)
            return;
        uint32_t dollarCount, aCount, cCount, gCount, tCount;
        CountCharsInRange(seq, seq_index, step, length_ref, depth, begin, end, dollarCount, aCount, cCount, gCount, tCount);
        uint32_t sortBegin = begin;
        SortSuffixArrayOneStep(seq, seq_index, step, length_ref, depth, sortBegin, end, dollarCount, '$');
        sortBegin += dollarCount;
        SortSuffixArrayOneStep(seq, seq_index, step, length_ref, depth, sortBegin, end, aCount, 'A');
        sortBegin += aCount;
        SortSuffixArrayOneStep(seq, seq_index, step, length_ref, depth, sortBegin, end, cCount, 'C');
        sortBegin += cCount;
        SortSuffixArrayOneStep(seq, seq_index, step, length_ref, depth, sortBegin, end, gCount, 'G');
    }

    // Sort suffix array
    inline void SortSuffixArray(
        const char *seq,           /* sequence */
        uint32_t *seq_index,       /* suffix array */
        const uint32_t &step,      /* a.k.a period*/
        const uint32_t &length_ref /* length of reference sequence */
    )
    {
        vector<pair<uint32_t, uint32_t>> ranges;
        vector<pair<uint32_t, uint32_t>> new_ranges;
        uint32_t beg = 0;
        uint32_t end = 0;
        ranges.push_back({0, length_ref});
        LOGINFO("Sorting suffix array of length " << length_ref << " with step " << step << "\n");
        for (uint32_t i = 0; i < length_ref && ranges.size() > 0; i += step)
        {
            if (i % 100 < step)
            {
                LOGINFO("Sorting at depth " << i << "\trange size\t" << ranges.size() << std::endl);
            }
            new_ranges.clear();
            for (auto &p : ranges)
            {
                beg = p.first;
                end = p.second;
                if (end <= 1 + beg)
                    continue;
                SortSuffixArrayOneStep(seq, seq_index, step, length_ref, i, beg, end);
                // find equal ranges in [beg, end)
                uint32_t j = beg + 1;
                while (j < end)
                {
                    char c0 = seq[(seq_index[j - 1] + i) % length_ref];
                    char c1 = seq[(seq_index[j] + i) % length_ref];
                    if (c0 == c1)
                    {
                        uint32_t k = j + 1;
                        while (k < end)
                        {
                            char c2 = seq[(seq_index[k] + i) % length_ref];
                            if (c1 != c2)
                                break;
                            ++k;
                        }
                        new_ranges.push_back({j - 1, k});
                        j = k + 1;
                    }
                    else
                    {
                        ++j;
                    }
                }
            }
            ranges.swap(new_ranges);
        }
        return;
    }
} /* namespace utility */
#endif /* SBWT_UTILITY_SORT_H */
