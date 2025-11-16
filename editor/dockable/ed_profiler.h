#ifndef ED_PROFILER_H
#define ED_PROFILER_H

ed_profiler_t ed_profiler_create(context_t *context);
void ed_profiler_refresh(ed_profiler_t *profiler);
void ed_profiler_draw(ed_profiler_t *profiler);
void ed_profiler_destroy(ed_profiler_t *profiler);

#endif // ED_PROFILER_H
