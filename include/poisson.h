#ifndef POISSON_H
#define POISSON_H
#include <stddef.h>

// probabilities for a positive integer number of batch sizes
int pmf(double mu, size_t, double *out);

// needed to calculate arrivals after bacth size for "queue full" conditions
double tail(const double *pmf, size_t);

double poisson_term(double mu, size_t);

#endif // !POISSON_H
