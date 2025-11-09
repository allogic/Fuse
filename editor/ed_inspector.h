#ifndef ED_INSPECTOR_H
#define ED_INSPECTOR_H

extern uint8_t g_inspector_is_open;
extern uint8_t g_inspector_is_docked;

void inspector_create(context_t *context);
void inspector_refresh(context_t *context);
void inspector_draw(context_t *context);
void inspector_destroy(context_t *context);

#endif // ED_INSPECTOR_H
