#include "profile.h"
#include <math.h>
#include <stdio.h>

static int failures = 0;

static void check_close(const char *name, double got, double want, double tol) {
  if (fabs(got - want) <= tol) {
    printf("  ok    %-34s %.6f\n", name, got);
  } else {
    printf("  FAIL  %-34s got %.6f, want %.6f (tol %g)\n", name, got, want,
           tol);
    failures++;
  }
}

static void check_int(const char *name, long long got, long long want) {
  if (got == want) {
    printf("  ok    %-34s %lld\n", name, got);
  } else {
    printf("  FAIL  %-34s got %lld, want %lld\n", name, got, want);
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

/* images/s when every batch has size r; uses your own functions end to end */
static double rate_at(const GPUProfile *p, size_t r) {
  return throughput(service_rate(service_time(p, r)), r);
}

int main(void) {
  GPUProfile gp = {.v = 0.000679, .t0 = 0.005, .k = 15.0, .m0 = 500.0, .B = 64};

  printf("formulas\n");
  check_close("service_time(r=1)", service_time(&gp, 1), 0.005679, 1e-9);
  check_close("service_time(r=16)", service_time(&gp, 16), 0.015864, 1e-9);
  check_close("service_rate(t_r at r=16)", service_rate(service_time(&gp, 16)),
              63.035, 0.01);
  check_close("throughput(r=1) img/s", rate_at(&gp, 1), 176.09, 0.05);
  check_close("throughput(r=16) img/s", rate_at(&gp, 16), 1008.6, 0.1);
  check_close("memory_usage(r=1) MB", memory_usage(&gp, 1), 515.0, 1e-9);
  check_close("memory_usage(r=64) MB", memory_usage(&gp, 64), 1460.0, 1e-9);

  printf("capacity and load\n");
  check_close("max_arrival_rate", max_arrival_rate(&gp), 1320.8, 0.5);
  check_close("traffic_intensity(990)", traffic_intensity(&gp, 990.0), 0.75,
              0.005);

  printf("knee_batch\n");
  check_int("knee_batch(990)", knee_batch(&gp, 990.0), 16);
  check_true("b=15 cannot keep up with 990", rate_at(&gp, 15) < 990.0);
  check_true("b=16 keeps up with 990", rate_at(&gp, 16) >= 990.0);
  check_int("knee_batch(1.0) clamps up to 1", knee_batch(&gp, 1.0), 1);
  check_int("knee_batch(0.0) is 1", knee_batch(&gp, 0.0), 1);
  check_int("knee_batch(1500) > 1/v returns B", knee_batch(&gp, 1500.0), 64);

  printf("table\n");
  Profiles *t = build(&gp);
  check_true("build returned non-NULL", t != NULL);
  if (t) {
    check_int("table B", (long long)t->B, 64);
    check_int("rows[1].r", (long long)t->rows[1].r, 1);
    check_int("rows[16].r", (long long)t->rows[16].r, 16);
    check_int("rows[64].r (last valid index)", (long long)t->rows[64].r, 64);
    check_close("rows[16].t_r", t->rows[16].t_r, 0.015864, 1e-9);
    check_close("rows[16].throughput_r", t->rows[16].throughput_r, 1008.6, 0.1);
    check_close("rows[64].m_r", t->rows[64].m_r, 1460.0, 1e-9);
    free_profiles(t);
  }
  check_true("build(NULL) returns NULL", build(NULL) == NULL);
  GPUProfile bad = gp;
  bad.B = 0;
  check_true("build with B=0 returns NULL", build(&bad) == NULL);

  printf("\n%s (%d failure%s)\n", failures ? "FAILED" : "ALL PASSED", failures,
         failures == 1 ? "" : "s");
  return failures ? 1 : 0;
}
