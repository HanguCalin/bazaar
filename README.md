# bazaar

[![CI](https://github.com/HanguCalin/bazaar/actions/workflows/ci.yml/badge.svg)](https://github.com/HanguCalin/bazaar/actions/workflows/ci.yml)

A small e-commerce backend in C++20, built over 8 weeks as interview
preparation. See [PLAN.md](PLAN.md) for the plan and `docs/WEEKn.md` for each
week.

## Build and test

Needs g++ 13 or newer, CMake 3.25 or newer and Ninja.

```
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
```
