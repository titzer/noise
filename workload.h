#ifndef _WORKLOAD_H_
#define _WORKLOAD_H_

typedef struct {
  const char* name;
  void (*func)(void* arg);
  void* arg;
} Workload;

int spin_workload(void* count);
int empty_workload(void* nonce);

#endif
