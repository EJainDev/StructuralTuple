# Structural Tuple
> A simple, structural variant of std::tuple for use in compile time evaluated contexts

## Contents

- [Quick Start](#quick-start)
- [Installation](#installation)
    - [CMake FetchContent (Recommended)](#cmake-fetchcontent-recommended)
    - [Build From Source](#build-from-source)
- [Usage](#usage)
    - [Constructors](#constructors)
    - [Member Functions](#member-functions)
    - [tuple_cat](#tuple_cat)
- [Motivation](#motivation)

## Quick Start

Before:
```cpp
import std;

int main() {
    auto my_tuple = std::make_tuple(3, 4.2F);

    std::println("{}", my_tuple);

    return 0;
}
```

After:
```cpp
import std;

import structural_tuple;

int main() {
    auto my_tuple = structural_tuple::tuple(3, 4.2F);

    std::println("{}", my_tuple);

    return 0;
}
```

## Installation

### CMake FetchContent (Recommended)

This project uses C++26 modules and reflection.

Requirements:
- CMake 4.3+
- A compiler with C++26 modules and reflection support

Example:
```cmake
include(FetchContent)

FetchContent_Declare(
    structural_tuple
    GIT_REPOSITORY https://github.com/EJainDev/StructuralTuple.git
    GIT_TAG main
)

FetchContent_MakeAvailable(StructuralTuple)

add_executable(example main.cpp)
target_link_libraries(example PRIVATE structural_tuple::structural_tuple)
target_compile_features(example PRIVATE cxx_std_26)
```

In your source file:
```cpp
import structural_tuple;
```

### Build From Source

Configure and build with the provided presets:

```bash
cmake --preset release-gcc-16
cmake --build --preset build-release # or cmake --build build/release
```

Install locally:

```bash
cmake --install build/release
```

Build and run tests:

```bash
cmake --preset debug-gcc-16 \
      -DBUILD_TESTS=ON

cmake --build build/debug
ctest --test-dir build/debug --output-on-failure
```

Use as an installed package:

```cmake
find_package(StructuralTuple REQUIRED)

add_executable(example main.cpp)
target_link_libraries(example PRIVATE structural_tuple::structural_tuple)
target_compile_features(example PRIVATE cxx_std_26)
```

## Usage

### Constructors

You can construct tuples with explicit types:

```cpp
import structural_tuple;

structural_tuple::tuple<int, double> a{1, 2.5};
```

Or use class template argument deduction:

```cpp
import structural_tuple;

auto b = structural_tuple::tuple{3, 4.0F, true};
```

Empty tuples are supported:

```cpp
import structural_tuple;

structural_tuple::tuple<> empty{};
```

### Member Functions

Access by index:

```cpp
import std;
import structural_tuple;

structural_tuple::tuple<int, std::string> t{42, "hello"};

auto& first = t.get<0>();
const auto& second = t.get<1>();
```

Free `get` overloads also work, including structured bindings and tuple-like traits:

```cpp
import std;
import structural_tuple;

structural_tuple::tuple<int, double, std::string> t{1, 2.5, "x"};

static_assert(std::tuple_size_v<decltype(t)> == 3);
static_assert(std::is_same_v<std::tuple_element_t<1, decltype(t)>, double>);

auto [x, y, z] = t;
auto v = structural_tuple::get<0>(t);
```

`begin()` and `end()` expose member reflection metadata for compile time iteration.

### tuple_cat

This library contains an optimized `tuple_cat` that leverages reflection to avoid heavy template usage. `tuple_cat` concatenates structural tuples into a new structural tuple:

```cpp
import std;
import structural_tuple;

structural_tuple::tuple<int, double> left{1, 2.5};
structural_tuple::tuple<std::string, bool> right{"hello", true};

auto combined = structural_tuple::tuple_cat(left, right);

static_assert(std::tuple_size_v<decltype(combined)> == 4);
```

However, you can still use the standard library implementation.

## Motivation

C++26 reflection significantly enhances compile time capabilities. With this, it becomes more relevant to return multiple values out of consteval functions for runtime code. However, to use `std::define_static_*` on the return value, the type of the return value must be structural. When creating arrays or objects with tuples, `std::tuple` makes that impossible across different compilers. Therefore, this tuple type is completely structural, supports for-each loops (with a constraint of course), and meets the standard tuple-like object constraints.