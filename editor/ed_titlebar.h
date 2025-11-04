#ifndef ED_TITLEBAR_H
#define ED_TITLEBAR_H

extern bool g_titlebar_catalog_open;
extern bool g_titlebar_hierarchy_open;
extern bool g_titlebar_inspector_open;
extern bool g_titlebar_detail_open;

void titlebar_create(context_t *context);
void titlebar_refresh(context_t *context);
void titlebar_draw(context_t *context);
void titlebar_destroy(context_t *context);

#endif // ED_TITLEBAR_H
