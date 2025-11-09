#include <engine/eg_pch.h>
#include <engine/eg_profiler.h>

context_t *g_profiler_context = 0;

uint64_t g_profiler_frame_index = 0;

char const *g_profiler_cpu_sample_names[PROFILER_SAMPLE_LANE_COUNT] = {0};

cpu_frame_sample_t g_profiler_cpu_samples[PROFILER_SAMPLE_LANE_COUNT][PROFILER_SAMPLE_COUNT] = {0};
