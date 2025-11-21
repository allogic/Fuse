#include <editor/ed_pch.h>
#include <editor/ed_graph.h>

#include <editor/node/ed_mesh.h>
#include <editor/node/ed_noise.h>
#include <editor/node/ed_output.h>

ed_graph_t ed_graph_create(eg_context_t *context, char const *name) {
  ed_graph_t graph = {0};

  graph.context = context;
  graph.unique_node_id = 0;
  graph.unique_pin_id = 0;
  graph.unique_link_id = 0;
  graph.nodes = vector_create(sizeof(void *));

  ed_node_t *output_node = (ed_node_t *)ed_output_node_create(&graph);

  vector_push(&graph.nodes, &output_node);

  return graph;
}
void ed_graph_destroy(ed_graph_t *graph) {
  uint64_t node_index = 0;
  uint64_t node_count = vector_count(&graph->nodes);

  while (node_index < node_count) {

    ed_node_t *node = *(ed_node_t **)vector_at(&graph->nodes, node_index);

    switch (node->type) {
      case ED_NODE_TYPE_MESH: {

        ed_mesh_node_destroy((ed_mesh_node_t *)node);

        break;
      }
      case ED_NODE_TYPE_NOISE: {

        ed_noise_node_destroy((ed_noise_node_t *)node);

        break;
      }
      case ED_NODE_TYPE_OUTPUT: {

        ed_output_node_destroy((ed_output_node_t *)node);

        break;
      }
    }

    node_index++;
  }

  vector_destroy(&graph->nodes);
}
