#include <editor/ed_pch.h>
#include <editor/ed_hierarchy.h>
#include <editor/ed_dockspace.h>
#include <editor/ed_titlebar.h>
#include <editor/ed_main.h>

static void hierarchy_draw_tree(ecs_world_t *world, ecs_entity_t entity);
static void hierarchy_draw_context_menu(ecs_world_t *world, ecs_entity_t entity);
static void hierarchy_draw_entity_buttons(ecs_world_t *world, ecs_entity_t entity);

uint8_t g_hierarchy_is_open = 1;
uint8_t g_hierarchy_is_docked = 0;

ecs_entity_t g_hierarchy_selected_entity = 0;

void hierarchy_create(context_t *context) {
}
void hierarchy_refresh(context_t *context) {
}
void hierarchy_draw(context_t *context) {
  if (dockspace_begin_child("Hierarchy", &g_hierarchy_is_open, &g_hierarchy_is_docked)) {

    hierarchy_draw_tree(context->scene->world, context->scene->root);

    dockspace_end_child(g_hierarchy_is_docked);
  }
}
void hierarchy_destroy(context_t *context) {
}

static void hierarchy_draw_tree(ecs_world_t *world, ecs_entity_t entity) {
  const char *name = ecs_get_name(world, entity);

  ecs_iter_t child_iter = ecs_children(world, entity);

  ecs_children_next(&child_iter);

  ImGuiTreeNodeFlags tree_node_flags =
    ImGuiTreeNodeFlags_None |
    ImGuiTreeNodeFlags_SpanFullWidth |
    ImGuiTreeNodeFlags_OpenOnArrow |
    ImGuiTreeNodeFlags_FramePadding;

  if (child_iter.count == 0) {
    tree_node_flags |= ImGuiTreeNodeFlags_Leaf;
  }

  if (entity == g_hierarchy_selected_entity) {
    tree_node_flags |= ImGuiTreeNodeFlags_Selected;
  }

  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, 0.0F));

  uint8_t is_open = ImGui::TreeNodeEx(name, tree_node_flags);

  if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
    g_hierarchy_selected_entity = entity;
  }

  hierarchy_draw_context_menu(world, entity);
  hierarchy_draw_entity_buttons(world, entity);

  ImGui::PopStyleVar(1);

  if (is_open) {

    do {

      uint64_t child_index = 0;
      uint64_t child_count = child_iter.count;

      while (child_index < child_count) {

        hierarchy_draw_tree(world, child_iter.entities[child_index]);

        child_index++;
      }

    } while (ecs_children_next(&child_iter));

    ImGui::TreePop();
  }
}
static void hierarchy_draw_context_menu(ecs_world_t *world, ecs_entity_t entity) {
  if (ImGui::BeginPopupContextItem("Hierarchy Context Menu")) {

    ImGui::PushFont(g_editor_material_symbols_h5);

    if (ImGui::MenuItem(ICON_MS_ADD)) {
      // TODO
    }
    if (ImGui::MenuItem(ICON_MS_REMOVE)) {
      // TODO
    }

    ImGui::Separator();

    if (ImGui::MenuItem(ICON_MS_EDIT)) {
      // TODO
    }

    ImGui::PopFont();
    ImGui::EndPopup();
  }
}
static void hierarchy_draw_entity_buttons(ecs_world_t *world, ecs_entity_t entity) {
  ImGui::SameLine(ImGui::GetWindowSize().x - 20.0F);

  ImGui::PushStyleColor(ImGuiCol_Button, EDITOR_DOCKING_BACKGROUND_COLOR);
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, EDITOR_HIGHLIGHT_COLOR);
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, EDITOR_ACTIVE_COLOR);

  ImGui::PushFont(g_editor_material_symbols);

  if (ImGui::Button(ICON_MS_DELETE, ImVec2(20.0F, 0.0F))) {
    // TODO
  }

  ImGui::SameLine(ImGui::GetWindowSize().x - 40.0F);

  if (ImGui::Button(ICON_MS_VISIBILITY, ImVec2(20.0F, 0.0F))) {
    // TODO
  }

  ImGui::PopFont();
  ImGui::PopStyleColor(3);
}
