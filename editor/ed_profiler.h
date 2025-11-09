#ifndef ED_PROFILER_H
#define ED_PROFILER_H

extern uint8_t g_profiler_is_open;
extern uint8_t g_profiler_is_docked;

void profiler_create(context_t *context);
void profiler_refresh(context_t *context);
void profiler_draw(context_t *context);
void profiler_destroy(context_t *context);

#endif // ED_PROFILER_H
