# Spaced Burrows Wheeler Transform

## Compile and confugration on Linux
```
mkdir release
cd release
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

## Build and test

### prerequisite
Install cmake and g++ (version >= 11)

### Build
```bash
cmake -S . -B build -D CMAKE_BUILD_TYPE=Release
cmake --build build -j
```

## Running tests (CTest)

```bash
ctest --test-dir build -V
```