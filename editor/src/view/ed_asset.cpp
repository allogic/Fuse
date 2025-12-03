#include <editor/ed_pch.h>

#include <editor/view/ed_asset.h>

char const *g_asset_type_names[EG_ASSET_TYPE_COUNT]{
  "Swapchain",
  "Renderer",
  "Pipeline",
  "Model",
  "Scene",
  "Graph",
};

ed_asset_t::ed_asset_t(char const *name) : ed_view_t(name) {
  create_assets();
}
ed_asset_t::~ed_asset_t() {
  destroy_assets();
}

void ed_asset_t::refresh() {
  if (m_is_dirty) {

    m_is_dirty = 0;
    m_selected_asset = -1;

    destroy_assets();
    create_assets();
  }
}
void ed_asset_t::draw() {
  ImVec2 window_size = ImGui::GetWindowSize();
  ImVec2 controlbar_size = ImVec2(window_size.x, 30.0F);

  ImGui::PushStyleColor(ImGuiCol_ChildBg, ED_SHALLOW_GRAY_COLOR);
  ImGui::BeginChild("controlbar", controlbar_size);
  ImGui::PopStyleColor(1);

  ImGui::SetCursorPos(ImVec2(3.0F, 3.0F));
  ImGui::SetNextItemWidth(120.0F);

  if (ImGui::BeginCombo("##Type", g_asset_type_names[m_asset_type])) {

    uint64_t asset_index = 0;
    uint64_t asset_count = EG_ASSET_TYPE_COUNT;

    while (asset_index < asset_count) {

      uint8_t is_selected = (m_asset_type == asset_index);

      if (ImGui::Selectable(g_asset_type_names[asset_index], is_selected)) {
        m_asset_type = (eg_asset_type_t)asset_index;
        m_is_dirty = 1;

        refresh();
      }

      if (is_selected) {
        ImGui::SetItemDefaultFocus();
      }

      asset_index++;
    }

    ImGui::EndCombo();
  }

  ImGui::SameLine();

  if (ImGui::Button("Add")) {
  }

  ImGui::SameLine();

  if (ImGui::Button("Remove")) {
  }

  ImGui::EndChild();

  static ImGuiTableFlags table_flags =
    ImGuiTableFlags_BordersInner |
    ImGuiTableFlags_RowBg |
    ImGuiTableFlags_Resizable |
    ImGuiTableFlags_SizingStretchProp;

  ImVec2 cursor_screen_position = ImGui::GetCursorScreenPos();
  cursor_screen_position.y -= 4.0F;
  ImGui::SetCursorScreenPos(cursor_screen_position);

  if (ImGui::BeginTable("##asset_table", 2, table_flags)) {

    ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("NAME", ImGuiTableColumnFlags_WidthStretch);

    ImGui::TableHeadersRow();

    uint64_t asset_index = 0;
    uint64_t asset_count = eg_vector_count(m_assets);

    while (asset_index < asset_count) {

      switch (m_asset_type) {
        case EG_ASSET_TYPE_SWAPCHAIN: {

          draw_swapchain(asset_index);

          break;
        }
        case EG_ASSET_TYPE_RENDERER: {

          draw_renderer(asset_index);

          break;
        }
        case EG_ASSET_TYPE_PIPELINE: {

          draw_pipeline(asset_index);

          break;
        }
        case EG_ASSET_TYPE_MODEL: {

          draw_model(asset_index);

          break;
        }
        case EG_ASSET_TYPE_SCENE: {

          draw_scene(asset_index);

          break;
        }
        case EG_ASSET_TYPE_GRAPH: {

          draw_graph(asset_index);

          break;
        }
      }

      asset_index++;
    }

    ImGui::EndTable();
  }
}

