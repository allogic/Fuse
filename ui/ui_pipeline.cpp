#include <ui/ui_pch.h>
#include <ui/ui_pipeline.h>

#include <imgui/imgui.h>

static void ui_pipeline_select_new_asset(uint64_t selected_index);
static void ui_pipeline_draw_insert();

static vector_t s_ui_pipeline_assets = {};
static vector_t s_ui_pipeline_vertex_input_bindings = {};
static vector_t s_ui_pipeline_descriptor_bindings = {};

static int64_t s_ui_selected_pipeline_asset = -1;
static int64_t s_ui_selected_pipeline_vertex_input_bindings = -1;
static int64_t s_ui_selected_pipeline_descriptor_bindings = -1;

void ui_pipeline_create() {
  s_ui_pipeline_assets = database_load_pipeline_assets();

  if (vector_count(&s_ui_pipeline_assets) > 0) {
    pipeline_asset_t *pipeline_asset = (pipeline_asset_t *)vector_front(&s_ui_pipeline_assets);

    s_ui_selected_pipeline_asset = 0;

    s_ui_pipeline_vertex_input_bindings = database_load_pipeline_vertex_input_bindings_by_id(pipeline_asset->id);
    s_ui_pipeline_descriptor_bindings = database_load_pipeline_descriptor_bindings_by_id(pipeline_asset->id);
  }
}
void ui_pipeline_refresh() {
  if (vector_count(&s_ui_pipeline_assets) > 0) {
    pipeline_asset_t *pipeline = (pipeline_asset_t *)vector_front(&s_ui_pipeline_assets);

    database_destroy_pipeline_vertex_input_bindings(&s_ui_pipeline_vertex_input_bindings);
    database_destroy_pipeline_descriptor_bindings(&s_ui_pipeline_descriptor_bindings);
  }

  database_destroy_pipeline_assets(&s_ui_pipeline_assets);

  s_ui_selected_pipeline_asset = -1;
  s_ui_selected_pipeline_vertex_input_bindings = -1;
  s_ui_selected_pipeline_descriptor_bindings = -1;

  s_ui_pipeline_assets = database_load_pipeline_assets();

  if (vector_count(&s_ui_pipeline_assets) > 0) {
    pipeline_asset_t *pipeline_asset = (pipeline_asset_t *)vector_front(&s_ui_pipeline_assets);

    s_ui_pipeline_vertex_input_bindings = database_load_pipeline_vertex_input_bindings_by_id(pipeline_asset->id);
    s_ui_pipeline_descriptor_bindings = database_load_pipeline_descriptor_bindings_by_id(pipeline_asset->id);
  }
}
void ui_pipeline_draw() {
  if (ImGui::TreeNodeEx("Pipelines", ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding)) {
    ImGui::Dummy(ImVec2(0.0f, 10.0F));

    ui_pipeline_draw_insert();

    ImGui::Dummy(ImVec2(0.0f, 20.0F));

    static ImGuiTableFlags pipeline_table_flags =
      ImGuiTableFlags_Borders |
      ImGuiTableFlags_RowBg |
      ImGuiTableFlags_Resizable |
      ImGuiTableFlags_SizingStretchProp;

    ImGui::Text("Assets");
    if (ImGui::BeginTable("##PipelineAssetTable", 5, pipeline_table_flags)) {
      ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableSetupColumn("NAME", ImGuiTableColumnFlags_WidthStretch);
      ImGui::TableSetupColumn("TYPE", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableSetupColumn("LINK_INDEX", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableSetupColumn("AUTO_CREATE", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableHeadersRow();

      uint64_t pipeline_asset_index = 0;
      uint64_t pipeline_asset_count = vector_count(&s_ui_pipeline_assets);

      while (pipeline_asset_index < pipeline_asset_count) {
        pipeline_asset_t *pipeline_asset = (pipeline_asset_t *)vector_at(&s_ui_pipeline_assets, pipeline_asset_index);

        ImGui::TableNextRow();

        ImGui::PushID(pipeline_asset);

        ImGui::TableSetColumnIndex(0);
        ImGui::Text("%d", pipeline_asset->id);

        ImGui::TableSetColumnIndex(1);
        if (ImGui::Selectable(string_buffer(&pipeline_asset->name), (pipeline_asset_index == s_ui_selected_pipeline_asset), ImGuiSelectableFlags_SpanAllColumns)) {
          ui_pipeline_select_new_asset(pipeline_asset_index);
        }

        ImGui::TableSetColumnIndex(2);
        ImGui::Text("%d", pipeline_asset->type);

        ImGui::TableSetColumnIndex(3);
        ImGui::Text("%d", pipeline_asset->link_index);

        ImGui::TableSetColumnIndex(4);
        ImGui::Text("%d", pipeline_asset->auto_create);

        ImGui::PopID();

        pipeline_asset_index++;
      }

      ImGui::EndTable();
    }

    ImGui::Dummy(ImVec2(0.0f, 20.0F));

    static ImGuiTableFlags pipeline_vertex_input_binding_table_flags =
      ImGuiTableFlags_Borders |
      ImGuiTableFlags_RowBg |
      ImGuiTableFlags_Resizable |
      ImGuiTableFlags_SizingStretchProp;

    ImGui::Text("Vertex Input Bindings");
    if (ImGui::BeginTable("##PipelineVertexInputBindingTable", 8, pipeline_vertex_input_binding_table_flags)) {
      ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableSetupColumn("BINDING_NAME", ImGuiTableColumnFlags_WidthStretch);
      ImGui::TableSetupColumn("BINDING", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableSetupColumn("LOCATION", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableSetupColumn("STRIDE", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableSetupColumn("FORMAT", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableSetupColumn("OFFSET", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableSetupColumn("INPUT_RATE", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableHeadersRow();

      uint64_t pipeline_vertex_input_binding_index = 0;
      uint64_t pipeline_vertex_input_binding_count = vector_count(&s_ui_pipeline_vertex_input_bindings);

      while (pipeline_vertex_input_binding_index < pipeline_vertex_input_binding_count) {
        pipeline_vertex_input_binding_t *pipeline_vertex_input_binding = (pipeline_vertex_input_binding_t *)vector_at(&s_ui_pipeline_vertex_input_bindings, pipeline_vertex_input_binding_index);

        ImGui::TableNextRow();

        ImGui::PushID(pipeline_vertex_input_binding);

        ImGui::TableSetColumnIndex(0);
        ImGui::Text("%d", pipeline_vertex_input_binding->id);

        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%s", string_buffer(&pipeline_vertex_input_binding->binding_name));

        ImGui::TableSetColumnIndex(2);
        ImGui::Text("%d", pipeline_vertex_input_binding->binding);

        ImGui::TableSetColumnIndex(3);
        ImGui::Text("%d", pipeline_vertex_input_binding->location);

        ImGui::TableSetColumnIndex(4);
        ImGui::Text("%d", pipeline_vertex_input_binding->stride);

        ImGui::TableSetColumnIndex(5);
        ImGui::Text("%d", pipeline_vertex_input_binding->format);

        ImGui::TableSetColumnIndex(6);
        ImGui::Text("%d", pipeline_vertex_input_binding->offset);

        ImGui::TableSetColumnIndex(7);
        ImGui::Text("%d", pipeline_vertex_input_binding->input_rate);

        ImGui::PopID();

        pipeline_vertex_input_binding_index++;
      }

      ImGui::EndTable();
    }

    ImGui::Dummy(ImVec2(0.0f, 20.0F));

    static ImGuiTableFlags pipeline_descriptor_binding_table_flags =
      ImGuiTableFlags_Borders |
      ImGuiTableFlags_RowBg |
      ImGuiTableFlags_Resizable |
      ImGuiTableFlags_SizingStretchProp;

    ImGui::Text("Descriptor Bindings");
    if (ImGui::BeginTable("##PipelineDescriptorBindingTable", 7, pipeline_descriptor_binding_table_flags)) {
      ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableSetupColumn("BINDING_NAME", ImGuiTableColumnFlags_WidthStretch);
      ImGui::TableSetupColumn("BINDING", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableSetupColumn("DESCRIPTOR_TYPE", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableSetupColumn("DESCRIPTOR_COUNT", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableSetupColumn("STAGE_FLAGS", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableSetupColumn("AUTO_BUFFER", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableHeadersRow();

      uint64_t pipeline_descriptor_binding_index = 0;
      uint64_t pipeline_descriptor_binding_count = vector_count(&s_ui_pipeline_descriptor_bindings);

      while (pipeline_descriptor_binding_index < pipeline_descriptor_binding_count) {
        pipeline_descriptor_binding_t *pipeline_descriptor_binding = (pipeline_descriptor_binding_t *)vector_at(&s_ui_pipeline_descriptor_bindings, pipeline_descriptor_binding_index);

        ImGui::TableNextRow();

        ImGui::PushID(pipeline_descriptor_binding);

        ImGui::TableSetColumnIndex(0);
        ImGui::Text("%d", pipeline_descriptor_binding->id);

        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%s", string_buffer(&pipeline_descriptor_binding->binding_name));

        ImGui::TableSetColumnIndex(2);
        ImGui::Text("%d", pipeline_descriptor_binding->binding);

        ImGui::TableSetColumnIndex(3);
        ImGui::Text("%d", pipeline_descriptor_binding->descriptor_type);

        ImGui::TableSetColumnIndex(4);
        ImGui::Text("%d", pipeline_descriptor_binding->descriptor_count);

        ImGui::TableSetColumnIndex(5);
        ImGui::Text("%d", pipeline_descriptor_binding->stage_flags);

        ImGui::TableSetColumnIndex(6);
        ImGui::Text("%d", pipeline_descriptor_binding->auto_buffer);

        ImGui::PopID();

        pipeline_descriptor_binding_index++;
      }

      ImGui::EndTable();
    }

    ImGui::Dummy(ImVec2(0.0f, 10.0F));

    ImGui::TreePop();
  }
}
void ui_pipeline_destroy() {
  if (vector_count(&s_ui_pipeline_assets) > 0) {
    pipeline_asset_t *pipeline_asset = (pipeline_asset_t *)vector_front(&s_ui_pipeline_assets);

    database_destroy_pipeline_vertex_input_bindings(&s_ui_pipeline_vertex_input_bindings);
    database_destroy_pipeline_descriptor_bindings(&s_ui_pipeline_descriptor_bindings);
  }

  database_destroy_pipeline_assets(&s_ui_pipeline_assets);

  s_ui_selected_pipeline_asset = -1;
  s_ui_selected_pipeline_vertex_input_bindings = -1;
  s_ui_selected_pipeline_descriptor_bindings = -1;
}

static void ui_pipeline_select_new_asset(uint64_t selected_index) {
  if (s_ui_selected_pipeline_asset >= 0) {
    pipeline_asset_t *pipeline_asset = (pipeline_asset_t *)vector_front(&s_ui_pipeline_assets);

    database_destroy_pipeline_vertex_input_bindings(&s_ui_pipeline_vertex_input_bindings);
    database_destroy_pipeline_descriptor_bindings(&s_ui_pipeline_descriptor_bindings);
  }

  database_destroy_pipeline_assets(&s_ui_pipeline_assets);

  s_ui_selected_pipeline_asset = selected_index;
  s_ui_selected_pipeline_vertex_input_bindings = -1;
  s_ui_selected_pipeline_descriptor_bindings = -1;

  s_ui_pipeline_assets = database_load_pipeline_assets();

  if (s_ui_selected_pipeline_asset >= 0) {
    pipeline_asset_t *pipeline_asset = (pipeline_asset_t *)vector_at(&s_ui_pipeline_assets, s_ui_selected_pipeline_asset);

    s_ui_pipeline_vertex_input_bindings = database_load_pipeline_vertex_input_bindings_by_id(pipeline_asset->id);
    s_ui_pipeline_descriptor_bindings = database_load_pipeline_descriptor_bindings_by_id(pipeline_asset->id);
  }
}
static void ui_pipeline_draw_insert() {
  static char const *pipeline_types[] = {"Graphic", "Compute"};
  static uint64_t selected_pipline_type_index = 0;

  if (ImGui::BeginCombo("Pipeline Type", pipeline_types[selected_pipline_type_index])) {

    uint64_t pipeline_type_index = 0;
    uint64_t pipeline_type_count = ARRAY_COUNT(pipeline_types);

    while (pipeline_type_index < pipeline_type_count) {
      uint8_t is_selected = (pipeline_type_index == selected_pipline_type_index);

      if (ImGui::Selectable(pipeline_types[pipeline_type_index], is_selected)) {
        selected_pipline_type_index = pipeline_type_index;
      }

      if (is_selected) {
        ImGui::SetItemDefaultFocus();
      }

      pipeline_type_index++;
    }

    ImGui::EndCombo();
  }

  switch (selected_pipline_type_index) {
    case 0: {
      // TODO
      static char pipeline_name[0xFF] = "debug_line";
      static char pipeline_vertex_shader_file_path[0xFF] = "C:\\Users\\mialb\\Downloads\\fuse\\shader\\debug\\line.vert.spv";
      static char pipeline_fragment_shader_file_path[0xFF] = "C:\\Users\\mialb\\Downloads\\fuse\\shader\\debug\\line.frag.spv";

      ImGui::InputText("Pipeline Name", pipeline_name, sizeof(pipeline_name));
      ImGui::InputText("Vertex File", pipeline_vertex_shader_file_path, sizeof(pipeline_vertex_shader_file_path));
      ImGui::InputText("Fragment File", pipeline_fragment_shader_file_path, sizeof(pipeline_fragment_shader_file_path));

      if (ImGui::Button("Create Pipeline")) {
        importer_import_graphic_pipeline(pipeline_name, pipeline_vertex_shader_file_path, pipeline_fragment_shader_file_path);

        ui_pipeline_refresh();
      }

      break;
    }
    case 1: {
      static char pipeline_name[0xFF] = "";
      static char pipeline_compute_shader_file_path[0xFF] = "";

      ImGui::InputText("Pipeline Name", pipeline_name, sizeof(pipeline_name));
      ImGui::InputText("Compute File", pipeline_compute_shader_file_path, sizeof(pipeline_compute_shader_file_path));

      if (ImGui::Button("Create Pipeline")) {
        importer_import_compute_pipeline(pipeline_name, pipeline_compute_shader_file_path);

        ui_pipeline_refresh();
      }

      break;
    }
  }
}

// TODO
/*
static int editingRow = -1;
static int editingCol = -1;
static char editBuffer[128]; // temp buffer for editing

if (ImGui::BeginTable("MyTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
  for (int row = 0; row < rowsCount; ++row) {
    ImGui::TableNextRow();
    for (int col = 0; col < 3; ++col) {
      ImGui::TableSetColumnIndex(col);

      // Unique ID for the cell
      std::string cellId = "cell_" + std::to_string(row) + "_" + std::to_string(col);

      bool isEditing = (editingRow == row && editingCol == col);

      if (isEditing) {
        ImGui::PushID(cellId.c_str());
        if (ImGui::InputText("##edit", editBuffer, IM_ARRAYSIZE(editBuffer),
                             ImGuiInputTextFlags_EnterReturnsTrue |
                               ImGuiInputTextFlags_AutoSelectAll)) {
          // Save edited text to your data model
          tableData[row][col] = editBuffer;
          editingRow = editingCol = -1; // exit edit mode
        }
        // Lose focus = cancel edit
        if (!ImGui::IsItemActive() && !ImGui::IsItemFocused()) {
          editingRow = editingCol = -1;
        }
        ImGui::PopID();
      } else {
        // Draw label that can be clicked to start editing
        if (ImGui::Selectable(tableData[row][col].c_str(), false,
                              ImGuiSelectableFlags_AllowDoubleClick |
                                ImGuiSelectableFlags_SpanAllColumns)) {
          editingRow = row;
          editingCol = col;
          // Copy current text to buffer
          strncpy(editBuffer, tableData[row][col].c_str(), sizeof(editBuffer));
          editBuffer[sizeof(editBuffer) - 1] = '\0';
        }
      }
    }
  }
  ImGui::EndTable();
}
*/
