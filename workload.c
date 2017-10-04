int spin_workload(void* input) {
  unsigned count = (unsigned)input, i = 0;
  for (i = 0; i < count; i++) {
    __asm__ __volatile__ ("");
  }
  return (int)i;
}

int empty_workload(void* nonce) {
  return 0;
}

int add_workload(void* input) {
  unsigned i = 0, sum = 0, max = (unsigned)input;
  for (i = 0; i < max; i++) {
    sum += i;
  }
  return (int)sum;
}
