#include <editor/ed_pch.h>
#include <editor/ed_catalog.h>
#include <editor/ed_titlebar.h>

static void catalog_select_swapchain(uint64_t selected_index);
static void catalog_select_renderer(uint64_t selected_index);
static void catalog_select_pipeline(uint64_t selected_index);
static void catalog_select_model(uint64_t selected_index);

asset_type_t g_catalog_selected_asset_type = ASSET_TYPE_NONE;

vector_t g_catalog_swapchain_assets = {};
vector_t g_catalog_renderer_assets = {};
vector_t g_catalog_pipeline_assets = {};
vector_t g_catalog_model_assets = {};

int64_t g_catalog_selected_swapchain_asset = -1;
int64_t g_catalog_selected_renderer_asset = -1;
int64_t g_catalog_selected_pipeline_asset = -1;
int64_t g_catalog_selected_model_asset = -1;

vector_t g_catalog_pipeline_vertex_input_bindings = {};
vector_t g_catalog_pipeline_descriptor_bindings = {};

vector_t g_catalog_model_meshes = {};

void catalog_create() {
  g_catalog_swapchain_assets = database_load_swapchain_assets();
  g_catalog_renderer_assets = database_load_renderer_assets();
  g_catalog_pipeline_assets = database_load_pipeline_assets();
  g_catalog_model_assets = database_load_model_assets();
}
void catalog_refresh() {
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
  ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar;

  ImGui::Begin("Catalog", &g_titlebar_catalog_open, window_flags);

  ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(50, 50, 50, 255));
  ImGui::BeginChild("CatalogFirstChild");

  ImVec2 second_window_size = ImGui::GetWindowSize();
  ImVec2 second_cursor_position = ImVec2(5.0F, 5.0F);

  second_window_size.x -= 10.0F;
  second_window_size.y -= 30.0F;

  ImGui::SetCursorPos(second_cursor_position);
  ImGui::BeginChild("CatalogSecondChild", second_window_size);

  if (ImGui::TreeNodeEx("Swapchain", ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_FramePadding)) {

    ImGui::Dummy(ImVec2(0.0f, 5.0F));

    ImGui::BeginTable("##SwapchainAssetTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchProp);

    ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("NAME", ImGuiTableColumnFlags_WidthStretch);

    ImGui::TableHeadersRow();

    uint64_t swapchain_asset_index = 0;
    uint64_t swapchain_asset_count = vector_count(&g_catalog_swapchain_assets);

    while (swapchain_asset_index < swapchain_asset_count) {
      swapchain_asset_t *swapchain_asset = (swapchain_asset_t *)vector_at(&g_catalog_swapchain_assets, swapchain_asset_index);

      ImGui::TableNextRow();

      ImGui::TableSetColumnIndex(0);
      ImGui::Text("%d", swapchain_asset->id);

      ImGui::TableSetColumnIndex(1);
      if (ImGui::Selectable(swapchain_asset->name, (swapchain_asset_index == g_catalog_selected_swapchain_asset) && (g_catalog_selected_asset_type == ASSET_TYPE_SWAPCHAIN), ImGuiSelectableFlags_SpanAllColumns)) {
        catalog_select_swapchain(swapchain_asset_index);
      }

      swapchain_asset_index++;
    }

    ImGui::EndTable();

    ImGui::Dummy(ImVec2(0.0f, 5.0F));

    ImGui::TreePop();
  }

  if (ImGui::TreeNodeEx("Renderer", ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_FramePadding)) {

    ImGui::Dummy(ImVec2(0.0f, 5.0F));

    ImGui::BeginTable("##RendererSwapchainAssetTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchProp);

    ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("NAME", ImGuiTableColumnFlags_WidthStretch);

    ImGui::TableHeadersRow();

    uint64_t renderer_asset_index = 0;
    uint64_t renderer_asset_count = vector_count(&g_catalog_renderer_assets);

    while (renderer_asset_index < renderer_asset_count) {
      renderer_asset_t *renderer_asset = (renderer_asset_t *)vector_at(&g_catalog_renderer_assets, renderer_asset_index);

      ImGui::TableNextRow();

      ImGui::TableSetColumnIndex(0);
      ImGui::Text("%d", renderer_asset->id);

      ImGui::TableSetColumnIndex(1);
      if (ImGui::Selectable(renderer_asset->name, (renderer_asset_index == g_catalog_selected_renderer_asset) && (g_catalog_selected_asset_type == ASSET_TYPE_RENDERER), ImGuiSelectableFlags_SpanAllColumns)) {
        catalog_select_renderer(renderer_asset_index);
      }

      renderer_asset_index++;
    }

    ImGui::EndTable();

    ImGui::Dummy(ImVec2(0.0f, 5.0F));

    ImGui::TreePop();
  }

  if (ImGui::TreeNodeEx("Pipeline", ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_FramePadding)) {

    ImGui::Dummy(ImVec2(0.0f, 5.0F));

    ImGui::BeginTable("##PipelineAssetTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchProp);

    ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("NAME", ImGuiTableColumnFlags_WidthStretch);

    ImGui::TableHeadersRow();

    uint64_t pipeline_asset_index = 0;
    uint64_t pipeline_asset_count = vector_count(&g_catalog_pipeline_assets);

    while (pipeline_asset_index < pipeline_asset_count) {
      pipeline_asset_t *pipeline_asset = (pipeline_asset_t *)vector_at(&g_catalog_pipeline_assets, pipeline_asset_index);

      ImGui::TableNextRow();

      ImGui::TableSetColumnIndex(0);
      ImGui::Text("%d", pipeline_asset->id);

      ImGui::TableSetColumnIndex(1);
      if (ImGui::Selectable(pipeline_asset->name, (pipeline_asset_index == g_catalog_selected_pipeline_asset) && (g_catalog_selected_asset_type == ASSET_TYPE_PIPELINE), ImGuiSelectableFlags_SpanAllColumns)) {
        catalog_select_pipeline(pipeline_asset_index);
      }

      pipeline_asset_index++;
    }

    ImGui::EndTable();

    ImGui::Dummy(ImVec2(0.0f, 5.0F));

    ImGui::TreePop();
  }

  if (ImGui::TreeNodeEx("Model", ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_FramePadding)) {

    ImGui::Dummy(ImVec2(0.0f, 5.0F));

    ImGui::BeginTable("##ModelAssetTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchProp);

    ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("NAME", ImGuiTableColumnFlags_WidthStretch);

    ImGui::TableHeadersRow();

    uint64_t model_asset_index = 0;
    uint64_t model_asset_count = vector_count(&g_catalog_model_assets);

    while (model_asset_index < model_asset_count) {
      model_asset_t *model_asset = (model_asset_t *)vector_at(&g_catalog_model_assets, model_asset_index);

      ImGui::TableNextRow();

      ImGui::TableSetColumnIndex(0);
      ImGui::Text("%d", model_asset->id);

      ImGui::TableSetColumnIndex(1);
      if (ImGui::Selectable(model_asset->name, (model_asset_index == g_catalog_selected_model_asset) && (g_catalog_selected_asset_type == ASSET_TYPE_MODEL), ImGuiSelectableFlags_SpanAllColumns)) {
        catalog_select_model(model_asset_index);
      }

      model_asset_index++;
    }

    ImGui::EndTable();

    ImGui::Dummy(ImVec2(0.0f, 5.0F));

    ImGui::TreePop();
  }

  ImGui::EndChild();

  ImGui::EndChild();
  ImGui::PopStyleColor();

  ImGui::End();
}
void catalog_destroy() {
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
