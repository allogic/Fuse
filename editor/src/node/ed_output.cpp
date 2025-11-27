#include <editor/ed_pch.h>
#include <editor/ed_node.h>

#include <editor/node/ed_output.h>

/*
ed_output_node_t *ed_output_node_create(ed_graph_t *graph) {
  ed_output_node_t *node = (ed_output_node_t *)heap_alloc(sizeof(ed_output_node_t), 1, 0);

  node->base.context = canvas->context;
  node->base.id = canvas->unique_node_id++;
  node->base.type = ED_NODE_TYPE_OUTPUT;

  node->input_mesh = canvas->unique_pin_id++;

  return node;
}
void ed_output_node_refresh(ed_output_node_t *node) {
  // TODO
}
void ed_output_node_draw(ed_output_node_t *node) {
  ImNodes::BeginNode(node->base.id);

  ImNodes::BeginNodeTitleBar();
  ImGui::Text("Output");
  ImNodes::EndNodeTitleBar();

  ed_node_draw_input(node->input_mesh, "Mesh");

  ImNodes::EndNode();
}
void ed_output_node_destroy(ed_output_node_t *node) {
  heap_free(node);
}
*/
