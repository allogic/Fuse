#ifndef CO_RANDOM_H
#define CO_RANDOM_H

#include <stdint.h>

#include <library/core/co_forward.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void random_seed(char const *seed);

int8_t random_int8(void);
int16_t random_int16(void);
int32_t random_int32(void);
int64_t random_int64(void);

int8_t random_int8_between(int8_t min, int8_t max);
int16_t random_int16_between(int16_t min, int16_t max);
int32_t random_int32_between(int32_t min, int32_t max);
int64_t random_int64_between(int64_t min, int64_t max);

uint8_t random_uint8(void);
uint16_t random_uint16(void);
uint32_t random_uint32(void);
uint64_t random_uint64(void);

uint8_t random_uint8_between(uint8_t min, uint8_t max);
uint16_t random_uint16_between(uint16_t min, uint16_t max);
uint32_t random_uint32_between(uint32_t min, uint32_t max);
uint64_t random_uint64_between(uint64_t min, uint64_t max);

float random_real32(void);
double random_real64(void);

float random_real32_between(float min, float max);
double random_real64_between(double min, double max);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CO_RANDOM_H
