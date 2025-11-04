#include <editor/ed_pch.h>
#include <editor/ed_detail.h>
#include <editor/ed_catalog.h>
#include <editor/ed_dockspace.h>
#include <editor/ed_titlebar.h>

static void detail_draw_swapchain(void);
static void detail_draw_renderer(void);
static void detail_draw_pipeline(void);
static void detail_draw_model(void);

static void detail_select_model_mesh(uint64_t selected_index);
static void detail_select_mesh_primitive(uint64_t selected_index);

int64_t g_detail_selected_model_mesh = -1;
int64_t g_detail_selected_mesh_primitive = -1;

vector_t g_detail_mesh_primitives = {};
vector_t g_detail_mesh_attributes = {};

void detail_create(context_t *context) {
}
void detail_refresh(context_t *context) {
}
void detail_draw(context_t *context) {
  bool is_docked = false;

  if (dockspace_begin_child("Detail", &g_titlebar_detail_open, &is_docked)) {
    switch (g_catalog_selected_asset_type) {
      case ASSET_TYPE_NONE: {

        break;
      }
      case ASSET_TYPE_SWAPCHAIN: {

        if (g_catalog_selected_swapchain_asset != -1) {
          detail_draw_swapchain();
        }

        break;
      }
      case ASSET_TYPE_RENDERER: {

        if (g_catalog_selected_renderer_asset != -1) {
          detail_draw_renderer();
        }

        break;
      }
      case ASSET_TYPE_PIPELINE: {

        if (g_catalog_selected_pipeline_asset != -1) {
          detail_draw_pipeline();
        }

        break;
      }
      case ASSET_TYPE_MODEL: {

        if (g_catalog_selected_model_asset != -1) {
          detail_draw_model();
        }

        break;
      }
    }

    dockspace_end_child(is_docked);
  }
}
void detail_destroy(context_t *context) {
  if (g_detail_selected_model_mesh != -1) {
    database_destroy_mesh_primitives(&g_detail_mesh_primitives);
  }

  if (g_detail_selected_mesh_primitive != -1) {
    database_destroy_mesh_attributes(&g_detail_mesh_attributes);
  }

  g_detail_selected_model_mesh = -1;
  g_detail_selected_mesh_primitive = -1;
}

