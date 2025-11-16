#ifndef EG_VIEWPORT_H
#define EG_VIEWPORT_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

eg_viewport_t *eg_viewport_create(eg_context_t *context, uint32_t width, uint32_t height);
void eg_viewport_resize(eg_viewport_t *viewport, uint32_t width, uint32_t height);
void eg_viewport_destroy(eg_viewport_t *viewport);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_VIEWPORT_H
