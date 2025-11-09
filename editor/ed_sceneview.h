#ifndef ED_SCENEVIEW_H
#define ED_SCENEVIEW_H

void sceneview_create(context_t *context, uint64_t link_index, uint32_t width, uint32_t height, char const *name);
void sceneview_refresh(sceneview_t *sceneview);
void sceneview_draw(sceneview_t *sceneview);
void sceneview_destroy(sceneview_t *sceneview);

#endif // ED_SCENEVIEW_H
