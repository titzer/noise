#include <inttypes.h>
#include <stdio.h>
#include <unistd.h>
#include "workload.h"

#define NUM_SAMPLES 16
#define DEFAULT_RUNS 100
#define NUM_GROUPS 10
#define WORKLOAD add_workload((void*)1000000)
#define SLEEP_US 50000

uint64_t samples[NUM_SAMPLES];
uint64_t sum;
uint64_t common_vals[NUM_SAMPLES];
unsigned common_count[NUM_SAMPLES];

uint64_t __attribute__((always_inline)) rdtscp_inline() {
  uint64_t tsc;
  __asm__ __volatile__(";"
                       "lfence;"
                       "rdtsc;"            // serializing read of tsc
                       "shl $32, %%rdx;"   // shift higher 32 bits stored in rdx up
                       "or %%rdx, %%rax;"  // and or onto rax
                       : "=a"(tsc)         // output to tsc variable
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
  int c = 0, j = 0;
  double sample_sum = 0;
  for (i = 0; i < NUM_SAMPLES; i++) {
    uint64_t s = samples[i];
    if (s < min) min = s;
    if (s > max) max = s;
    printf("%3" PRId64 " ", s);
    sum += s;
    sample_sum += s;
    // find the count for this sample.
    for (j = 0; j < c; j++) {
      if (s == common_vals[j]) {
        common_count[j]++;
        break;
      }
    }
    if (j == c) {
      common_vals[c] = s;
      common_count[c] = 1;
      c++;
    }
  }
  // find most common value.
  for (j = 0; j < c; j++) {
    if (common_count[j] > common_count[0]) {
      common_vals[0] = common_vals[j];
      common_count[0] = common_count[j];
    }
  }
  printf("    min=%" PRId64 " max=%" PRId64 " uniq=%d common=%" PRId64 " avg=%0.2lf\n", min, max, c, common_vals[0], sample_sum / NUM_SAMPLES);
}

int main(int argc, char **argp) {
  int i = 0, g = 0, num_runs = DEFAULT_RUNS, num_groups = NUM_GROUPS;
  warm_samples();
  uint64_t before = rdtscp_inline();
  for (g = 0; g < num_groups; g++) {
    sum = 0;
    for (i = 0; i < num_runs; i++) {
      run_rdtscp_loop();
      output();
    }
    printf("sum=%" PRId64 "\n", sum);
    if (SLEEP_US > 0) usleep(SLEEP_US);
  }
  uint64_t after = rdtscp_inline();
  printf("elapsed=%" PRId64 "\n\n", after - before);
  return 0;
}
