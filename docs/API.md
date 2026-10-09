# API reference (`include/profile.h`)

All batch sizes `r` must satisfy 1 ≤ r ≤ `B`. Times are seconds, memory is MB.

## Types

```c
typedef struct {
  double v;    // extra cost per additional image; s/image
  double t0;   // fixed cost per batch; s
  double k;    // extra GPU memory per image in the batch; MB/image
  double m0;   // memory for the loaded model; MB
  size_t B;    // max batch size
} GPUProfile;      // inputs: set once, passed as const

typedef struct {
  size_t r;
  double t_r;           // service time, s
  double mu_r;          // service rate, batches/s
  double throughput_r;  // images/s
  double m_r;           // MB
} Profile;         // outputs for one batch size

typedef struct {
  size_t B;
  Profile *rows;        // B+1 entries; rows[r] is batch size r; rows[0] unused
} Profiles;
```

## Functions

| Function | Returns | Unit |
|---|---|---|
| `double service_time(const GPUProfile *, size_t r)` | τ_r = v·r + τ₀ | s |
| `double service_rate(double t_r)` | μ_r = 1 / τ_r | batches/s |
| `double throughput(double mu_r, size_t r)` | r·μ_r | images/s |
| `double memory_usage(const GPUProfile *, size_t r)` | m_r = k·r + m₀ | MB |
| `double max_arrival_rate(const GPUProfile *)` | B·μ_B | tasks/s |
| `double traffic_intensity(const GPUProfile *, double lambda)` | λ / (B·μ_B) | none |
| `int knee_batch(const GPUProfile *, double lambda)` | ⌈λτ₀ / (1 − λv)⌉ clamped to [1, B]; B if λ ≥ 1/v | images |
| `Profiles *build(const GPUProfile *)` | table of all rows, or `NULL` on bad input / allocation failure | |
| `void free_profiles(Profiles *)` | frees the table; safe on `NULL` | |

## Usage

```c
GPUProfile gp = { .v = 0.000679, .t0 = 0.005, .k = 15.0, .m0 = 500.0, .B = 64 };

int b = knee_batch(&gp, 990.0);          // 16

Profiles *t = build(&gp);
if (t) {
  double thr = t->rows[16].throughput_r; // about 1008.6 images/s
  free_profiles(t);
}
```

## Conventions

- `const GPUProfile *` is the GPU's constants; functions never modify it.
- The table is indexed directly by batch size: `rows[r]`.
- Library functions never call `exit`; failures are reported by return value.
