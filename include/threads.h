#ifndef THREADS_H_
#define THREADS_H_

#include <pthread.h>
#include "queue.h"

/* Fila das threads que foram finalizadas e aguardam um join */
typedef struct cleanup_queue {
  pthread_mutex_t mutex;
  pthread_cond_t cond;
  queue cleanup;
} cleanup_queue;

#endif
