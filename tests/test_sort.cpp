#include <cassert>
#include <cstdint>
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

#include "utils/sort.hpp"

// Helper to build naive suffix array (cyclic like implementation used in SortSuffixArrayOneStep)
static std::vector<uint32_t> naive_suffix_array_cyclic(const std::string &s, const uint32_t step=1) {
    const uint32_t n = static_cast<uint32_t>(s.size());
    std::vector<uint32_t> sa(n);
    for (uint32_t i = 0; i < n; ++i) sa[i] = i;
    std::sort(sa.begin(), sa.end(), [&](uint32_t a, uint32_t b){
        for (uint32_t d = 0; d < n; d+=step) {
            char ca = s[(a + d) % n];
            char cb = s[(b + d) % n];
            if (ca != cb) return ca < cb;
        }
        return a < b; // tie-breaker
    });
    return sa;
}

// Aggregate array comparison so that at most ONE assert(false) is triggered per test.
static void assert_equal_array(const std::vector<uint32_t> &got,
                               const std::vector<uint32_t> &expected,
                               const char *test_name) {
    if (got.size() != expected.size()) {
        std::cerr << "[" << test_name << "] size mismatch: expected " << expected.size()
                  << " got " << got.size() << std::endl;
        assert(false);
        throw;
    }
    size_t mismatches = 0;
    size_t first_idx = 0;
    for (size_t i = 0; i < got.size(); ++i) {
        if (got[i] != expected[i]) {
            if (mismatches == 0) first_idx = i;
            ++mismatches;
        }
    }
    if (mismatches) {
        std::cerr << "[" << test_name << "] mismatches=" << mismatches
                  << ", first at index=" << first_idx
                  << ": expected=" << expected[first_idx]
                  << " got=" << got[first_idx] << std::endl;
        assert(false);
        throw;
    }
}

static void test_one_step_basic() {
    std::string seq = "ACGT"; // length 4
    const uint32_t n = seq.size();
    std::vector<uint32_t> sa = {3,2,1,0}; // reversed initial order
    // perform depth 0 one step sort over whole range
    // Explicitly capture const char* to avoid any ambiguity with potential old declarations
    const char *seq_ptr = seq.c_str();
    utility::SortSuffixArrayOneStep(seq_ptr, sa.data(), 1 /*step*/, n, 0 /*depth*/, 0, n);
    // After sorting by first char cyclically at depth 0, order should be A(0), C(1), G(2), T(3)
    assert(sa[0] == 0 && sa[1] == 1 && sa[2] == 2 && sa[3] == 3);
}

static void test_one_step_stable_tie_break() {
    std::string seq = "AAAA"; // all same letters
    const uint32_t n = seq.size();
    std::vector<uint32_t> sa = {3,2,1,0};
    const char *seq_ptr = seq.c_str();
    utility::SortSuffixArrayOneStep(seq_ptr, sa.data(), 1, n, 0, 0, n);
    // all characters equal, comparator returns a<b as tie-break => ascending indices
    {
        std::vector<uint32_t> expected(n); for(uint32_t i=0;i<n;++i) expected[i]=i;
        assert_equal_array(sa, expected, "test_one_step_stable_tie_break");
    }
}

static void test_full_sort_simple() {
    std::string seq = "ACGT";
    const uint32_t n = seq.size();
    std::vector<uint32_t> sa(n);
    for (uint32_t i = 0; i < n; ++i) sa[i] = i;
    const char *seq_ptr = seq.c_str();
    utility::SortSuffixArray(seq_ptr, sa.data(), 1 /*step*/, n);
    // For a unique character string already in lexicographic order, result should remain identity
    {
        std::vector<uint32_t> expected(n); for(uint32_t i=0;i<n;++i) expected[i]=i;
        assert_equal_array(sa, expected, "test_full_sort_simple");
    }
}

static void test_full_sort_repeated_pattern() {
    std::string seq = "ATAT"; // cyclic comparisons matter
    const uint32_t n = seq.size();
    std::vector<uint32_t> sa(n); for (uint32_t i=0;i<n;++i) sa[i]=i;
    const char *seq_ptr = seq.c_str();
    utility::SortSuffixArray(seq_ptr, sa.data(), 1, n);

    // Build expected order by naive cyclic suffix ordering
    auto expected = naive_suffix_array_cyclic(seq);
    assert_equal_array(sa, expected, "test_full_sort_repeated_pattern");
}

static void test_full_sort_random_small() {
    std::string seq = "GATTACA"; // classic test string
    const uint32_t n = seq.size();
    std::vector<uint32_t> sa(n); for (uint32_t i=0;i<n;++i) sa[i]=i;
    const char *seq_ptr = seq.c_str();
    utility::SortSuffixArray(seq_ptr, sa.data(), 1, n);
    auto expected = naive_suffix_array_cyclic(seq);
    assert_equal_array(sa, expected, "test_full_sort_random_small");
}

static void test_full_sort_period2() {
    std::string seq = "ACAACGCCAA$$";
    const uint32_t n = seq.size();
    std::vector<uint32_t> sa(n); for(uint32_t i=0;i<n;++i) sa[i]=i;
    const char *seq_ptr = seq.c_str();
    utility::SortSuffixArray(seq_ptr, sa.data(), 2, n);
    std::vector<uint32_t> expected = naive_suffix_array_cyclic(seq, 2);
    assert_equal_array(sa, expected, "test_full_sort_forced_fail_optional");
}

static void test_full_sort_forced_fail_period3() {
    std::string seq = "ACAACGCCAAACGGGCTCGGTCGGCAGCTACGTAC$$$";
    const uint32_t n = seq.size();
    std::vector<uint32_t> sa(n); for(uint32_t i=0;i<n;++i) sa[i]=i;
    const char *seq_ptr = seq.c_str();
    utility::SortSuffixArray(seq_ptr, sa.data(), 3, n);
    std::vector<uint32_t> expected = naive_suffix_array_cyclic(seq, 3);
    assert_equal_array(sa, expected, "test_full_sort_forced_fail_period3");
}

int main() {
    test_one_step_basic();
    test_one_step_stable_tie_break();
    test_full_sort_simple();
    test_full_sort_repeated_pattern();
    test_full_sort_random_small();
    test_full_sort_period2();
    test_full_sort_forced_fail_period3();
    std::cout << "All sort unit tests passed" << std::endl;
    return 0;
}
