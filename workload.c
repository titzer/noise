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
