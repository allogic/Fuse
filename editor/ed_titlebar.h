#ifndef ED_TITLEBAR_H
#define ED_TITLEBAR_H

extern uint8_t g_titlebar_catalog_is_open;
extern uint8_t g_titlebar_hierarchy_is_open;
extern uint8_t g_titlebar_inspector_is_open;
extern uint8_t g_titlebar_detail_is_open;

extern uint8_t g_titlebar_catalog_is_docked;
extern uint8_t g_titlebar_hierarchy_is_docked;
extern uint8_t g_titlebar_inspector_is_docked;
extern uint8_t g_titlebar_detail_is_docked;

void titlebar_create(context_t *context);
void titlebar_refresh(context_t *context);
void titlebar_draw(context_t *context);
void titlebar_destroy(context_t *context);

#endif // ED_TITLEBAR_H
