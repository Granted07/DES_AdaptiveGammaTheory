#include "poisson.h"
#include <math.h>
#include <stdio.h>

static int failures = 0;

static void check_close(const char *name, double got, double want, double tol) {
  if (fabs(got - want) <= tol) {
    printf("  ok    %-42s %.10g\n", name, got);
  } else {
    printf("  FAIL  %-42s got %.10g, want %.10g (tol %g)\n", name, got, want, tol);
    failures++;
  }
}

static void check_true(const char *name, int cond) {
  if (cond) {
    printf("  ok    %s\n", name);
  } else {
    printf("  FAIL  %s\n", name);
    failures++;
  }
}

int main(void) {
  double a[4];
  double one[1];
  double big[201];

  printf("mu = 0\n");
  check_true("pmf returns 0", pmf(0.0, 3, a) == 0);
  check_close("p_0 = 1", a[0], 1.0, 1e-15);
  check_close("p_1 = 0", a[1], 0.0, 1e-15);
  check_close("p_3 = 0", a[3], 0.0, 1e-15);
  check_close("tail beyond j=3 = 0", tail(a, 3), 0.0, 1e-15);

  printf("mu = 2 (hand-computed values)\n");
  check_true("pmf returns 0", pmf(2.0, 3, a) == 0);
  check_close("p_0", a[0], 0.1353352832, 1e-9);
  check_close("p_1", a[1], 0.2706705664, 1e-9);
  check_close("p_2", a[2], 0.2706705664, 1e-9);
  check_close("p_3", a[3], 0.1804470443, 1e-9);
  check_close("tail beyond j=3", tail(a, 3), 0.1428765397, 1e-9);
  check_close("tail beyond j=1", tail(a, 1), 0.5939941504, 1e-9);

  printf("n = 0 (table with a single entry)\n");
  check_true("pmf(2, 0) returns 0", pmf(2.0, 0, one) == 0);
  check_close("p_0", one[0], 0.1353352832, 1e-9);
  check_close("tail beyond j=0 = 1 - e^-2", tail(one, 0), 0.8646647168, 1e-9);

  printf("mu = 48, n = 200 (lambda=990, tau_64=0.0485)\n");
  check_true("pmf returns 0", pmf(48.0, 200, big) == 0);
  double sum = 0.0, m1 = 0.0, m2 = 0.0;
  int all_ok = 1;
  for (int j = 0; j <= 200; j++) {
    if (!isfinite(big[j]) || big[j] < 0.0) all_ok = 0;
    sum += big[j];
    m1 += j * big[j];
    m2 += (double)j * j * big[j];
  }
  check_true("every entry finite and >= 0", all_ok);
  check_close("sum of p_j = 1", sum, 1.0, 1e-12);
  check_close("mean = 48", m1, 48.0, 1e-9);
  check_close("variance = 48 (Poisson: variance = mean)", m2 - m1 * m1, 48.0, 1e-8);
  check_true("tail never negative", tail(big, 200) >= 0.0);
  check_close("tail beyond j=200 is ~0", tail(big, 200), 0.0, 1e-12);

  printf("limit of mu\n");
  check_true("mu = 700 accepted", pmf(700.0, 3, a) == 0);
  check_true("p_0 at mu = 700 is a normal positive double",
             a[0] >= 2.2250738585072014e-308);
  check_true("mu = 700.5 rejected", pmf(700.5, 3, a) == -1);
  check_true("mu = 800 rejected", pmf(800.0, 3, a) == -1);

  printf("invalid input\n");
  check_true("NULL out", pmf(1.0, 3, NULL) == -1);
  check_true("negative mu", pmf(-1.0, 3, a) == -1);
  check_true("tail with NULL table", tail(NULL, 3) == -1.0);

  printf("\n%s (%d failure%s)\n", failures ? "FAILED" : "ALL PASSED", failures,
         failures == 1 ? "" : "s");
  return failures ? 1 : 0;
}
