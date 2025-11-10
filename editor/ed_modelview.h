#ifndef ED_MODELVIEW_H
#define ED_MODELVIEW_H

modelview_t *modelview_create(context_t *context, char const *name);
void modelview_refresh(modelview_t *modelview);
void modelview_draw(modelview_t *modelview);
void modelview_destroy(modelview_t *modelview);

#endif // ED_MODELVIEW_H
