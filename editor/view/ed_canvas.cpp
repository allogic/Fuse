#include <editor/ed_pch.h>
#include <editor/ed_main.h>

#include <editor/view/ed_canvas.h>

#include <editor/node/ed_noise.h>

static void ed_canvas_view_handle_pending_node_transactions(ed_canvas_view_t *canvas);
static void ed_canvas_view_handle_pending_view_resets(ed_canvas_view_t *canvas);

static void ed_canvas_view_draw_context_menu(ed_canvas_view_t *canvas);

ed_canvas_view_t *ed_canvas_view_create(eg_context_t *context) {
  ed_canvas_view_t *canvas = (ed_canvas_view_t *)heap_alloc(sizeof(ed_canvas_view_t), 1, 0);

  canvas->base.context = context;
  canvas->base.is_dirty = 0;
  canvas->base.is_open = 1;
  canvas->base.is_docked = 0;

  canvas->unique_node_id = 0;
  canvas->unique_pin_id = 0;
  canvas->unique_link_id = 0;
  canvas->nodes = vector_create(sizeof(void *));
  canvas->reset_view_is_prending = 0;
  canvas->create_node_is_pending = 0;
  canvas->next_node_type = ED_NODE_TYPE_NONE;
  canvas->next_node_screen_position_x = 0.0F;
  canvas->next_node_screen_position_y = 0.0F;

  return canvas;
}
void ed_canvas_view_draw(ed_canvas_view_t *canvas) {
  ed_canvas_view_handle_pending_node_transactions(canvas);
  ed_canvas_view_handle_pending_view_resets(canvas);

  ImNodes::BeginNodeEditor();

  uint64_t node_index = 0;
  uint64_t node_count = vector_count(&canvas->nodes);

  while (node_index < node_count) {

    ed_node_t *node = *(ed_node_t **)vector_at(&canvas->nodes, node_index);

    switch (node->type) {
      case ED_NODE_TYPE_NOISE: {

        ed_noise_node_draw((ed_noise_node_t *)node);

        break;
      }
    }

    node_index++;
  }

  ed_canvas_view_draw_context_menu(canvas);

  ImNodes::EndNodeEditor();
}
void ed_canvas_view_destroy(ed_canvas_view_t *canvas) {
  uint64_t node_index = 0;
  uint64_t node_count = vector_count(&canvas->nodes);

  while (node_index < node_count) {

    ed_node_t *node = *(ed_node_t **)vector_at(&canvas->nodes, node_index);

    switch (node->type) {
      case ED_NODE_TYPE_NOISE: {

        ed_noise_node_destroy((ed_noise_node_t *)node);

        break;
      }
    }

    node_index++;
  }

  vector_destroy(&canvas->nodes);

  heap_free(canvas);
}

static void ed_canvas_view_draw_context_menu(ed_canvas_view_t *canvas) {
  if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) &&
      ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {

    ImGui::OpenPopup("canvas_context_menu");
  }

  if (ImGui::BeginPopup("canvas_context_menu")) {

    if (ImGui::MenuItem("Noise")) {
      canvas->create_node_is_pending = 1;
      canvas->next_node_type = ED_NODE_TYPE_NOISE;
      canvas->next_node_screen_position_x = ImGui::GetMousePos().x;
      canvas->next_node_screen_position_y = ImGui::GetMousePos().y;
    }

    if (ImGui::MenuItem("Reset View")) {
      canvas->reset_view_is_prending = 1;
    }

    ImGui::EndPopup();
  }
}

static void ed_canvas_view_handle_pending_node_transactions(ed_canvas_view_t *canvas) {
  if (canvas->create_node_is_pending) {

    uint32_t node_id = canvas->unique_node_id++;

    ImNodes::SetNodeScreenSpacePos(node_id, ImVec2(canvas->next_node_screen_position_x, canvas->next_node_screen_position_y));

    ed_node_t *node = 0;

    switch (canvas->next_node_type) {
      case ED_NODE_TYPE_NOISE: {

        node = (ed_node_t *)ed_noise_node_create(canvas->base.context, node_id);

        break;
      }
    }

    if (node) {
      vector_push(&canvas->nodes, &node);
    }

    canvas->create_node_is_pending = 0;
  }
}
static void ed_canvas_view_handle_pending_view_resets(ed_canvas_view_t *canvas) {
  if (canvas->reset_view_is_prending) {

    ImNodes::EditorContextResetPanning(ImVec2(0.0F, 0.0F));

    canvas->reset_view_is_prending = 0;
  }
}
