#include <ui/ui_pch.h>
#include <ui/ui_model.h>

#include <imgui/imgui.h>

static void ui_model_select_new_asset(uint64_t selected_index);
static void ui_model_draw_insert();

static vector_t s_ui_model_assets = {};

static int64_t s_ui_selected_model_asset = -1;

void ui_model_create() {
  s_ui_model_assets = database_load_model_assets();

  if (vector_count(&s_ui_model_assets) > 0) {
    model_asset_t *model_asset = (model_asset_t *)vector_front(&s_ui_model_assets);

    s_ui_selected_model_asset = 0;
  }
}
void ui_model_refresh() {
  database_destroy_model_assets(&s_ui_model_assets);

  s_ui_selected_model_asset = -1;

  s_ui_model_assets = database_load_model_assets();
}
void ui_model_draw() {
  if (ImGui::TreeNodeEx("Models", ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding)) {
    ImGui::Dummy(ImVec2(0.0f, 10.0F));

    ui_model_draw_insert();

    ImGui::Dummy(ImVec2(0.0f, 20.0F));

    static ImGuiTableFlags model_table_flags =
      ImGuiTableFlags_Borders |
      ImGuiTableFlags_RowBg |
      ImGuiTableFlags_Resizable |
      ImGuiTableFlags_SizingStretchProp;

    ImGui::Text("Assets");
    if (ImGui::BeginTable("##ModelAssetTable", 2, model_table_flags)) {
      ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableSetupColumn("NAME", ImGuiTableColumnFlags_WidthStretch);
      ImGui::TableHeadersRow();

      uint64_t model_asset_index = 0;
      uint64_t model_asset_count = vector_count(&s_ui_model_assets);

      while (model_asset_index < model_asset_count) {
        model_asset_t *model_asset = (model_asset_t *)vector_at(&s_ui_model_assets, model_asset_index);

        ImGui::TableNextRow();

        ImGui::PushID(model_asset);

        ImGui::TableSetColumnIndex(0);
        ImGui::Text("%d", model_asset->id);

        ImGui::TableSetColumnIndex(1);
        if (ImGui::Selectable(string_buffer(&model_asset->name), (model_asset_index == s_ui_selected_model_asset), ImGuiSelectableFlags_SpanAllColumns)) {
          // TODO
          // ui_model_select_new_asset(model_asset_index);
        }

        ImGui::PopID();

        model_asset_index++;
      }

      ImGui::EndTable();
    }

    ImGui::Dummy(ImVec2(0.0f, 10.0F));

    ImGui::TreePop();
  }
}
void ui_model_destroy() {
  database_destroy_model_assets(&s_ui_model_assets);

  s_ui_selected_model_asset = -1;
}

static void ui_model_select_new_asset(uint64_t selected_index) {
  database_destroy_model_assets(&s_ui_model_assets);

  s_ui_selected_model_asset = selected_index;

  s_ui_model_assets = database_load_model_assets();
}
static void ui_model_draw_insert() {
  // TODO
  static char model_name[0xFF] = "kalista";
  static char model_file_path[0xFF] = "C:\\Users\\burm\\Downloads\\marauder_kalista.glb";

  ImGui::InputText("Model Name", model_name, sizeof(model_name));
  ImGui::InputText("Model File", model_file_path, sizeof(model_file_path));

  if (ImGui::Button("Create Model")) {
    importer_import_model(model_name, model_file_path);

    ui_model_refresh();
  }
}
