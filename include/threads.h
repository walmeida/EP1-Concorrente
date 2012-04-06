#ifndef THREADS_H_
#define THREADS_H_

typedef struct cleanup_queue {
  pthread_mutex_t mutex;
  pthread_cond_t cond;
  queue cleanup;
} cleanup_queue;

#endif
