#include <editor/ed_pch.h>
#include <editor/ed_main.h>
#include <editor/ed_titlebar.h>
#include <editor/ed_dockspace.h>

#include <editor/dockable/ed_catalog.h>

static void catalog_draw_swapchain(void);
static void catalog_draw_renderer(void);
static void catalog_draw_pipeline(void);
static void catalog_draw_model(void);

static void catalog_draw_asset_buttons(void);

static void catalog_select_swapchain(uint64_t selected_index);
static void catalog_select_renderer(uint64_t selected_index);
static void catalog_select_pipeline(uint64_t selected_index);
static void catalog_select_model(uint64_t selected_index);

uint8_t g_catalog_is_open = 1;
uint8_t g_catalog_is_docked = 0;

asset_type_t g_catalog_selected_asset_type = ASSET_TYPE_NONE;

vector_t g_catalog_swapchain_assets = {0};
vector_t g_catalog_renderer_assets = {0};
vector_t g_catalog_pipeline_assets = {0};
vector_t g_catalog_model_assets = {0};

int64_t g_catalog_selected_swapchain_asset = -1;
int64_t g_catalog_selected_renderer_asset = -1;
int64_t g_catalog_selected_pipeline_asset = -1;
int64_t g_catalog_selected_model_asset = -1;

vector_t g_catalog_pipeline_vertex_input_bindings = {0};
vector_t g_catalog_pipeline_descriptor_bindings = {0};

vector_t g_catalog_model_meshes = {0};

void catalog_create(context_t *context) {
  g_catalog_swapchain_assets = database_load_swapchain_assets();
  g_catalog_renderer_assets = database_load_renderer_assets();
  g_catalog_pipeline_assets = database_load_pipeline_assets();
  g_catalog_model_assets = database_load_model_assets();
}
void catalog_refresh(context_t *context) {
  database_destroy_swapchain_assets(&g_catalog_swapchain_assets);
  database_destroy_renderer_assets(&g_catalog_renderer_assets);
  database_destroy_pipeline_assets(&g_catalog_pipeline_assets);
  database_destroy_model_assets(&g_catalog_model_assets);

  g_catalog_selected_swapchain_asset = -1;
  g_catalog_selected_renderer_asset = -1;
  g_catalog_selected_pipeline_asset = -1;
  g_catalog_selected_model_asset = -1;

  g_catalog_swapchain_assets = database_load_swapchain_assets();
  g_catalog_renderer_assets = database_load_renderer_assets();
  g_catalog_pipeline_assets = database_load_pipeline_assets();
  g_catalog_model_assets = database_load_model_assets();
}
void catalog_draw(context_t *context) {
  if (dockspace_begin_child("Catalog", &g_catalog_is_open, &g_catalog_is_docked)) {

    catalog_draw_swapchain();
    catalog_draw_renderer();
    catalog_draw_pipeline();
    catalog_draw_model();

    dockspace_end_child(g_catalog_is_docked);
  }
}
void catalog_destroy(context_t *context) {
  if (g_catalog_selected_pipeline_asset != -1) {
    database_destroy_pipeline_vertex_input_bindings(&g_catalog_pipeline_vertex_input_bindings);
    database_destroy_pipeline_descriptor_bindings(&g_catalog_pipeline_descriptor_bindings);
  }

  if (g_catalog_selected_model_asset != -1) {
    database_destroy_model_meshes(&g_catalog_model_meshes);
  }

  database_destroy_swapchain_assets(&g_catalog_swapchain_assets);
  database_destroy_renderer_assets(&g_catalog_renderer_assets);
  database_destroy_pipeline_assets(&g_catalog_pipeline_assets);
  database_destroy_model_assets(&g_catalog_model_assets);

  g_catalog_selected_swapchain_asset = -1;
  g_catalog_selected_renderer_asset = -1;
  g_catalog_selected_pipeline_asset = -1;
  g_catalog_selected_model_asset = -1;
}

