# Model

## Symbols and units

| Symbol | Code | Meaning | Unit |
|---|---|---|---|
| r | `r` | batch size actually used for one GPU pass | images |
| a, b | | lower / upper bound of the batch size (a ≤ r ≤ b) | images |
| B | `B` | hard limit on b (GPU memory) | images |
| N | | maximum number of waiting tasks in the queue | tasks |
| λ | `lambda` | task arrival rate (Poisson) | tasks/s |
| v | `v` | extra service time per additional image | s/image |
| τ₀ | `t0` | fixed service time per batch | s |
| τ_r | `t_r` | service time of a batch of size r | s |
| μ_r | `mu_r` | service rate at batch size r | batches/s |
| k | `k` | extra GPU memory per image in the batch | MB/image |
| m₀ | `m0` | GPU memory with only the model loaded | MB |
| m_r | `m_r` | peak GPU memory at batch size r | MB |
| ρ | | traffic intensity, λ / (B·μ_B) | dimensionless |
| γ | | latency/memory exchange rate | s/MB |

## Batch service model

The GPU returns a whole batch at once, and the time is linear in r:

```
τ_r = v·r + τ₀            μ_r = 1 / τ_r            throughput(r) = r·μ_r
m_r = k·r + m₀
```

Units to keep straight: `μ_r` counts *batches* per second, `throughput`
counts *images* per second.

Maximum sustainable arrival rate (images/s): `B·μ_B`.
Traffic intensity: `ρ = λ / (B·μ_B)`, assumed ≤ 1.

## The knee batch size

The knee is the smallest b whose maximum throughput keeps up with arrivals:

```
b / (v·b + τ₀) = λ
b = λ(v·b + τ₀)
b(1 − λ·v) = λ·τ₀
b = λ·τ₀ / (1 − λ·v)
```

round up to an integer and clamp to [1, B]. It exists only for λ < 1/v, since
1/v is the limit of throughput as b → ∞. For λ ≥ 1/v the code returns B.

The paper's Proposition 1 says E(W_b) changes sharply with b near this point,
so the optimum is most likely at the knee, and Algorithm 1 searches outward
from it instead of scanning all b.

## Optimization problem (paper)

```
minimize over b:  E(W_b) + γ · m_b
subject to:       1 ≤ b ≤ B,   N ≥ B,   λ/(B·μ_B) ≤ 1,   γ > 0
```

E(W_b) comes from the paper's M/G(a,b)/1/N queueing model (an embedded
two-dimensional Markov chain on (queue length, batch size)).

## Adaptive γ (this project's extension)

Idea: γ is the price of memory in seconds per MB. With plenty of free VRAM the
price should be low (large b, low latency); under memory pressure it should be
high (smaller b). The exact form of γ(t) as a function of the live memory
signal is **not yet fixed** and will be designed and evaluated in Step 6.
The live signal is planned to come from NVML (`nvmlDeviceGetMemoryInfo`).

## Known issues in the paper's Algorithm 1

To be handled when the optimizer is implemented (Step 5/6):

- Line 5 computes `R ← E(W_b) + m_b` without γ; the objective in (OP) is
  `E(W_b) + γ·m_b`.
- For b = 1, `E(W_{b−1})` is defined as +∞ because a batch size of 0 stops
  service.
- Units differ between latency (s) and memory (MB), so γ carries units of s/MB.
