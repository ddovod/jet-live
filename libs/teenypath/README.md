# TeenyPath
[![As-Is](https://img.shields.io/badge/Support%20Level-As--Is-e8762c.svg)](https://www.tableau.com/support-levels-it-and-developer-tools) [![Build Status](https://travis-ci.org/tableau/teenypath.svg?branch=master)](https://travis-ci.org/tableau/teenypath)

TeenyPath is a small (single file) cross-platform (Windows, Apple, Linux) filesystem abstraction for C++.

* [Why TeenyPath?](#why-teenypath)
* [Demo](#demo)
* [Get started](#get-started)
  * [Prerequisites](#prerequisites)
  * [Compilation](#compilation)
  * [Link against](#link-against)
* [Support](#support)
* [Contributions](#contributions)

# Why TeenyPath?

TeenyPath lets you avoid using a gigantic library (Boost, Qt, the not-fully-done filesystem.h in future C++ standards) to write cross-platform code which has to touch the filesystem.

# Demo/Samples

```cpp
  using namespace TeenyPath;

  const path cwd = current_path();
  assert(cwd.is_directory());
  assert(!cwd.is_symlink());
  assert(!cwd.is_regular_file());
```

# Get started

This section describes how to compile and link against TeenyPath.

## Prerequistes

To work with TeenyPath, you need the following:

* A C++ compiler (clang, gcc, msvc).
* A C++ linker (lld, ld, link.exe).
* [CMake](https://cmake.org).

## Compilation

```
cmake -H. -B./build
cmake --build build
```

## Link against

Add `-I/path/to/teenypath.h` to your compilation command, and add `-L/path/to/libteenypath.so` to your command line options for linking.

# Support

TeenyPath is made available AS-IS with no support. Any bugs that you discover should be filed in the TeenyPath [issue tracker](/issues).

# Contributions

Code contributions and improvements by the community are welcomed!
See the LICENSE file for current open-source licensing and use information.

Before we can accept pull requests from contributors, we require a signed [Contributor License Agreement (CLA)](http://tableau.github.io/contributing.html),
