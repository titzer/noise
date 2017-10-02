#include <inttypes.h>
#include <stdio.h>
#include "workload.h"

#define NUM_SAMPLES 28
#define DEFAULT_RUNS 16
#define WORKLOAD

uint64_t samples[NUM_SAMPLES];
uint64_t sum;

uint64_t __attribute__((always_inline)) rdtscp_inline() {
  uint64_t tsc;
  __asm__ __volatile__("lfence;"
                       "rdtscp;"           // serializing read of tsc
                       "shl $32, %%rdx;"   // shift higher 32 bits stored in rdx up
                       "or %%rdx, %%rax;"  // and or onto rax
                       : "=a"(tsc)        // output to tsc variable
                       :
                       : "%rcx", "%rdx"); // rcx and rdx are clobbered 
  return tsc;
}

// Warm up the data cache by reading and writing the samples array.
void warm_samples() {
  int i = 0;
  for (i = 0; i < NUM_SAMPLES; i++) {
    samples[i] ^= 0xFFFFFFFFFFFFFFFull;
  }
}

void run_rdtscp_loop() {
  int i = 0;
  for (i = 0; i < NUM_SAMPLES; i++) {
    uint64_t before = rdtscp_inline();
    WORKLOAD;
    uint64_t after = rdtscp_inline();
    samples[i] = after - before;
  }
}

void output() {
  int i = 0;
  uint64_t min = 1000000000;
  uint64_t max = 0;
  for (i = 0; i < NUM_SAMPLES; i++) {
    uint64_t s = samples[i];
    if (s < min) min = s;
    if (s > max) max = s;
    printf("%-3" PRId64 " ", s);
    sum += s;
  }
  printf("    min=%" PRId64 " max=%" PRId64 "\n", min, max);
}

int main(int argc, char **argp) {
  int i = 0, num_runs = DEFAULT_RUNS;  // TODO: parse argp
  warm_samples();
  uint64_t before = rdtscp_inline();
  for (i = 0; i < num_runs; i++) {
    run_rdtscp_loop();
    output();
  }
  uint64_t after = rdtscp_inline();
  printf("sum=%" PRId64 "  elapsed=%" PRId64 "\n\n", sum, after - before);
  return 0;
}
