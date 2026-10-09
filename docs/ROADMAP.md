# Roadmap

The project runs against a real NVIDIA GPU (a laptop) rather than a simulator.
The real server is the ground truth; the queueing model is the fast predictor
that is validated against it.

| Step | What | Output |
|---|---|---|
| 1 | Profile structs and formulas | `profile.h/.c`, `test_profile.c` |
| 2 | Profiling benchmark: time batches of r = 1..B and record peak memory; fit v, τ₀, k, m₀ | a small constants file read by the C code |
| 3 | Load generator: exponential inter-arrival gaps, real timestamps | Poisson task stream |
| 4 | Real batching server: bounded queue (N), batcher with r = min(max(a, n), b), GPU worker | per-task latency, P_block, peak memory |
| 5 | Queueing-model predictor of E(W_b) (embedded Markov chain), checked against Step 4; the paper's Algorithm 1 as the static-γ baseline | predicted vs measured E(W) |
| 6 | Adaptive γ(t) driven by live memory pressure (NVML), compared against static γ under changing arrival rates | final comparison |

## Plan for the languages

- Profiling and load generation: Python + PyTorch (they only produce numbers).
- Model, optimizer and adaptive γ: C, reading the measured constants from file.

## Benchmarking hygiene (laptop GPU)

- Plug in; use the high-performance power mode.
- Make sure the program runs on the NVIDIA GPU, not the integrated one.
- Warm up before timing; average over many runs (throttling when hot).
- Close other GPU-heavy applications (they eat VRAM and skew memory numbers).
- Use `torch.cuda.max_memory_allocated()` consistently for memory.
