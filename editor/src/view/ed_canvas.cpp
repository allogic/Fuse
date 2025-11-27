#include <editor/ed_pch.h>
#include <editor/ed_main.h>
#include <editor/ed_graph.h>

#include <editor/view/ed_canvas.h>

#include <editor/node/ed_mesh.h>
#include <editor/node/ed_noise.h>
#include <editor/node/ed_output.h>

static void ed_canvas_view_handle_pending_node_transactions(ed_canvas_view_t *canvas);
static void ed_canvas_view_handle_pending_view_resets(ed_canvas_view_t *canvas);
static void ed_canvas_view_handle_node_selection(ed_canvas_view_t *canvas);

static void ed_canvas_view_draw_context_menu(ed_canvas_view_t *canvas);

ed_canvas_view_t *ed_canvas_view_create(eg_context_t *context) {
  ed_canvas_view_t *canvas = (ed_canvas_view_t *)lb_heap_alloc(sizeof(ed_canvas_view_t), 1, 0);

  canvas->base.context = context;
  canvas->base.is_dirty = 0;
  canvas->base.is_open = 1;
  canvas->base.is_docked = 0;

  canvas->reset_view_is_pending = 0;
  canvas->create_node_is_pending = 0;
  canvas->next_node_screen_position_x = 0.0F;
  canvas->next_node_screen_position_y = 0.0F;
  canvas->graph = ed_graph_create(context, "kek W");
  canvas->next_node_type = LB_NODE_TYPE_COUNT;
  canvas->selected_nodes = lb_vector_create(sizeof(uint32_t));

  return canvas;
}
void ed_canvas_view_draw(ed_canvas_view_t *canvas) {
  ImNodes::BeginNodeEditor();

  /*
  uint64_t node_index = 0;
  uint64_t node_count = lb_vector_count(&canvas->graph);

  while (node_index < node_count) {

    ed_node_t *node = *(ed_node_t **)vector_at(&canvas->handle.nodes, node_index);

    switch (node->type) {
      case ED_NODE_TYPE_MESH: {

        ed_mesh_node_draw((ed_mesh_node_t *)node);

        break;
      }
      case ED_NODE_TYPE_NOISE: {

        ed_noise_node_draw((ed_noise_node_t *)node);

        break;
      }
      case ED_NODE_TYPE_OUTPUT: {

        ed_output_node_draw((ed_output_node_t *)node);

        break;
      }
    }

    node_index++;
  }

  ed_canvas_view_draw_context_menu(canvas);
  */

  ImNodes::EndNodeEditor();

  ed_canvas_view_handle_pending_node_transactions(canvas);
  ed_canvas_view_handle_pending_view_resets(canvas);
  ed_canvas_view_handle_node_selection(canvas);
}
void ed_canvas_view_destroy(ed_canvas_view_t *canvas) {
  ed_graph_destroy(&canvas->graph);

  lb_vector_destroy(canvas->selected_nodes);

  lb_heap_free(canvas);
}

static void ed_canvas_view_handle_pending_node_transactions(ed_canvas_view_t *canvas) {
  // if (canvas->create_node_is_pending) {
  //
  //   ed_node_t *node = 0;
  //
  //   switch (canvas->next_node_type) {
  //     case ED_NODE_TYPE_MESH: {
  //
  //       node = (ed_node_t *)ed_mesh_node_create(&canvas->handle);
  //
  //       break;
  //     }
  //     case ED_NODE_TYPE_NOISE: {
  //
  //       node = (ed_node_t *)ed_noise_node_create(&canvas->handle);
  //
  //       break;
  //     }
  //   }
  //
  //   if (node) {
  //     ImNodes::SetNodeScreenSpacePos(node->id, ImVec2(canvas->next_node_screen_position_x, canvas->next_node_screen_position_y));
  //
  //     vector_push(&canvas->handle.nodes, &node);
  //   }
  //
  //   canvas->create_node_is_pending = 0;
  // }
}
static void ed_canvas_view_handle_pending_view_resets(ed_canvas_view_t *canvas) {
  // if (canvas->reset_view_is_pending) {
  //
  //   ImNodes::EditorContextResetPanning(ImVec2(0.0F, 0.0F));
  //
  //   canvas->reset_view_is_pending = 0;
  // }
}
static void ed_canvas_view_handle_node_selection(ed_canvas_view_t *canvas) {
  // int32_t node_count = ImNodes::NumSelectedNodes();
  //
  // vector_resize(&canvas->selected_nodes, node_count);
  //
  // if (node_count > 0) {
  //   int32_t *selected_node_buffer = (int32_t *)vector_buffer(&canvas->selected_nodes);
  //
  //   ImNodes::GetSelectedNodes(selected_node_buffer);
  //
  //   canvas->selected_node = *selected_node_buffer;
  // } else {
  //   canvas->selected_node = -1;
  // }
}

static void ed_canvas_view_draw_context_menu(ed_canvas_view_t *canvas) {
  // if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) &&
  //     ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
  //
  //   ImGui::OpenPopup("canvas_context_menu");
  // }
  //
  // if (ImGui::BeginPopup("canvas_context_menu")) {
  //
  //   if (ImGui::MenuItem("Mesh")) {
  //     canvas->create_node_is_pending = 1;
  //     canvas->next_node_type = ED_NODE_TYPE_MESH;
  //     canvas->next_node_screen_position_x = ImGui::GetMousePos().x;
  //     canvas->next_node_screen_position_y = ImGui::GetMousePos().y;
  //   }
  //
  //   if (ImGui::MenuItem("Noise")) {
  //     canvas->create_node_is_pending = 1;
  //     canvas->next_node_type = ED_NODE_TYPE_NOISE;
  //     canvas->next_node_screen_position_x = ImGui::GetMousePos().x;
  //     canvas->next_node_screen_position_y = ImGui::GetMousePos().y;
  //   }
  //
  //   if (ImGui::MenuItem("Reset View")) {
  //     canvas->reset_view_is_pending = 1;
  //   }
  //
  //   ImGui::EndPopup();
  // }
}