static void catalog_draw_swapchain(void) {
  ImGuiTreeNodeFlags tree_node_flags =
    ImGuiTreeNodeFlags_None |
    ImGuiTreeNodeFlags_SpanFullWidth |
    ImGuiTreeNodeFlags_OpenOnArrow |
    ImGuiTreeNodeFlags_FramePadding;

  if (ImGui::TreeNodeEx("Swapchain", tree_node_flags)) {

    tree_node_flags |= ImGuiTreeNodeFlags_Leaf;

    uint64_t swapchain_asset_index = 0;
    uint64_t swapchain_asset_count = vector_count(&g_catalog_swapchain_assets);

    while (swapchain_asset_index < swapchain_asset_count) {

      swapchain_asset_t *swapchain_asset = (swapchain_asset_t *)vector_at(&g_catalog_swapchain_assets, swapchain_asset_index);

      if ((swapchain_asset_index == g_catalog_selected_swapchain_asset) && (g_catalog_selected_asset_type == ASSET_TYPE_SWAPCHAIN)) {
        tree_node_flags |= ImGuiTreeNodeFlags_Selected;
      }

      uint8_t is_open = ImGui::TreeNodeEx(swapchain_asset->name, tree_node_flags);

      if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
        catalog_select_swapchain(swapchain_asset_index);
      }

      catalog_draw_asset_buttons();

      if (is_open) {
        ImGui::TreePop();
      }

      swapchain_asset_index++;
    }

    ImGui::TreePop();
  }
}
static void catalog_draw_renderer(void) {
  ImGuiTreeNodeFlags tree_node_flags =
    ImGuiTreeNodeFlags_None |
    ImGuiTreeNodeFlags_SpanFullWidth |
    ImGuiTreeNodeFlags_OpenOnArrow |
    ImGuiTreeNodeFlags_FramePadding;

  if (ImGui::TreeNodeEx("Renderer", tree_node_flags)) {

    tree_node_flags |= ImGuiTreeNodeFlags_Leaf;

    uint64_t renderer_asset_index = 0;
    uint64_t renderer_asset_count = vector_count(&g_catalog_renderer_assets);

    while (renderer_asset_index < renderer_asset_count) {

      renderer_asset_t *renderer_asset = (renderer_asset_t *)vector_at(&g_catalog_renderer_assets, renderer_asset_index);

      if ((renderer_asset_index == g_catalog_selected_renderer_asset) && (g_catalog_selected_asset_type == ASSET_TYPE_RENDERER)) {
        tree_node_flags |= ImGuiTreeNodeFlags_Selected;
      }

      uint8_t is_open = ImGui::TreeNodeEx(renderer_asset->name, tree_node_flags);

      if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
        catalog_select_renderer(renderer_asset_index);
      }

      catalog_draw_asset_buttons();

      if (is_open) {
        ImGui::TreePop();
      }

      renderer_asset_index++;
    }

    ImGui::TreePop();
  }
}
static void catalog_draw_pipeline(void) {
  ImGuiTreeNodeFlags tree_node_flags =
    ImGuiTreeNodeFlags_None |
    ImGuiTreeNodeFlags_SpanFullWidth |
    ImGuiTreeNodeFlags_OpenOnArrow |
    ImGuiTreeNodeFlags_FramePadding;

  if (ImGui::TreeNodeEx("Pipeline", tree_node_flags)) {

    tree_node_flags |= ImGuiTreeNodeFlags_Leaf;

    uint64_t pipeline_asset_index = 0;
    uint64_t pipeline_asset_count = vector_count(&g_catalog_pipeline_assets);

    while (pipeline_asset_index < pipeline_asset_count) {

      pipeline_asset_t *pipeline_asset = (pipeline_asset_t *)vector_at(&g_catalog_pipeline_assets, pipeline_asset_index);

      if ((pipeline_asset_index == g_catalog_selected_pipeline_asset) && (g_catalog_selected_asset_type == ASSET_TYPE_PIPELINE)) {
        tree_node_flags |= ImGuiTreeNodeFlags_Selected;
      }

      uint8_t is_open = ImGui::TreeNodeEx(pipeline_asset->name, tree_node_flags);

      if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
        catalog_select_pipeline(pipeline_asset_index);
      }

      catalog_draw_asset_buttons();

      if (is_open) {
        ImGui::TreePop();
      }

      pipeline_asset_index++;
    }

    ImGui::TreePop();
  }
}
static void catalog_draw_model(void) {
  ImGuiTreeNodeFlags tree_node_flags =
    ImGuiTreeNodeFlags_None |
    ImGuiTreeNodeFlags_SpanFullWidth |
    ImGuiTreeNodeFlags_OpenOnArrow |
    ImGuiTreeNodeFlags_FramePadding;

  if (ImGui::TreeNodeEx("Model", tree_node_flags)) {

    tree_node_flags |= ImGuiTreeNodeFlags_Leaf;

    uint64_t model_asset_index = 0;
    uint64_t model_asset_count = vector_count(&g_catalog_model_assets);

    while (model_asset_index < model_asset_count) {

      model_asset_t *model_asset = (model_asset_t *)vector_at(&g_catalog_model_assets, model_asset_index);

      if ((model_asset_index == g_catalog_selected_model_asset) && (g_catalog_selected_asset_type == ASSET_TYPE_MODEL)) {
        tree_node_flags |= ImGuiTreeNodeFlags_Selected;
      }

      uint8_t is_open = ImGui::TreeNodeEx(model_asset->name, tree_node_flags);

      if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
        catalog_select_model(model_asset_index);
      }

      catalog_draw_asset_buttons();

      if (is_open) {
        ImGui::TreePop();
      }

      model_asset_index++;
    }

    ImGui::TreePop();
  }
}

