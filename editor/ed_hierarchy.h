#ifndef ED_HIERARCHY_H
#define ED_HIERARCHY_H

extern uint8_t g_hierarchy_is_open;
extern uint8_t g_hierarchy_is_docked;

extern ecs_entity_t g_hierarchy_selected_entity;

void hierarchy_create(context_t *context);
void hierarchy_refresh(context_t *context);
void hierarchy_draw(context_t *context);
void hierarchy_destroy(context_t *context);

#endif // ED_HIERARCHY_H
