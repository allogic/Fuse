#include <editor/ed_pch.h>
#include <editor/ed_main.h>

#include <editor/dockable/ed_canvas.h>

ed_canvas_t ed_canvas_create(eg_context_t *context) {
  ed_canvas_t canvas = {0};

  canvas.context = context;
  canvas.is_dirty = 0;
  canvas.is_open = 1;
  canvas.is_docked = 0;

  return canvas;
}
void ed_canvas_refresh(ed_canvas_t *canvas) {
}
void ed_canvas_draw(ed_canvas_t *canvas) {
  ImNodes::BeginNodeEditor();

  const int node_id = 1;

  ImNodes::BeginNode(node_id);

  ImNodes::BeginNodeTitleBar();
  ImGui::TextUnformatted("output node");
  ImNodes::EndNodeTitleBar();

  const int output_attr_id = 2;

  ImNodes::BeginOutputAttribute(output_attr_id);
  ImGui::Text("output pin");
  ImNodes::EndOutputAttribute();

  ImGui::Text("node content");

  ImNodes::EndNode();

  ImNodes::EndNodeEditor();
}
void ed_canvas_destroy(ed_canvas_t *canvas) {
}
