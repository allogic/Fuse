#include <editor/ed_pch.h>
#include <editor/ed_node.h>

#include <editor/node/ed_mesh.h>

ed_mesh_node_t *ed_mesh_node_create(ed_canvas_t *canvas) {
  ed_mesh_node_t *node = (ed_mesh_node_t *)heap_alloc(sizeof(ed_mesh_node_t), 1, 0);

  node->base.context = canvas->context;
  node->base.id = canvas->unique_node_id++;
  node->base.type = ED_NODE_TYPE_MESH;

  node->output_mesh = canvas->unique_pin_id++;
  node->vertex_buffer = eg_buffer_create_vertex_coherent(canvas->context, sizeof(eg_brdf_vertex_t) * 3);
  node->source_code = string_create();

  string_resize(&node->source_code, 1024);

  return node;
}
void ed_mesh_node_refresh(ed_mesh_node_t *node) {
  // TODO
}
void ed_mesh_node_draw(ed_mesh_node_t *node) {
  ImNodes::BeginNode(node->base.id);

  ImNodes::BeginNodeTitleBar();
  ImGui::Text("Mesh");
  ImNodes::EndNodeTitleBar();

  ed_node_draw_output(node->output_mesh, "");

  ImGui::InputTextMultiline("##SourceCode", string_buffer(&node->source_code), string_size(&node->source_code), ImVec2(350.0F, 200.0F));

  ImNodes::EndNode();
}
void ed_mesh_node_destroy(ed_mesh_node_t *node) {
  string_destroy(&node->source_code);

  eg_buffer_destroy(node->vertex_buffer);

  heap_free(node);
}
