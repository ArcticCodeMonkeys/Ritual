# Building Ritual

## Prerequisites

- CMake 3.15 or higher
- C++17 compatible compiler
- Make (or another build system supported by CMake)

## Quick Build

Use the provided build script:

```bash
./build.sh
```

The executable will be created at `build/bin/ritual`.

## Manual Build

If you prefer to build manually:

```bash
# Create build directory
mkdir -p build
cd build

# Configure with CMake
cmake -DCMAKE_BUILD_TYPE=Release ..

# Build
make -j$(sysctl -n hw.ncpu)
```

## Build Options

### Debug Build

```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
```

### Custom Options

```bash
# Build with verbose output
cmake -DCMAKE_BUILD_TYPE=Release ..
make VERBOSE=1

# Clean build
make clean
```

## Running

After building, run the executable:

```bash
./build/bin/ritual
```

Or from the build directory:

```bash
cd build/bin
./ritual
```

## Project Structure

- `src/` - Source files
  - `main.cpp` - Main entry point
  - `core/` - Core functionality
    - `Parser.cpp` - Ritual parsing logic
    - Header files for various components
- `external/efsw/` - File watcher library
- `build/` - Build output directory (created during build)
  - `bin/` - Compiled executables

## Troubleshooting

If you encounter build errors:

1. Make sure you have CMake 3.15+ installed: `cmake --version`
2. Ensure you have a C++17 compatible compiler
3. Try cleaning the build directory: `rm -rf build && mkdir build`
4. Check that the efsw submodule is properly initialized
