#include <library/core/co_api.h>
#include <library/database/db_api.h>

#include <ui/ui_pipelines.h>

#include <imgui/imgui.h>

static void ui_pipeline_refresh();
static void ui_pipeline_draw_insert();

static uint8_t s_ui_pipeline_allocated = 0;

static vector_t s_ui_pipeline_configs = {};
static vector_t s_ui_pipeline_vertex_input_binding_descriptions = {};
static vector_t s_ui_pipeline_vertex_input_attribute_descriptions = {};
static vector_t s_ui_pipeline_descriptor_pool_sizes = {};
static vector_t s_ui_pipeline_descriptor_set_layout_bindings = {};

static int64_t s_ui_selected_pipeline_config = -1;
static int64_t s_ui_selected_pipeline_vertex_input_binding_description = -1;
static int64_t s_ui_selected_pipeline_vertex_input_attribute_description = -1;
static int64_t s_ui_selected_pipeline_descriptor_pool_size = -1;
static int64_t s_ui_selected_pipeline_descriptor_set_layout_bindings = -1;

void ui_pipelines_create() {
  ui_pipeline_refresh();
}
void ui_pipelines_draw() {
  ImGui::Begin("Pipelines");

  if (ImGui::TreeNodeEx("Pipelines", ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding)) {
    ui_pipeline_draw_insert();

    ImGui::Dummy(ImVec2(0.0f, 20.0F));

    static ImGuiTableFlags pipeline_table_flags =
      ImGuiTableFlags_Borders |
      ImGuiTableFlags_RowBg |
      ImGuiTableFlags_Resizable |
      ImGuiTableFlags_SizingStretchProp;

    if (ImGui::BeginTable("##PipelineConfigTable", 4, pipeline_table_flags)) {
      ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthStretch, 0.1F);
      ImGui::TableSetupColumn("TYPE", ImGuiTableColumnFlags_WidthStretch, 0.1F);
      ImGui::TableSetupColumn("NAME", ImGuiTableColumnFlags_WidthStretch, 0.6F);
      ImGui::TableSetupColumn("AUTO_CREATE", ImGuiTableColumnFlags_WidthStretch, 0.2F);
      ImGui::TableHeadersRow();

      uint64_t pipeline_config_index = 0;
      uint64_t pipeline_config_count = vector_count(&s_ui_pipeline_configs);

      while (pipeline_config_index < pipeline_config_count) {
        pipeline_config_t *pipeline_config = (pipeline_config_t *)vector_at(&s_ui_pipeline_configs, pipeline_config_index);

        ImGui::TableNextRow();

        ImGui::PushID(pipeline_config);

        ImGui::TableSetColumnIndex(0);
        ImGui::Text("%d", pipeline_config->id);

        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%d", pipeline_config->type);

        ImGui::TableSetColumnIndex(2);
        if (ImGui::Selectable(string_buffer(&pipeline_config->name), (pipeline_config_index == s_ui_selected_pipeline_config), ImGuiSelectableFlags_SpanAllColumns)) {
          s_ui_selected_pipeline_config = pipeline_config_index;
          s_ui_selected_pipeline_vertex_input_binding_description = -1;
          s_ui_selected_pipeline_vertex_input_attribute_description = -1;
          s_ui_selected_pipeline_descriptor_pool_size = -1;
          s_ui_selected_pipeline_descriptor_set_layout_bindings = -1;

          db_destroy_vertex_input_binding_descriptions(&s_ui_pipeline_vertex_input_binding_descriptions);
          db_destroy_vertex_input_attribute_descriptions(&s_ui_pipeline_vertex_input_attribute_descriptions);
          db_destroy_descriptor_pool_sizes(&s_ui_pipeline_descriptor_pool_sizes);
          db_destroy_descriptor_set_layout_bindings(&s_ui_pipeline_descriptor_set_layout_bindings);

          s_ui_pipeline_vertex_input_binding_descriptions = db_load_pipeline_vertex_input_binding_descriptions_by_id(pipeline_config->id);
          s_ui_pipeline_vertex_input_attribute_descriptions = db_load_pipeline_vertex_input_attribute_descriptions_by_id(pipeline_config->id);
          s_ui_pipeline_descriptor_pool_sizes = db_load_pipeline_descriptor_pool_sizes_by_id(pipeline_config->id);
          s_ui_pipeline_descriptor_set_layout_bindings = db_load_pipeline_descriptor_set_layout_bindings_by_id(pipeline_config->id);
        }

        ImGui::TableSetColumnIndex(3);
        ImGui::Text("%d", pipeline_config->auto_create);

        ImGui::PopID();

        pipeline_config_index++;
      }

      ImGui::EndTable();
    }

    ImGui::Dummy(ImVec2(0.0f, 20.0F));

    static ImGuiTableFlags pipeline_vertex_input_binding_description_table_flags =
      ImGuiTableFlags_Borders |
      ImGuiTableFlags_RowBg |
      ImGuiTableFlags_Resizable |
      ImGuiTableFlags_SizingStretchProp;

    if (ImGui::BeginTable("##PipelineVertexInputBindingDescriptionTable", 5, pipeline_vertex_input_binding_description_table_flags)) {
      ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableSetupColumn("BINDING_NAME", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableSetupColumn("BINDING", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableSetupColumn("STRIDE", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableSetupColumn("INPUT_RATE", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableHeadersRow();

      uint64_t pipeline_vertex_input_binding_description_index = 0;
      uint64_t pipeline_vertex_input_binding_description_count = vector_count(&s_ui_pipeline_vertex_input_binding_descriptions);

      while (pipeline_vertex_input_binding_description_index < pipeline_vertex_input_binding_description_count) {
        pipeline_vertex_input_binding_description_t *pipeline_vertex_input_binding_description = (pipeline_vertex_input_binding_description_t *)vector_at(&s_ui_pipeline_vertex_input_binding_descriptions, pipeline_vertex_input_binding_description_index);

        ImGui::TableNextRow();

        ImGui::PushID(pipeline_vertex_input_binding_description);

        ImGui::TableSetColumnIndex(0);
        ImGui::Text("%d", pipeline_vertex_input_binding_description->id);

        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%s", string_buffer(&pipeline_vertex_input_binding_description->binding_name));

        ImGui::TableSetColumnIndex(2);
        ImGui::Text("%d", pipeline_vertex_input_binding_description->binding);

        ImGui::TableSetColumnIndex(3);
        ImGui::Text("%d", pipeline_vertex_input_binding_description->stride);

        ImGui::TableSetColumnIndex(4);
        ImGui::Text("%d", pipeline_vertex_input_binding_description->input_rate);

        ImGui::PopID();

        pipeline_vertex_input_binding_description_index++;
      }

      ImGui::EndTable();
    }

    ImGui::Dummy(ImVec2(0.0f, 20.0F));

    static ImGuiTableFlags pipeline_vertex_input_attribute_description_table_flags =
      ImGuiTableFlags_Borders |
      ImGuiTableFlags_RowBg |
      ImGuiTableFlags_Resizable |
      ImGuiTableFlags_SizingStretchProp;

    if (ImGui::BeginTable("##PipelineVertexInputAttributeDescriptionTable", 6, pipeline_vertex_input_attribute_description_table_flags)) {
      ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableSetupColumn("ATTRIBUTE_NAME", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableSetupColumn("LOCATION", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableSetupColumn("BINDING", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableSetupColumn("FORMAT", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableSetupColumn("OFFSET", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableHeadersRow();

      uint64_t pipeline_vertex_input_attribute_description_index = 0;
      uint64_t pipeline_vertex_input_attribute_description_count = vector_count(&s_ui_pipeline_vertex_input_attribute_descriptions);

      while (pipeline_vertex_input_attribute_description_index < pipeline_vertex_input_attribute_description_count) {
        pipeline_vertex_input_attribute_description_t *pipeline_vertex_input_attribute_description = (pipeline_vertex_input_attribute_description_t *)vector_at(&s_ui_pipeline_vertex_input_attribute_descriptions, pipeline_vertex_input_attribute_description_index);

        ImGui::TableNextRow();

        ImGui::PushID(pipeline_vertex_input_attribute_description);

        ImGui::TableSetColumnIndex(0);
        ImGui::Text("%d", pipeline_vertex_input_attribute_description->id);

        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%s", string_buffer(&pipeline_vertex_input_attribute_description->attribute_name));

        ImGui::TableSetColumnIndex(2);
        ImGui::Text("%d", pipeline_vertex_input_attribute_description->location);

        ImGui::TableSetColumnIndex(3);
        ImGui::Text("%d", pipeline_vertex_input_attribute_description->binding);

        ImGui::TableSetColumnIndex(4);
        ImGui::Text("%d", pipeline_vertex_input_attribute_description->format);

        ImGui::TableSetColumnIndex(5);
        ImGui::Text("%d", pipeline_vertex_input_attribute_description->offset);

        ImGui::PopID();

        pipeline_vertex_input_attribute_description_index++;
      }

      ImGui::EndTable();
    }

    ImGui::Dummy(ImVec2(0.0f, 20.0F));

    static ImGuiTableFlags pipeline_descriptor_pool_size_table_flags =
      ImGuiTableFlags_Borders |
      ImGuiTableFlags_RowBg |
      ImGuiTableFlags_Resizable |
      ImGuiTableFlags_SizingStretchProp;

    if (ImGui::BeginTable("##PipelineDescriptorPoolSizeTable", 4, pipeline_descriptor_pool_size_table_flags)) {
      ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableSetupColumn("PIPELINE_ID", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableSetupColumn("TYPE", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableSetupColumn("COUNT", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableHeadersRow();

      uint64_t pipeline_descriptor_pool_size_index = 0;
      uint64_t pipeline_descriptor_pool_size_count = vector_count(&s_ui_pipeline_descriptor_pool_sizes);

      while (pipeline_descriptor_pool_size_index < pipeline_descriptor_pool_size_count) {
        pipeline_descriptor_pool_size_t *pipeline_descriptor_pool_size = (pipeline_descriptor_pool_size_t *)vector_at(&s_ui_pipeline_descriptor_pool_sizes, pipeline_descriptor_pool_size_index);

        ImGui::TableNextRow();

        ImGui::PushID(pipeline_descriptor_pool_size);

        ImGui::TableSetColumnIndex(0);
        ImGui::Text("%d", pipeline_descriptor_pool_size->id);

        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%d", pipeline_descriptor_pool_size->pipeline_id);

        ImGui::TableSetColumnIndex(2);
        ImGui::Text("%d", pipeline_descriptor_pool_size->type);

        ImGui::TableSetColumnIndex(3);
        ImGui::Text("%d", pipeline_descriptor_pool_size->count);

        ImGui::PopID();

        pipeline_descriptor_pool_size_index++;
      }

      ImGui::EndTable();
    }

    ImGui::Dummy(ImVec2(0.0f, 20.0F));

    static ImGuiTableFlags pipeline_descriptor_set_layout_binding_table_flags =
      ImGuiTableFlags_Borders |
      ImGuiTableFlags_RowBg |
      ImGuiTableFlags_Resizable |
      ImGuiTableFlags_SizingStretchProp;

    if (ImGui::BeginTable("##PipelineDescriptorSetLayoutBindingTable", 7, pipeline_descriptor_set_layout_binding_table_flags)) {
      ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableSetupColumn("BINDING_NAME", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableSetupColumn("BINDING", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableSetupColumn("DESCRIPTOR_TYPE", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableSetupColumn("DESCRIPTOR_COUNT", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableSetupColumn("STAGE_FLAGS", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableSetupColumn("AUTO_BUFFER", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableHeadersRow();

      uint64_t pipeline_descriptor_set_layout_binding_index = 0;
      uint64_t pipeline_descriptor_set_layout_binding_count = vector_count(&s_ui_pipeline_descriptor_set_layout_bindings);

      while (pipeline_descriptor_set_layout_binding_index < pipeline_descriptor_set_layout_binding_count) {
        pipeline_descriptor_set_layout_binding_t *pipeline_descriptor_set_layout_binding = (pipeline_descriptor_set_layout_binding_t *)vector_at(&s_ui_pipeline_descriptor_set_layout_bindings, pipeline_descriptor_set_layout_binding_index);

        ImGui::TableNextRow();

        ImGui::PushID(pipeline_descriptor_set_layout_binding);

        ImGui::TableSetColumnIndex(0);
        ImGui::Text("%d", pipeline_descriptor_set_layout_binding->id);

        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%s", string_buffer(&pipeline_descriptor_set_layout_binding->binding_name));

        ImGui::TableSetColumnIndex(2);
        ImGui::Text("%d", pipeline_descriptor_set_layout_binding->binding);

        ImGui::TableSetColumnIndex(3);
        ImGui::Text("%d", pipeline_descriptor_set_layout_binding->descriptor_type);

        ImGui::TableSetColumnIndex(4);
        ImGui::Text("%d", pipeline_descriptor_set_layout_binding->descriptor_count);

        ImGui::TableSetColumnIndex(5);
        ImGui::Text("%d", pipeline_descriptor_set_layout_binding->stage_flags);

        ImGui::TableSetColumnIndex(6);
        ImGui::Text("%d", pipeline_descriptor_set_layout_binding->auto_buffer);

        ImGui::PopID();

        pipeline_descriptor_set_layout_binding_index++;
      }

      ImGui::EndTable();
    }

    ImGui::TreePop();
  }

  ImGui::End();
}
void ui_pipelines_destroy() {
  if (s_ui_pipeline_allocated) {
    s_ui_pipeline_allocated = 0;

    db_destroy_pipeline_configs(&s_ui_pipeline_configs);
  }

  s_ui_selected_pipeline_config = -1;
  s_ui_selected_pipeline_vertex_input_binding_description = -1;
  s_ui_selected_pipeline_vertex_input_attribute_description = -1;
  s_ui_selected_pipeline_descriptor_pool_size = -1;
  s_ui_selected_pipeline_descriptor_set_layout_bindings = -1;
}

static void ui_pipeline_refresh() {
  if (s_ui_pipeline_allocated) {
    db_destroy_pipeline_configs(&s_ui_pipeline_configs);
  }

  s_ui_pipeline_configs = db_load_pipeline_configs();

  if (vector_count(&s_ui_pipeline_configs) > 0) {
    pipeline_config_t *pipeline = (pipeline_config_t *)vector_front(&s_ui_pipeline_configs);

    s_ui_pipeline_allocated = 1;

    s_ui_selected_pipeline_config = 0;

    s_ui_pipeline_vertex_input_binding_descriptions = db_load_pipeline_vertex_input_binding_descriptions_by_id(pipeline->id);
    s_ui_pipeline_vertex_input_attribute_descriptions = db_load_pipeline_vertex_input_attribute_descriptions_by_id(pipeline->id);
    s_ui_pipeline_descriptor_pool_sizes = db_load_pipeline_descriptor_pool_sizes_by_id(pipeline->id);
    s_ui_pipeline_descriptor_set_layout_bindings = db_load_pipeline_descriptor_set_layout_bindings_by_id(pipeline->id);
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
      static char pipeline_vertex_shader[0xFF] = "C:\\Users\\mialb\\Downloads\\fuse\\shader\\debug\\line.vert.spv";
      static char pipeline_fragment_shader[0xFF] = "C:\\Users\\mialb\\Downloads\\fuse\\shader\\debug\\line.frag.spv";

      ImGui::InputText("Pipeline Name", pipeline_name, sizeof(pipeline_name));
      ImGui::InputText("Vertex Shader", pipeline_vertex_shader, sizeof(pipeline_vertex_shader));
      ImGui::InputText("Fragment Shader", pipeline_fragment_shader, sizeof(pipeline_fragment_shader));

      if (ImGui::Button("Create Pipeline")) {
        db_importer_insert_graphic_pipeline(pipeline_name, pipeline_vertex_shader, pipeline_fragment_shader);

        ui_pipeline_refresh();
      }

      break;
    }
    case 1: {
      static char pipeline_name[0xFF] = "";
      static char pipeline_compute_shader[0xFF] = "";

      ImGui::InputText("Pipeline Name", pipeline_name, sizeof(pipeline_name));
      ImGui::InputText("Compute Shader", pipeline_compute_shader, sizeof(pipeline_compute_shader));

      if (ImGui::Button("Create Pipeline")) {
        db_importer_insert_compute_pipeline(pipeline_name, pipeline_compute_shader);

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
