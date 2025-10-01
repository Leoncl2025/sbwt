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

## VS Code Debugging

Debug configuration files have been added under `.vscode/`:

Files:
- `tasks.json` – configure (`CMake: Configure (Debug)`) and build tasks for each target (`sbwt`, `build_index`, `count_occ`, `sbwt_test`) plus a `CTest: run all` task.
- `launch.json` – LLDB debugging (via `cppdbg`) launch configs for each C++ binary and Python `debugpy` configs for the end‑to‑end tests.
- `c_cpp_properties.json` – IntelliSense settings pointing to `build/compile_commands.json` (generated after first configure) and include paths for `src/` and `SeqAn-1.1`.

### One‑time setup
1. Install VS Code extensions:
	- C/C++ (ms-vscode.cpptools)
	- Python (ms-python.python)
2. Run the task: `CMake: Configure (Debug)` (Terminal > Run Task...). This creates the `build/` directory and `compile_commands.json`.

### Typical workflow
1. Build everything: run task `Build: all (Debug)` (default build task: Cmd+Shift+B).
2. Set breakpoints in source files (e.g. `src/sbwt_aligner.cpp`).
3. Start a debug session:
	- Choose a configuration (e.g. "Debug sbwt") from the Run and Debug panel and press F5.
4. Pass program arguments: edit the `args` array inside the corresponding entry in `launch.json` or use the gear icon to duplicate a config with custom arguments.

### Running end‑to‑end tests under the debugger
Select `Python e2e_build_index` or `Python e2e_sbwt` and press F5. These configs call the Python test scripts with paths to the built binaries so you can set breakpoints both in Python and C++ (use the C++ configs if you only need to step through C++ after producing test inputs).

### Regenerating compile_commands.json
If you change CMake options, rerun the `CMake: Configure (Debug)` task. IntelliSense will refresh automatically.

### Troubleshooting
- If symbols show as optimized out, ensure you selected a Debug build (`-DCMAKE_BUILD_TYPE=Debug`).
- If `compile_commands.json` is missing, re-run the configure task (it adds `-DCMAKE_EXPORT_COMPILE_COMMANDS=ON`).
- If the debugger type `cppdbg` is not recognized, confirm the C/C++ extension is installed and enabled.

## Running tests (CTest)

```bash
ctest --test-dir build -V
```