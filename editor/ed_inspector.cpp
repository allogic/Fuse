#include <editor/ed_pch.h>
#include <editor/ed_inspector.h>
#include <editor/ed_dockspace.h>
#include <editor/ed_titlebar.h>

void inspector_create(context_t *context) {
}
void inspector_refresh(context_t *context) {
}
void inspector_draw(context_t *context) {
  bool is_docked = false;

  if (dockspace_begin_child("Inspector", &g_titlebar_inspector_open, &is_docked)) {

    // TODO: Draw content..

    dockspace_end_child(is_docked);
  }
}
void inspector_destroy(context_t *context) {
}
