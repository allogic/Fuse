#include <editor/ed_pch.h>
#include <editor/ed_main.h>
#include <editor/ed_titlebar.h>
#include <editor/ed_dockspace.h>

#include <editor/dockable/ed_catalog.h>

static void ed_catalog_draw_swapchain(ed_catalog_t *catalog);
static void ed_catalog_draw_renderer(ed_catalog_t *catalog);
static void ed_catalog_draw_pipeline(ed_catalog_t *catalog);
static void ed_catalog_draw_model(ed_catalog_t *catalog);
static void ed_catalog_draw_scene(ed_catalog_t *catalog);

static void ed_catalog_draw_asset_buttons(ed_catalog_t *catalog);

ed_catalog_t ed_catalog_create(eg_context_t *context, asset_type_t asset_type) {
  ed_catalog_t catalog = {0};

  catalog.context = context;
  catalog.is_dirty = 0;
  catalog.is_open = 1;
  catalog.is_docked = 0;
  catalog.selected_asset = -1;
  catalog.asset_type = asset_type;

  switch (catalog.asset_type) {
    case ASSET_TYPE_SWAPCHAIN: {

      catalog.assets = database_load_swapchain_assets();

      break;
    }
    case ASSET_TYPE_RENDERER: {

      catalog.assets = database_load_renderer_assets();

      break;
    }
    case ASSET_TYPE_PIPELINE: {

      catalog.assets = database_load_pipeline_assets();

      break;
    }
    case ASSET_TYPE_MODEL: {

      catalog.assets = database_load_model_assets();

      break;
    }
    case ASSET_TYPE_SCENE: {

      catalog.assets = database_load_scene_assets();

      break;
    }
  }

  return catalog;
}
void ed_catalog_refresh(ed_catalog_t *catalog) {
  switch (catalog->asset_type) {
    case ASSET_TYPE_SWAPCHAIN: {

      database_destroy_swapchain_assets(&catalog->assets);

      break;
    }
    case ASSET_TYPE_RENDERER: {

      database_destroy_renderer_assets(&catalog->assets);

      break;
    }
    case ASSET_TYPE_PIPELINE: {

      database_destroy_pipeline_assets(&catalog->assets);

      break;
    }
    case ASSET_TYPE_MODEL: {

      database_destroy_model_assets(&catalog->assets);

      break;
    }
    case ASSET_TYPE_SCENE: {

      database_destroy_scene_assets(&catalog->assets);

      break;
    }
  }

  switch (catalog->asset_type) {
    case ASSET_TYPE_SWAPCHAIN: {

      catalog->assets = database_load_swapchain_assets();

      break;
    }
    case ASSET_TYPE_RENDERER: {

      catalog->assets = database_load_renderer_assets();

      break;
    }
    case ASSET_TYPE_PIPELINE: {

      catalog->assets = database_load_pipeline_assets();

      break;
    }
    case ASSET_TYPE_MODEL: {

      catalog->assets = database_load_model_assets();

      break;
    }
    case ASSET_TYPE_SCENE: {

      catalog->assets = database_load_scene_assets();

      break;
    }
  }
}
void ed_catalog_draw(ed_catalog_t *catalog) {
  switch (catalog->asset_type) {
    case ASSET_TYPE_SWAPCHAIN: {

      ed_catalog_draw_swapchain(catalog);

      break;
    }
    case ASSET_TYPE_RENDERER: {

      ed_catalog_draw_renderer(catalog);

      break;
    }
    case ASSET_TYPE_PIPELINE: {

      ed_catalog_draw_pipeline(catalog);

      break;
    }
    case ASSET_TYPE_MODEL: {

      ed_catalog_draw_model(catalog);

      break;
    }
    case ASSET_TYPE_SCENE: {

      ed_catalog_draw_scene(catalog);

      break;
    }
  }
}
void ed_catalog_destroy(ed_catalog_t *catalog) {
  switch (catalog->asset_type) {
    case ASSET_TYPE_SWAPCHAIN: {

      database_destroy_swapchain_assets(&catalog->assets);

      break;
    }
    case ASSET_TYPE_RENDERER: {

      database_destroy_renderer_assets(&catalog->assets);

      break;
    }
    case ASSET_TYPE_PIPELINE: {

      database_destroy_pipeline_assets(&catalog->assets);

      break;
    }
    case ASSET_TYPE_MODEL: {

      database_destroy_model_assets(&catalog->assets);

      break;
    }
    case ASSET_TYPE_SCENE: {

      database_destroy_scene_assets(&catalog->assets);

      break;
    }
  }
}

