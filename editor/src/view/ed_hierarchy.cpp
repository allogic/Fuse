#include <editor/ed_pch.h>
#include <editor/ed_titlebar.h>

#include <editor/view/ed_hierarchy.h>

/*
static void ed_hierarchy_view_draw_tree(ed_hierarchy_view_t *hierarchy, ecs_world_t *world, ecs_entity_t entity);
static void ed_hierarchy_view_draw_context_menu(ed_hierarchy_view_t *hierarchy, ecs_world_t *world, ecs_entity_t entity);
static void ed_hierarchy_view_draw_entity_buttons(ed_hierarchy_view_t *hierarchy, ecs_world_t *world, ecs_entity_t entity);

ed_hierarchy_view_t *ed_hierarchy_view_create(eg_context_t *context) {
  ed_hierarchy_view_t *hierarchy = (ed_hierarchy_view_t *)eg_heap_alloc(sizeof(ed_hierarchy_view_t), 1, 0);

  hierarchy->base.context = context;
  hierarchy->base.is_dirty = 0;
  hierarchy->base.is_open = 1;
  hierarchy->base.is_docked = 1;

  hierarchy->selected_entity = 0;

  return hierarchy;
}
void ed_hierarchy_view_refresh(ed_hierarchy_view_t *hierarchy) {
  // TODO
}
void ed_hierarchy_view_draw(ed_hierarchy_view_t *hierarchy) {
  eg_scene_t *scene = eg_context_scene(hierarchy->base.context);
  ecs_world_t *world = eg_scene_world(scene);
  ecs_entity_t root = eg_scene_root(scene);

  ed_hierarchy_view_draw_tree(hierarchy, world, root);
}
void ed_hierarchy_view_destroy(ed_hierarchy_view_t *hierarchy) {
  eg_heap_free(hierarchy);
}

static void ed_hierarchy_view_draw_tree(ed_hierarchy_view_t *hierarchy, ecs_world_t *world, ecs_entity_t entity) {
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

  if (entity == hierarchy->selected_entity) {
    tree_node_flags |= ImGuiTreeNodeFlags_Selected;
  }

  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, 0.0F));

  uint8_t is_open = ImGui::TreeNodeEx(name, tree_node_flags);

  if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
    hierarchy->selected_entity = entity;
  }

  ed_hierarchy_view_draw_context_menu(hierarchy, world, entity);
  ed_hierarchy_view_draw_entity_buttons(hierarchy, world, entity);

  ImGui::PopStyleVar(1);

  if (is_open) {

    do {

      uint64_t child_index = 0;
      uint64_t child_count = child_iter.count;

      while (child_index < child_count) {

        ed_hierarchy_view_draw_tree(hierarchy, world, child_iter.entities[child_index]);

        child_index++;
      }

    } while (ecs_children_next(&child_iter));

    ImGui::TreePop();
  }
}
static void ed_hierarchy_view_draw_context_menu(ed_hierarchy_view_t *hierarchy, ecs_world_t *world, ecs_entity_t entity) {
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
static void ed_hierarchy_view_draw_entity_buttons(ed_hierarchy_view_t *hierarchy, ecs_world_t *world, ecs_entity_t entity) {
  ImGui::SameLine(ImGui::GetWindowSize().x - 20.0F);

  ImGui::PushStyleColor(ImGuiCol_Button, ED_SHALLOW_GRAY_COLOR);
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ED_LIGHT_GRAY_COLOR);
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ED_ACTIVE_GREY_COLOR);

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
*/
