#include "profile.h"
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
double service_time(const GPUProfile *gpuprofile, size_t r) {
  return gpuprofile->v * r + gpuprofile->t0;
}

double service_rate(double t_r) { return 1 / t_r; }

double memory_usage(const GPUProfile *gpuprofile, size_t r) {
  return gpuprofile->k * r + gpuprofile->m0;
}

double throughput(double mu_r, size_t r) { return r * mu_r; }

double max_arrival_rate(const GPUProfile *gpuprofile) {
  if (!gpuprofile || gpuprofile->B < 1)
    return 0.0;
  double t_B = service_time(gpuprofile, gpuprofile->B);
  double mu_B = service_rate(t_B);
  return (double)gpuprofile->B * mu_B;
}

double traffic_intensity(const GPUProfile *gpuprofile, double lambda) {
  if (!gpuprofile || gpuprofile->B < 1)
    return 0.0;
  double lambda_max = max_arrival_rate(gpuprofile);
  if (lambda_max <= 0.0)
    return 1.0;
  return lambda / lambda_max;
}

int knee_batch(const GPUProfile *gpuprofile, double lambda) {
  if (!gpuprofile || gpuprofile->B < 1 || lambda <= 0.0)
    return 1;

  if (gpuprofile->v <= 0)
    return gpuprofile->B;
  if (lambda >= 1.0 / gpuprofile->v) {
    return gpuprofile->B;
  }

  // b = λ·τ₀ / (1 - λ·v)
  double numerator = lambda * gpuprofile->t0;
  double denominator = 1.0 - lambda * gpuprofile->v;

  if (denominator <= 0.0)
    return gpuprofile->B;

  double b_float = numerator / denominator;
  int b = (int)ceil(b_float);

  // Clamping to [1,B]
  if (b < 1)
    b = 1;
  if (b > gpuprofile->B)
    b = gpuprofile->B;

  return b;
}

Profiles *build(const GPUProfile *gpuprofile) {
  if (!gpuprofile || gpuprofile->B < 1)
    return NULL;

  Profiles *profiles = (Profiles *)malloc(sizeof(Profiles));
  if (!profiles)
    return NULL;
  profiles->rows = (Profile *)calloc(gpuprofile->B + 1, sizeof(Profile));
  if (!profiles->rows) {
    free_profiles(profiles);
    return NULL;
  }

  profiles->B = gpuprofile->B;

  for (size_t r = 1; r <= gpuprofile->B; r++) {
    Profile *profile = &profiles->rows[r];
    if (!profile) {
      fprintf(stderr, "failed to malloc profiler");
    }
    profile->r = r; // batch size for profile
    profile->t_r = service_time(gpuprofile, r);
    profile->mu_r =
        service_rate(profile->t_r); // service rate in batches/seconds
    profile->throughput_r =
        throughput(profile->mu_r, r); // throughput in imgs/seconds
    profile->m_r = memory_usage(gpuprofile, r);
  }
  return profiles;
}

void free_profiles(Profiles *profiles) {
  if (!profiles)
    return;
  free(profiles->rows);
  free(profiles);
}
