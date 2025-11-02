#ifndef ED_TITLEBAR_H
#define ED_TITLEBAR_H

extern bool g_titlebar_catalog_open;
extern bool g_titlebar_hierarchy_open;
extern bool g_titlebar_inspector_open;
extern bool g_titlebar_viewport_open;

void titlebar_create();
void titlebar_refresh();
void titlebar_draw(context_t *context);
void titlebar_destroy();

#endif // ED_TITLEBAR_H
