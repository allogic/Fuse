#ifndef ED_VIEWPORT_H
#define ED_VIEWPORT_H

viewport_t *viewport_create(context_t *context, char const *name);
void viewport_refresh(viewport_t *viewport);
void viewport_draw(viewport_t *viewport);
void viewport_destroy(viewport_t *viewport);

#endif // ED_VIEWPORT_H
