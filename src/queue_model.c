

#include "queue_model.h"

Queue *queue_batch_for_state(Queue *queue) {
  if (queue->n < queue->a) {
    queue->q0 = 0;
    queue->r = queue->a;
  } else if (queue->a <= queue->n || queue->n <= queue->b) {
    queue->q0 = 0;
    queue->r = queue->n;
  } else {
    queue->r = queue->b;
    queue->q0 = (int)(queue->n - queue->b);
  }
  return queue;
}
