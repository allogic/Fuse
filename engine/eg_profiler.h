#ifndef EG_PROFILER_H
#define EG_PROFILER_H

#define EG_PROFILER_SAMPLE_COUNT 2000

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern eg_context_t *g_profiler_context;

extern uint64_t g_profiler_frame_index;

extern char const *g_profiler_cpu_sample_names[EG_PROFILER_SAMPLE_LANE_COUNT];

extern eg_cpu_frame_sample_t g_profiler_cpu_samples[EG_PROFILER_SAMPLE_LANE_COUNT][EG_PROFILER_SAMPLE_COUNT];

#ifdef __cplusplus
}
#endif // __cplusplus

#define EG_PROFILER_INIT(CTX) \
  g_profiler_context = CTX;

#define EG_PROFILER_INC_FRAME() \
  g_profiler_frame_index = (g_profiler_frame_index + 1) % EG_PROFILER_SAMPLE_COUNT;

#define EG_PROFILER_SCOPE_BEGIN(LANE)                   \
  static LARGE_INTEGER profiler_scope_time_start = {0}; \
  QueryPerformanceCounter(&profiler_scope_time_start);

#define EG_PROFILER_SCOPE_END(LANE)                                                            \
  static LARGE_INTEGER profiler_scope_time_end = {0};                                          \
  QueryPerformanceCounter(&profiler_scope_time_end);                                           \
  g_profiler_cpu_sample_names[LANE] = __func__;                                                \
  g_profiler_cpu_samples[LANE][g_profiler_frame_index].time = (float)g_profiler_context->time; \
  g_profiler_cpu_samples[LANE][g_profiler_frame_index].delta = (float)(((double)(profiler_scope_time_end.QuadPart - profiler_scope_time_start.QuadPart)) / ((double)g_profiler_context->time_freq.QuadPart));

#endif // EG_PROFILER_H
