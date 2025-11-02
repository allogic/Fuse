#include <editor/ed_pch.h>
#include <editor/ed_hierarchy.h>
#include <editor/ed_titlebar.h>

static void hierarchy_draw_tree(ecs_world_t *world, ecs_entity_t e);

void hierarchy_create() {
}
void hierarchy_refresh() {
}
void hierarchy_draw(context_t *context) {
  ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar;

  ImGui::Begin("Hierarchy", &g_titlebar_hierarchy_open, window_flags);

  ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(50, 50, 50, 255));
  ImGui::BeginChild("HierarchyFirstChild");

  ImVec2 second_window_size = ImGui::GetWindowSize();
  ImVec2 second_cursor_position = ImVec2(5.0F, 5.0F);

  second_window_size.x -= 10.0F;
  second_window_size.y -= 30.0F;

  ImGui::SetCursorPos(second_cursor_position);
  ImGui::BeginChild("HierarchySecondChild", second_window_size);

  hierarchy_draw_tree(context->scene->world, context->scene->root);

  ImGui::EndChild();

  ImGui::EndChild();
  ImGui::PopStyleColor();

  ImGui::End();
}
void hierarchy_destroy() {
}

static void hierarchy_draw_tree(ecs_world_t *world, ecs_entity_t entity) {
  const char *name = ecs_get_name(world, entity);

  ecs_iter_t child_iter = ecs_children(world, entity);

  ecs_children_next(&child_iter);

  // camera_t const *camera = ecs_get(world, entity, camera_t);
  // editor_controller_t const *editor_controller = ecs_get(world, entity, editor_controller_t);
  // rigidbody_t const *controller = ecs_get(world, entity, rigidbody_t);
  // transform_t const *transform = ecs_get(world, entity, transform_t);

  ImGuiTreeNodeFlags tree_node_flags =
    ImGuiTreeNodeFlags_None |
    ImGuiTreeNodeFlags_SpanFullWidth |
    ImGuiTreeNodeFlags_OpenOnArrow |
    ImGuiTreeNodeFlags_FramePadding;

  if (child_iter.count == 0) {
    tree_node_flags |= ImGuiTreeNodeFlags_Leaf;
  }

  if (ImGui::TreeNodeEx(name, tree_node_flags)) {

    ImGui::Dummy(ImVec2(0.0f, 5.0F));

    do {

      uint64_t child_index = 0;
      uint64_t child_count = child_iter.count;

      while (child_index < child_count) {

        hierarchy_draw_tree(world, child_iter.entities[child_index]);

        child_index++;
      }

    } while (ecs_children_next(&child_iter));

    ImGui::Dummy(ImVec2(0.0f, 5.0F));

    ImGui::TreePop();
  }
}