static void ed_catalog_draw_swapchain(ed_catalog_t *catalog) {
  ImGuiTreeNodeFlags tree_node_flags =
    ImGuiTreeNodeFlags_None |
    ImGuiTreeNodeFlags_SpanFullWidth |
    ImGuiTreeNodeFlags_OpenOnArrow |
    ImGuiTreeNodeFlags_FramePadding |
    ImGuiTreeNodeFlags_Leaf;

  uint64_t asset_index = 0;
  uint64_t asset_count = vector_count(&catalog->assets);

  while (asset_index < asset_count) {

    swapchain_asset_t *swapchain_asset = (swapchain_asset_t *)vector_at(&catalog->assets, asset_index);

    if (asset_index == catalog->selected_asset) {
      tree_node_flags |= ImGuiTreeNodeFlags_Selected;
    }

    uint8_t is_open = ImGui::TreeNodeEx(swapchain_asset->name, tree_node_flags);

    if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
      catalog->selected_asset = asset_index;
    }

    ed_catalog_draw_asset_buttons(catalog);

    if (is_open) {
      ImGui::TreePop();
    }

    asset_index++;
  }
}
static void ed_catalog_draw_renderer(ed_catalog_t *catalog) {
  ImGuiTreeNodeFlags tree_node_flags =
    ImGuiTreeNodeFlags_None |
    ImGuiTreeNodeFlags_SpanFullWidth |
    ImGuiTreeNodeFlags_OpenOnArrow |
    ImGuiTreeNodeFlags_FramePadding |
    ImGuiTreeNodeFlags_Leaf;

  uint64_t asset_index = 0;
  uint64_t asset_count = vector_count(&catalog->assets);

  while (asset_index < asset_count) {

    renderer_asset_t *renderer_asset = (renderer_asset_t *)vector_at(&catalog->assets, asset_index);

    if (asset_index == catalog->selected_asset) {
      tree_node_flags |= ImGuiTreeNodeFlags_Selected;
    }

    uint8_t is_open = ImGui::TreeNodeEx(renderer_asset->name, tree_node_flags);

    if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
      catalog->selected_asset = asset_index;
    }

    ed_catalog_draw_asset_buttons(catalog);

    if (is_open) {
      ImGui::TreePop();
    }

    asset_index++;
  }
}
static void ed_catalog_draw_pipeline(ed_catalog_t *catalog) {
  ImGuiTreeNodeFlags tree_node_flags =
    ImGuiTreeNodeFlags_None |
    ImGuiTreeNodeFlags_SpanFullWidth |
    ImGuiTreeNodeFlags_OpenOnArrow |
    ImGuiTreeNodeFlags_FramePadding |
    ImGuiTreeNodeFlags_Leaf;

  uint64_t asset_index = 0;
  uint64_t asset_count = vector_count(&catalog->assets);

  while (asset_index < asset_count) {

    pipeline_asset_t *pipeline_asset = (pipeline_asset_t *)vector_at(&catalog->assets, asset_index);

    if (asset_index == catalog->selected_asset) {
      tree_node_flags |= ImGuiTreeNodeFlags_Selected;
    }

    uint8_t is_open = ImGui::TreeNodeEx(pipeline_asset->name, tree_node_flags);

    if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
      catalog->selected_asset = asset_index;
    }

    ed_catalog_draw_asset_buttons(catalog);

    if (is_open) {
      ImGui::TreePop();
    }

    asset_index++;
  }
}
static void ed_catalog_draw_model(ed_catalog_t *catalog) {
  ImGuiTreeNodeFlags tree_node_flags =
    ImGuiTreeNodeFlags_None |
    ImGuiTreeNodeFlags_SpanFullWidth |
    ImGuiTreeNodeFlags_OpenOnArrow |
    ImGuiTreeNodeFlags_FramePadding |
    ImGuiTreeNodeFlags_Leaf;

  uint64_t asset_index = 0;
  uint64_t asset_count = vector_count(&catalog->assets);

  while (asset_index < asset_count) {

    model_asset_t *model_asset = (model_asset_t *)vector_at(&catalog->assets, asset_index);

    if (asset_index == catalog->selected_asset) {
      tree_node_flags |= ImGuiTreeNodeFlags_Selected;
    }

    uint8_t is_open = ImGui::TreeNodeEx(model_asset->name, tree_node_flags);

    if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
      catalog->selected_asset = asset_index;
    }

    ed_catalog_draw_asset_buttons(catalog);

    if (is_open) {
      ImGui::TreePop();
    }

    asset_index++;
  }
}
static void ed_catalog_draw_scene(ed_catalog_t *catalog) {
  ImGuiTreeNodeFlags tree_node_flags =
    ImGuiTreeNodeFlags_None |
    ImGuiTreeNodeFlags_SpanFullWidth |
    ImGuiTreeNodeFlags_OpenOnArrow |
    ImGuiTreeNodeFlags_FramePadding |
    ImGuiTreeNodeFlags_Leaf;

  uint64_t asset_index = 0;
  uint64_t asset_count = vector_count(&catalog->assets);

  while (asset_index < asset_count) {

    scene_asset_t *scene_asset = (scene_asset_t *)vector_at(&catalog->assets, asset_index);

    if (asset_index == catalog->selected_asset) {
      tree_node_flags |= ImGuiTreeNodeFlags_Selected;
    }

    uint8_t is_open = ImGui::TreeNodeEx(scene_asset->name, tree_node_flags);

    if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
      catalog->selected_asset = asset_index;
    }

    ed_catalog_draw_asset_buttons(catalog);

    if (is_open) {
      ImGui::TreePop();
    }

    asset_index++;
  }
}

static void ed_catalog_draw_asset_buttons(ed_catalog_t *catalog) {
  ImGui::SameLine(ImGui::GetWindowSize().x - 20.0F);

  ImGui::PushStyleColor(ImGuiCol_Button, ED_SHALLOW_GRAY_COLOR);
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ED_LIGHT_GRAY_COLOR);
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ED_ACTIVE_GREY_COLOR);

  ImGui::PushFont(g_editor_material_symbols);

  if (ImGui::Button(ICON_MS_DELETE, ImVec2(20.0F, 0.0F))) {
    // TODO
  }

  ImGui::SameLine(ImGui::GetWindowSize().x - 40.0F);

  if (ImGui::Button(ICON_MS_MERGE, ImVec2(20.0F, 0.0F))) {
    // TODO
  }

  ImGui::PopFont();
  ImGui::PopStyleColor(3);
}
