#ifndef ED_SCENEVIEW_H
#define ED_SCENEVIEW_H

sceneview_t *sceneview_create(context_t *context, char const *name);
void sceneview_refresh(sceneview_t *sceneview);
void sceneview_draw(sceneview_t *sceneview);
void sceneview_destroy(sceneview_t *sceneview);

#endif // ED_SCENEVIEW_H
