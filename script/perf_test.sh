#!/bin/sh

cmake -S . -B build -D CMAKE_BUILD_TYPE=Release
cmake --build build -j

rm -rf data/ecoli-reads.fa data/ecoli.fa.3*

# should be around 2s
/usr/bin/time -al ./build/build_index ./data/ecoli.fa 3 50 > /dev/null
python3 tests/mock_reads_from_refernce.py ./data/ecoli.fa ./data/ecoli-reads.fa 20000 150 2

# should be around 0.5s
/usr/bin/time -al ./build/sbwt ./data/ecoli-reads.fa ./data/ecoli.fa.3 > /dev/null
