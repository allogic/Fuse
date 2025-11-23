#include <editor/ed_pch.h>
#include <editor/ed_node.h>

#include <editor/node/ed_noise.h>

ed_noise_node_t *ed_noise_node_create(ed_canvas_t *canvas) {
  ed_noise_node_t *node = (ed_noise_node_t *)heap_alloc(sizeof(ed_noise_node_t), 1, 0);

  node->base.context = canvas->context;
  node->base.id = canvas->unique_node_id++;
  node->base.type = ED_NODE_TYPE_NOISE;

  node->output_factor = canvas->unique_pin_id++;

  return node;
}
void ed_noise_node_refresh(ed_noise_node_t *node) {
  // TODO
}
void ed_noise_node_draw(ed_noise_node_t *node) {
  ImNodes::BeginNode(node->base.id);

  ImNodes::BeginNodeTitleBar();
  ImGui::Text("Noise");
  ImNodes::EndNodeTitleBar();

  ed_node_draw_output(node->output_factor, "Factor");

  ImNodes::EndNode();
}
void ed_noise_node_destroy(ed_noise_node_t *node) {
  heap_free(node);
}
