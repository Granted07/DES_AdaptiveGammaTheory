# BatOpt 2.0 — adaptive γ for GPU batch inference

A from-scratch C implementation of the ideas in **BatOpt** (Zhang et al.,
*IEEE Transactions on Cloud Computing*, vol. 12, no. 1, 2024,
DOI [10.1109/TCC.2024.3350561](https://doi.org/10.1109/TCC.2024.3350561)),
extended with an **adaptive trade-off parameter γ(t)**.

BatOpt serves deep-learning inference requests on a GPU by grouping waiting
requests into batches. It picks the batch-size upper bound `b` that minimizes

```
E(W_b) + γ · m_b
```

where `E(W_b)` is the average service latency (s), `m_b` is the peak GPU memory
(MB) and γ (s/MB) is the exchange rate between the two. The paper fixes
γ = 0.1. This project asks: *what if γ follows the current GPU memory
pressure instead?*

## Status

| Step | What | State |
|---|---|---|
| 1 | GPU profile structs and formulas (`profile.h/.c`) | in progress |
| 2 | Profiling benchmark (measure τ_r and m_r for r = 1..B) | planned |
| 3 | Poisson load generator | planned |
| 4 | Real batching server on the GPU | planned |
| 5 | Queueing-model predictor of E(W_b), validated on real runs | planned |
| 6 | Adaptive γ(t) vs static γ | planned |

See [docs/roadmap.md](docs/roadmap.md) for details.

## Layout

```
.
├── CMakeLists.txt
├── include/profile.h      # structs and function declarations
├── src/profile.c          # implementations
├── tests/test_profile.c   # self-checking tests (one executable per test_*.c)
└── docs/                  # model, API reference, roadmap
```

## Build and test

Requires CMake ≥ 3.16 and a C11 compiler (gcc/MinGW, clang or MSVC).

```sh
cmake -S . -B build
cmake --build build
./build/test_profile        # build\test_profile.exe on Windows
```

On Windows with MinGW, add `-G "MinGW Makefiles"` (or `-G Ninja`) to the first
command. With MSVC the executable is under `build\Debug\`.

`test_profile` prints one line per check and exits with code 1 on any failure.

In Neovim with cmake-tools.nvim: `:CMakeGenerate`, then build and run through
the plugin.

## Documentation

- [docs/model.md](docs/model.md): the theory, symbols and units
- [docs/api.md](docs/api.md): function reference for `profile.h`
- [docs/roadmap.md](docs/roadmap.md): the build plan

## Notes on the numbers

The profile constants used in the tests (`v = 0.000679`, `t0 = 0.005`,
`k = 15`, `m0 = 500`, `B = 64`) are **illustrative**. They roughly reproduce
the paper's GoogLeNet / RTX 2080 operating point and are not measurements.
Real values will come from the profiling benchmark (Step 2).

## License

TBD.