void ed_asset_t::draw_swapchain(uint64_t asset_index) {
  eg_swapchain_asset_t *swapchain_asset = (eg_swapchain_asset_t *)eg_vector_at(m_assets, asset_index);

  ImGui::TableNextRow();

  ImGui::TableSetColumnIndex(0);
  ImGui::Text("%d", swapchain_asset->id);

  ImGui::TableSetColumnIndex(1);
  if (ImGui::Selectable(swapchain_asset->name, asset_index == m_selected_asset, ImGuiSelectableFlags_SpanAllColumns)) {
    m_selected_asset = asset_index;
  }
}
void ed_asset_t::draw_renderer(uint64_t asset_index) {
  eg_renderer_asset_t *renderer_asset = (eg_renderer_asset_t *)eg_vector_at(m_assets, asset_index);

  ImGui::TableNextRow();

  ImGui::TableSetColumnIndex(0);
  ImGui::Text("%d", renderer_asset->id);

  ImGui::TableSetColumnIndex(1);
  if (ImGui::Selectable(renderer_asset->name, asset_index == m_selected_asset, ImGuiSelectableFlags_SpanAllColumns)) {
    m_selected_asset = asset_index;
  }
}
void ed_asset_t::draw_pipeline(uint64_t asset_index) {
  eg_pipeline_asset_t *pipeline_asset = (eg_pipeline_asset_t *)eg_vector_at(m_assets, asset_index);

  ImGui::TableNextRow();

  ImGui::TableSetColumnIndex(0);
  ImGui::Text("%d", pipeline_asset->id);

  ImGui::TableSetColumnIndex(1);
  if (ImGui::Selectable(pipeline_asset->name, asset_index == m_selected_asset, ImGuiSelectableFlags_SpanAllColumns)) {
    m_selected_asset = asset_index;
  }
}
void ed_asset_t::draw_model(uint64_t asset_index) {
  eg_model_asset_t *model_asset = (eg_model_asset_t *)eg_vector_at(m_assets, asset_index);

  ImGui::TableNextRow();

  ImGui::TableSetColumnIndex(0);
  ImGui::Text("%d", model_asset->id);

  ImGui::TableSetColumnIndex(1);
  if (ImGui::Selectable(model_asset->name, asset_index == m_selected_asset, ImGuiSelectableFlags_SpanAllColumns)) {
    m_selected_asset = asset_index;
  }
}
void ed_asset_t::draw_scene(uint64_t asset_index) {
  eg_scene_asset_t *scene_asset = (eg_scene_asset_t *)eg_vector_at(m_assets, asset_index);

  ImGui::TableNextRow();

  ImGui::TableSetColumnIndex(0);
  ImGui::Text("%d", scene_asset->id);

  ImGui::TableSetColumnIndex(1);
  if (ImGui::Selectable(scene_asset->name, asset_index == m_selected_asset, ImGuiSelectableFlags_SpanAllColumns)) {
    m_selected_asset = asset_index;
  }
}
void ed_asset_t::draw_graph(uint64_t asset_index) {
  eg_graph_asset_t *graph_asset = (eg_graph_asset_t *)eg_vector_at(m_assets, asset_index);

  ImGui::TableNextRow();

  ImGui::TableSetColumnIndex(0);
  ImGui::Text("%d", graph_asset->id);

  ImGui::TableSetColumnIndex(1);
  if (ImGui::Selectable(graph_asset->name, asset_index == m_selected_asset, ImGuiSelectableFlags_SpanAllColumns)) {
    m_selected_asset = asset_index;
  }
}

void ed_asset_t::create_assets() {
  switch (m_asset_type) {
    case EG_ASSET_TYPE_SWAPCHAIN: {

      m_assets = eg_database_load_all_swapchain_assets();

      break;
    }
    case EG_ASSET_TYPE_RENDERER: {

      m_assets = eg_database_load_all_renderer_assets();

      break;
    }
    case EG_ASSET_TYPE_PIPELINE: {

      m_assets = eg_database_load_all_pipeline_assets();

      break;
    }
    case EG_ASSET_TYPE_MODEL: {

      m_assets = eg_database_load_all_model_assets();

      break;
    }
    case EG_ASSET_TYPE_SCENE: {

      m_assets = eg_database_load_all_scene_assets();

      break;
    }
    case EG_ASSET_TYPE_GRAPH: {

      m_assets = eg_database_load_all_graph_assets();

      break;
    }
  }
}
void ed_asset_t::destroy_assets() {
  switch (m_asset_type) {
    case EG_ASSET_TYPE_SWAPCHAIN: {

      eg_database_destroy_swapchain_assets(m_assets);

      break;
    }
    case EG_ASSET_TYPE_RENDERER: {

      eg_database_destroy_renderer_assets(m_assets);

      break;
    }
    case EG_ASSET_TYPE_PIPELINE: {

      eg_database_destroy_pipeline_assets(m_assets);

      break;
    }
    case EG_ASSET_TYPE_MODEL: {

      eg_database_destroy_model_assets(m_assets);

      break;
    }
    case EG_ASSET_TYPE_SCENE: {

      eg_database_destroy_scene_assets(m_assets);

      break;
    }
    case EG_ASSET_TYPE_GRAPH: {

      eg_database_destroy_graph_assets(m_assets);

      break;
    }
  }
}
