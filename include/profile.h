#ifndef PROFILE_H
#define PROFILE_H
#include <stddef.h>

typedef struct {
  double v;  // extra cost per additional image; seconds/image
  double t0; // fixed cost per batch; seconds
  double k;  // Size of Image; MB
  double m0; // Memory needed for loading the model; MB
  size_t B;  // max batch size; int or service_time
} GPUProfile;

typedef struct {
  size_t r;            // batch size
  double t_r;          // service_time, s
  double mu_r;         // service_rate, batches/s
  double throughput_r; // throughput, images/s
  double m_r;          // memory; MB (even though its very less)
} Profile;

typedef struct {
  size_t B;
  Profile *rows; // malloc B+1 entries
} Profiles;

// r is the batch size actually used in a single run
// t_r = v . r + t0; t_r is timing real batches of varying sizes r.
// dk how to measure this as of right now exactly
// can be improved upon
//
// r is always taken for every run

Profiles *build(const GPUProfile *);
void free_profiles(Profiles *);

double service_time(const GPUProfile *, size_t); // t_r , seconds
double service_rate(double t_r);                 // mu_r, batches/seconds
double memory_usage(const GPUProfile *, size_t); // m_r, MB
double throughput(double mu_r, size_t);          // r * mu_r, images/s
double max_arrival_rate(const GPUProfile *);     // B * mu_B, tasks/s
double traffic_intensity(const GPUProfile *, double lambda); // rho
size_t knee_batch(const GPUProfile *, double lambda); // b, in range [1, B]

#endif
