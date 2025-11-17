#include <editor/ed_pch.h>
#include <editor/ed_node.h>

#include <editor/node/ed_noise.h>

ed_noise_node_t *ed_noise_node_create(eg_context_t *context, uint32_t node_id) {
  ed_noise_node_t *node = (ed_noise_node_t *)heap_alloc(sizeof(ed_noise_node_t), 1, 0);

  node->base.context = context;
  node->base.id = node_id;
  node->base.type = ED_NODE_TYPE_NOISE;

  return node;
}
void ed_noise_node_refresh(ed_noise_node_t *node) {
}
void ed_noise_node_draw(ed_noise_node_t *node) {
  ImNodes::BeginNode(node->base.id);

  ImNodes::BeginNodeTitleBar();
  ImGui::Text("Noise");
  ImNodes::EndNodeTitleBar();

  ed_node_draw_input(1, "Input 1");
  ImGui::SameLine();
  ed_node_draw_output(2, "Output 1");

  ed_node_draw_input(3, "Input 2");
  ImGui::SameLine();
  ed_node_draw_output(4, "Output 2");

  ImNodes::EndNode();
}
void ed_noise_node_destroy(ed_noise_node_t *node) {
  heap_free(node);
}