static void detail_draw_swapchain(void) {
  swapchain_asset_t *swapchain_asset = (swapchain_asset_t *)vector_at(&g_catalog_swapchain_assets, g_catalog_selected_swapchain_asset);

  if (ImGui::Button("Save")) {
    database_store_swapchain_asset(swapchain_asset);
  }

  ImGui::Dummy(ImVec2(0.0F, 5.0F));
  ImGui::Separator();
  ImGui::Dummy(ImVec2(0.0F, 5.0F));

  ImGui::Text(swapchain_asset->name);
  ImGui::InputInt("Image Count", (int32_t *)&swapchain_asset->image_count);
  ImGui::InputInt("Depth Format", (int32_t *)&swapchain_asset->depth_format);
  ImGui::Checkbox("Is Default", (bool *)&swapchain_asset->is_default);
}
static void detail_draw_renderer(void) {
  renderer_asset_t *renderer_asset = (renderer_asset_t *)vector_at(&g_catalog_renderer_assets, g_catalog_selected_renderer_asset);

  if (ImGui::Button("Save")) {
    database_store_renderer_asset(renderer_asset);
  }

  ImGui::Dummy(ImVec2(0.0F, 5.0F));
  ImGui::Separator();
  ImGui::Dummy(ImVec2(0.0F, 5.0F));

  ImGui::Text(renderer_asset->name);
  ImGui::InputInt("Frames In Flight", (int32_t *)&renderer_asset->frames_in_flight);
  ImGui::Checkbox("Is Default", (bool *)&renderer_asset->is_default);
}
static void detail_draw_pipeline(void) {
  pipeline_asset_t *pipeline_asset = (pipeline_asset_t *)vector_at(&g_catalog_pipeline_assets, g_catalog_selected_pipeline_asset);

  if (ImGui::Button("Save")) {
    database_store_pipeline_asset(pipeline_asset);

    uint64_t pipeline_vertex_input_binding_index = 0;
    uint64_t pipeline_vertex_input_binding_count = vector_count(&g_catalog_pipeline_vertex_input_bindings);

    while (pipeline_vertex_input_binding_index < pipeline_vertex_input_binding_count) {

      pipeline_vertex_input_binding_t *pipeline_vertex_input_binding = (pipeline_vertex_input_binding_t *)vector_at(&g_catalog_pipeline_vertex_input_bindings, pipeline_vertex_input_binding_index);

      database_store_pipeline_vertex_input_binding(pipeline_vertex_input_binding);

      pipeline_vertex_input_binding_index++;
    }

    uint64_t pipeline_descriptor_binding_index = 0;
    uint64_t pipeline_descriptor_binding_count = vector_count(&g_catalog_pipeline_descriptor_bindings);

    while (pipeline_descriptor_binding_index < pipeline_descriptor_binding_count) {

      pipeline_descriptor_binding_t *pipeline_descriptor_binding = (pipeline_descriptor_binding_t *)vector_at(&g_catalog_pipeline_descriptor_bindings, pipeline_descriptor_binding_index);

      database_store_pipeline_descriptor_binding(pipeline_descriptor_binding);

      pipeline_descriptor_binding_index++;
    }
  }

  ImGui::Dummy(ImVec2(0.0F, 5.0F));
  ImGui::Separator();
  ImGui::Dummy(ImVec2(0.0F, 5.0F));

  ImGui::Text(pipeline_asset->name);
  ImGui::InputInt("Link Index", (int32_t *)&pipeline_asset->link_index);
  ImGui::Checkbox("Auto Create Pipeline", (bool *)&pipeline_asset->auto_create_pipeline);
  ImGui::Checkbox("Auto Create Vertex Input Buffer", (bool *)&pipeline_asset->auto_create_vertex_input_buffer);
  ImGui::Checkbox("Auto Link Descriptor Bindings", (bool *)&pipeline_asset->auto_link_descriptor_bindings);
  ImGui::Checkbox("Interleaved Vertex Input Buffer", (bool *)&pipeline_asset->interleaved_vertex_input_buffer);

  ImGui::Dummy(ImVec2(0.0F, 5.0F));
  ImGui::Separator();
  ImGui::Dummy(ImVec2(0.0F, 5.0F));

  ImGui::Text("Vertex Input Bindings");

  uint64_t pipeline_vertex_input_binding_index = 0;
  uint64_t pipeline_vertex_input_binding_count = vector_count(&g_catalog_pipeline_vertex_input_bindings);

  while (pipeline_vertex_input_binding_index < pipeline_vertex_input_binding_count) {

    pipeline_vertex_input_binding_t *pipeline_vertex_input_binding = (pipeline_vertex_input_binding_t *)vector_at(&g_catalog_pipeline_vertex_input_bindings, pipeline_vertex_input_binding_index);

    if (ImGui::TreeNodeEx(pipeline_vertex_input_binding->binding_name, ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_FramePadding)) {

      ImGui::BeginTable("##VertexInputBindingTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchProp);

      ImGui::TableSetupColumn("NAME", ImGuiTableColumnFlags_WidthStretch, 0.5F);
      ImGui::TableSetupColumn("VALUE", ImGuiTableColumnFlags_WidthStretch, 0.5F);

      ImGui::TableHeadersRow();

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Location");
      ImGui::TableSetColumnIndex(1);
      ImGui::Text("%u", pipeline_vertex_input_binding->location);

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Size");
      ImGui::TableSetColumnIndex(1);
      ImGui::Text("%u", pipeline_vertex_input_binding->size);

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Component Count");
      ImGui::TableSetColumnIndex(1);
      ImGui::Text("%u", pipeline_vertex_input_binding->component_count);

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Format");
      ImGui::TableSetColumnIndex(1);
      ImGui::Text("%u", pipeline_vertex_input_binding->format);

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Input Rate");
      ImGui::TableSetColumnIndex(1);
      ImGui::Text("%u", pipeline_vertex_input_binding->input_rate);

      ImGui::EndTable();

      ImGui::TreePop();
    }

    pipeline_vertex_input_binding_index++;
  }

  ImGui::Dummy(ImVec2(0.0F, 5.0F));
  ImGui::Separator();
  ImGui::Dummy(ImVec2(0.0F, 5.0F));

  ImGui::Text("Descriptor Bindings");

  uint64_t pipeline_descriptor_binding_index = 0;
  uint64_t pipeline_descriptor_binding_count = vector_count(&g_catalog_pipeline_descriptor_bindings);

  while (pipeline_descriptor_binding_index < pipeline_descriptor_binding_count) {

    pipeline_descriptor_binding_t *pipeline_descriptor_binding = (pipeline_descriptor_binding_t *)vector_at(&g_catalog_pipeline_descriptor_bindings, pipeline_descriptor_binding_index);

    if (ImGui::TreeNodeEx(pipeline_descriptor_binding->binding_name, ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_FramePadding)) {

      ImGui::BeginTable("##DescriptorBindingTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchProp);

      ImGui::TableSetupColumn("NAME", ImGuiTableColumnFlags_WidthStretch, 0.5F);
      ImGui::TableSetupColumn("VALUE", ImGuiTableColumnFlags_WidthStretch, 0.5F);

      ImGui::TableHeadersRow();

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Binding");
      ImGui::TableSetColumnIndex(1);
      ImGui::Text("%u", pipeline_descriptor_binding->binding);

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Descriptor Type");
      ImGui::TableSetColumnIndex(1);
      ImGui::Text("%u", pipeline_descriptor_binding->descriptor_type);

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Descriptor Count");
      ImGui::TableSetColumnIndex(1);
      ImGui::Text("%u", pipeline_descriptor_binding->descriptor_count);

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Stage Flags");
      ImGui::TableSetColumnIndex(1);
      ImGui::Text("%u", pipeline_descriptor_binding->stage_flags);

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Auto Buffer");
      ImGui::TableSetColumnIndex(1);
      ImGui::Checkbox("##", (bool *)&pipeline_descriptor_binding->auto_buffer);

      ImGui::EndTable();

      ImGui::TreePop();
    }

    pipeline_descriptor_binding_index++;
  }
}
static void detail_draw_model(void) {
  model_asset_t *model_asset = (model_asset_t *)vector_at(&g_catalog_model_assets, g_catalog_selected_model_asset);

  if (ImGui::Button("Save")) {
    database_store_model_asset(model_asset);
  }

  ImGui::Dummy(ImVec2(0.0F, 5.0F));
  ImGui::Separator();
  ImGui::Dummy(ImVec2(0.0F, 5.0F));

  ImGui::Text(model_asset->name);

  ImGui::Dummy(ImVec2(0.0F, 5.0F));
  ImGui::Separator();
  ImGui::Dummy(ImVec2(0.0F, 5.0F));

  ImGui::Text("Meshes");

  ImGui::BeginTable("##ModelMeshTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchProp);

  ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed);
  ImGui::TableSetupColumn("NAME", ImGuiTableColumnFlags_WidthStretch);

  ImGui::TableHeadersRow();

  uint64_t model_mesh_index = 0;
  uint64_t model_mesh_count = vector_count(&g_catalog_model_meshes);

  while (model_mesh_index < model_mesh_count) {

    model_mesh_t *model_mesh = (model_mesh_t *)vector_at(&g_catalog_model_meshes, model_mesh_index);

    ImGui::TableNextRow();

    ImGui::TableSetColumnIndex(0);
    ImGui::Text("%d", model_mesh->id);

    ImGui::TableSetColumnIndex(1);
    if (ImGui::Selectable(model_mesh->name, (model_mesh_index == g_detail_selected_model_mesh), ImGuiSelectableFlags_SpanAllColumns)) {
      detail_select_model_mesh(model_mesh_index);
    }

    model_mesh_index++;
  }

  ImGui::EndTable();

  ImGui::Text("Primitives");

  ImGui::Dummy(ImVec2(0.0F, 5.0F));
  ImGui::Separator();
  ImGui::Dummy(ImVec2(0.0F, 5.0F));

  ImGui::BeginTable("##MeshPrimitiveTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchProp);

  ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed);
  ImGui::TableSetupColumn("NAME", ImGuiTableColumnFlags_WidthStretch);

  ImGui::TableHeadersRow();

  uint64_t mesh_primitive_index = 0;
  uint64_t mesh_primitive_count = vector_count(&g_detail_mesh_primitives);

  while (mesh_primitive_index < mesh_primitive_count) {

    mesh_primitive_t *mesh_primitive = (mesh_primitive_t *)vector_at(&g_detail_mesh_primitives, mesh_primitive_index);

    ImGui::TableNextRow();

    ImGui::TableSetColumnIndex(0);
    ImGui::Text("%d", mesh_primitive->id);

    ImGui::TableSetColumnIndex(1);
    if (ImGui::Selectable(mesh_primitive->name, (mesh_primitive_index == g_detail_selected_mesh_primitive), ImGuiSelectableFlags_SpanAllColumns)) {
      detail_select_mesh_primitive(mesh_primitive_index);
    }

    mesh_primitive_index++;
  }

  ImGui::EndTable();

  ImGui::Text("Attributes");

  ImGui::Dummy(ImVec2(0.0F, 5.0F));
  ImGui::Separator();
  ImGui::Dummy(ImVec2(0.0F, 5.0F));

  uint64_t mesh_attribute_index = 0;
  uint64_t mesh_attribute_count = vector_count(&g_detail_mesh_attributes);

  while (mesh_attribute_index < mesh_attribute_count) {

    mesh_attribute_t *mesh_attribute = (mesh_attribute_t *)vector_at(&g_detail_mesh_attributes, mesh_attribute_index);

    if (ImGui::TreeNodeEx(mesh_attribute->name, ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_FramePadding)) {

      ImGui::BeginTable("##MeshAttributeTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchProp);

      ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableSetupColumn("NAME", ImGuiTableColumnFlags_WidthStretch);

      ImGui::TableHeadersRow();

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Type");
      ImGui::TableSetColumnIndex(1);
      ImGui::Text("%d", mesh_attribute->type);

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Count");
      ImGui::TableSetColumnIndex(1);
      ImGui::Text("%d", mesh_attribute->count);

      ImGui::EndTable();

      ImGui::TreePop();
    }

    mesh_attribute_index++;
  }
}

static void detail_select_model_mesh(uint64_t selected_index) {
  if (g_detail_selected_model_mesh != -1) {
    database_destroy_mesh_primitives(&g_detail_mesh_primitives);
  }

  g_detail_selected_model_mesh = selected_index;

  model_mesh_t *model_mesh = (model_mesh_t *)vector_at(&g_catalog_model_meshes, g_detail_selected_model_mesh);

  g_detail_mesh_primitives = database_load_mesh_primitives_by_id(model_mesh->id);
}
static void detail_select_mesh_primitive(uint64_t selected_index) {
  if (g_detail_selected_mesh_primitive != -1) {
    database_destroy_mesh_attributes(&g_detail_mesh_attributes);
  }

  g_detail_selected_mesh_primitive = selected_index;

  mesh_primitive_t *mesh_primitive = (mesh_primitive_t *)vector_at(&g_detail_mesh_primitives, g_detail_selected_mesh_primitive);

  g_detail_mesh_attributes = database_load_mesh_attributes_by_id(mesh_primitive->id);
}
