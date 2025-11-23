#include <engine/eg_pch.h>
#include <engine/eg_profiler.h>

eg_context_t *g_profiler_context = 0;

uint64_t g_profiler_frame_index = 0;

char const *g_profiler_cpu_sample_names[EG_PROFILER_SAMPLE_LANE_COUNT] = {0};

eg_cpu_frame_sample_t g_profiler_cpu_samples[EG_PROFILER_SAMPLE_LANE_COUNT][EG_PROFILER_SAMPLE_COUNT] = {0};
