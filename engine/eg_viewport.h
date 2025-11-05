#ifndef EG_VIEWPORT_H
#define EG_VIEWPORT_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

viewport_t *viewport_create(context_t *context, uint32_t width, uint32_t height);
void viewport_destroy(viewport_t *viewport);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_VIEWPORT_H
