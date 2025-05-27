# Cool

## Overview

This repository contains my implementation of a compiler for the Classroom Object-Oriented Language (COOL) developed as part of [Stanford's CS143 Compilers](http://web.stanford.edu/class/cs143/) course. The compiler translates COOL programs to MIPS assembly code through a series of well-defined phases.

## Dependencies

+ [CMake](https://cmake.org/) (>= 3.10): Build system
+ [re2c](https://re2c.org/) (>= 0.14): Lexer generator
+ **C++17** compatible compiler (GCC/Clang)
+ [GoogleTest](https://google.github.io/googletest/) (>= 1.10): Only required for testing

## Building the Project

```bash
cmake -B build -S .
cmake --build build
```

## Running Tests

```bash
ctest --test-dir build
```
