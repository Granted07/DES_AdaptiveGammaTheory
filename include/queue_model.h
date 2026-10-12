#ifndef QUEUE_MODEL_H
#define QUEUE_MODEL_H

#include <stddef.h>

typedef struct {
  size_t n;   // number of tasks waiting in the queue
              // when the gpu finishes processing a batch
  size_t a;   // lower bound
  size_t b;   // upper bound
  size_t N;   // queue capacity
  size_t r;   // current batch size taken, range = [a,b]
  int q0;     // n - r; is how many tasks are left
              // after taking r tasks
  size_t n_q; // predicted number of tasks waiting
              //  therefore q0 + how many tasks arrived
              //  following poisson's process
} Queue;

Queue *queue_batch_for_state(Queue *);

#endif // !QUEUE_MODEL_H
