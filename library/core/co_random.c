#include <stdlib.h>
#include <time.h>

#include <library/core/co_random.h>

void random_seed(char const *seed) {
  if (seed) {
    uint32_t hash = 0;

    while (*seed++) {
      hash = hash * 31 + (uint32_t)*seed;
    }

    srand(hash);
  } else {
    srand((uint32_t)time(0));
  }
}

int8_t random_int8(void) {
  return (int8_t)((rand() % 0x100U) - 0x7F);
}
int16_t random_int16(void) {
  return (int16_t)((rand() % 0x10000U) - 0x7FFF);
}
int32_t random_int32(void) {
  return (int32_t)(((rand() << 16) | rand()) - 0x7FFFFFFF);
}
int64_t random_int64(void) {
  return (int64_t)((((int64_t)((rand() << 16) | rand()) << 32) | (int64_t)((rand() << 16) | rand())) - 0x7FFFFFFFFFFFFFFF);
}

int8_t random_int8_between(int8_t min, int8_t max) {
  return (int8_t)(min + rand() % (max - min + 1));
}
int16_t random_int16_between(int16_t min, int16_t max) {
  return (int16_t)(min + rand() % (max - min + 1));
}
int32_t random_int32_between(int32_t min, int32_t max) {
  return (int32_t)(min + (int32_t)rand() * ((int64_t)(max - min + 1)) / (RAND_MAX + 1));
}
int64_t random_int64_between(int64_t min, int64_t max) {
  return (int64_t)(min + (int64_t)rand() * ((int64_t)(max - min + 1)) / (RAND_MAX + 1));
}

uint8_t random_uint8(void) {
  return (uint8_t)(rand() % 0x100ULL);
}
uint16_t random_uint16(void) {
  return (uint16_t)((rand() % 0x10000ULL));
}
uint32_t random_uint32(void) {
  return ((uint32_t)rand() << 16) | (uint32_t)rand();
}
uint64_t random_uint64(void) {
  return (uint64_t)(((uint64_t)(((uint32_t)rand() << 16) | (uint32_t)rand()) << 32) | (uint64_t)(((uint32_t)rand() << 16) | (uint32_t)rand()));
}

uint8_t random_uint8_between(uint8_t min, uint8_t max) {
  return (uint8_t)(min + rand() % (max - min + 1));
}
uint16_t random_uint16_between(uint16_t min, uint16_t max) {
  return (uint16_t)(min + rand() % (max - min + 1));
}
uint32_t random_uint32_between(uint32_t min, uint32_t max) {
  return (uint32_t)(min + (uint32_t)rand() * ((uint64_t)(max - min + 1)) / (RAND_MAX + 1));
}
uint64_t random_uint64_between(uint64_t min, uint64_t max) {
  return (uint64_t)(min + (uint64_t)rand() * ((uint64_t)(max - min + 1)) / (RAND_MAX + 1));
}

float random_real32(void) {
  return (float)rand() / (float)RAND_MAX;
}
double random_real64(void) {
  return (double)rand() / (double)RAND_MAX;
}

float random_real32_between(float min, float max) {
  return min + (max - min) * ((float)rand() / (float)RAND_MAX);
}
double random_real64_between(double min, double max) {
  return min + (max - min) * ((double)rand() / (double)RAND_MAX);
}