static void catalog_draw_asset_buttons(void) {
  ImGui::SameLine(ImGui::GetWindowSize().x - 20.0F);

  ImGui::PushStyleColor(ImGuiCol_Button, EDITOR_DOCKING_BACKGROUND_COLOR);
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, EDITOR_HIGHLIGHT_COLOR);
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, EDITOR_ACTIVE_COLOR);

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

static void catalog_select_swapchain(uint64_t selected_index) {
  g_catalog_selected_asset_type = ASSET_TYPE_SWAPCHAIN;
  g_catalog_selected_swapchain_asset = selected_index;
}
static void catalog_select_renderer(uint64_t selected_index) {
  g_catalog_selected_asset_type = ASSET_TYPE_RENDERER;
  g_catalog_selected_renderer_asset = selected_index;
}
static void catalog_select_pipeline(uint64_t selected_index) {
  if (g_catalog_selected_pipeline_asset != -1) {
    database_destroy_pipeline_vertex_input_bindings(&g_catalog_pipeline_vertex_input_bindings);
    database_destroy_pipeline_descriptor_bindings(&g_catalog_pipeline_descriptor_bindings);
  }

  g_catalog_selected_asset_type = ASSET_TYPE_PIPELINE;
  g_catalog_selected_pipeline_asset = selected_index;

  pipeline_asset_t *pipeline_asset = (pipeline_asset_t *)vector_at(&g_catalog_pipeline_assets, g_catalog_selected_pipeline_asset);

  g_catalog_pipeline_vertex_input_bindings = database_load_pipeline_vertex_input_bindings_by_id(pipeline_asset->id);
  g_catalog_pipeline_descriptor_bindings = database_load_pipeline_descriptor_bindings_by_id(pipeline_asset->id);
}
static void catalog_select_model(uint64_t selected_index) {
  if (g_catalog_selected_model_asset != -1) {
    database_destroy_model_meshes(&g_catalog_model_meshes);
  }

  g_catalog_selected_asset_type = ASSET_TYPE_MODEL;
  g_catalog_selected_model_asset = selected_index;

  model_asset_t *model_asset = (model_asset_t *)vector_at(&g_catalog_model_assets, g_catalog_selected_model_asset);

  g_catalog_model_meshes = database_load_model_meshes_by_id(model_asset->id);
}
