#include "poisson.h"
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

// Largest mean for which exp(-mean) doesnt overflow double.

#define POISSON_MAX_MEAN 700.0

int pmf(double mu, size_t n, double *out) {
  if (mu < 0 || !out || mu > POISSON_MAX_MEAN)
    return -1;
  // p_j = e^(-m) · m^j / j!
  out[0] = exp(-mu);
  for (size_t i = 1; i <= n; i++) {
    out[i] = out[i - 1] * mu / i;
  }
  return 0;
}

double tail(const double *pmf, size_t n) {
  if (!pmf || n < 0)
    return -1.0;

  double sum = 0.0;
  for (size_t i = 0; i <= n; i++)
    sum += pmf[i];
  double tail = 1.0 - sum;
  return tail < 0.0 ? 0.0 : tail;
}
