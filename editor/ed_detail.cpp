#include <editor/ed_pch.h>
#include <editor/ed_detail.h>
#include <editor/ed_catalog.h>

static void detail_draw_swapchain(void);
static void detail_draw_renderer(void);
static void detail_draw_pipeline(void);
static void detail_draw_model(void);

void detail_create() {
}
void detail_refresh() {
}
void detail_draw() {
  ImGui::Begin("Detail");

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

  ImGui::End();
}
void detail_destroy() {
}

static void detail_draw_swapchain(void) {
  swapchain_asset_t *swapchain_asset = (swapchain_asset_t *)vector_at(&g_catalog_swapchain_assets, g_catalog_selected_swapchain_asset);

  ImGui::Dummy(ImVec2(0.0f, 5.0F));

  if (ImGui::Button("Save")) {
    database_store_swapchain_asset(swapchain_asset);
  }

  ImGui::Dummy(ImVec2(0.0f, 5.0F));

  ImGui::Text(swapchain_asset->name);
  ImGui::InputInt("Image Count", (int32_t *)&swapchain_asset->image_count);
  ImGui::InputInt("Depth Format", (int32_t *)&swapchain_asset->depth_format);
  ImGui::Checkbox("Is Default", (bool *)&swapchain_asset->is_default);

  ImGui::Dummy(ImVec2(0.0f, 5.0F));
}
static void detail_draw_renderer(void) {
  renderer_asset_t *renderer_asset = (renderer_asset_t *)vector_at(&g_catalog_renderer_assets, g_catalog_selected_renderer_asset);

  ImGui::Dummy(ImVec2(0.0f, 5.0F));

  if (ImGui::Button("Save")) {
    database_store_renderer_asset(renderer_asset);
  }

  ImGui::Dummy(ImVec2(0.0f, 5.0F));

  ImGui::Text(renderer_asset->name);
  ImGui::InputInt("Frames In Flight", (int32_t *)&renderer_asset->frames_in_flight);
  ImGui::Checkbox("Is Default", (bool *)&renderer_asset->is_default);

  ImGui::Dummy(ImVec2(0.0f, 5.0F));
}
static void detail_draw_pipeline(void) {
  pipeline_asset_t *pipeline_asset = (pipeline_asset_t *)vector_at(&g_catalog_pipeline_assets, g_catalog_selected_pipeline_asset);

  ImGui::Dummy(ImVec2(0.0f, 5.0F));

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

  ImGui::Dummy(ImVec2(0.0f, 5.0F));

  ImGui::Text(pipeline_asset->name);
  ImGui::InputInt("Link Index", (int32_t *)&pipeline_asset->link_index);
  ImGui::Checkbox("Auto Create Pipeline", (bool *)&pipeline_asset->auto_create_pipeline);
  ImGui::Checkbox("Auto Create Vertex Input Buffer", (bool *)&pipeline_asset->auto_create_vertex_input_buffer);
  ImGui::Checkbox("Auto Link Descriptor Bindings", (bool *)&pipeline_asset->auto_link_descriptor_bindings);
  ImGui::Checkbox("Interleaved Vertex Input Buffer", (bool *)&pipeline_asset->interleaved_vertex_input_buffer);

  ImGui::Dummy(ImVec2(0.0f, 5.0F));

  ImGui::Text("Vertex Input Bindings");

  ImGui::Dummy(ImVec2(0.0f, 5.0F));

  uint64_t pipeline_vertex_input_binding_index = 0;
  uint64_t pipeline_vertex_input_binding_count = vector_count(&g_catalog_pipeline_vertex_input_bindings);

  while (pipeline_vertex_input_binding_index < pipeline_vertex_input_binding_count) {

    pipeline_vertex_input_binding_t *pipeline_vertex_input_binding = (pipeline_vertex_input_binding_t *)vector_at(&g_catalog_pipeline_vertex_input_bindings, pipeline_vertex_input_binding_index);

    ImGui::PushID(pipeline_vertex_input_binding);

    if (ImGui::TreeNodeEx(pipeline_vertex_input_binding->binding_name, ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding)) {

      ImGui::Dummy(ImVec2(0.0f, 5.0F));

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

      ImGui::Dummy(ImVec2(0.0f, 5.0F));

      ImGui::TreePop();
    }

    ImGui::PopID();

    pipeline_vertex_input_binding_index++;
  }

  ImGui::Dummy(ImVec2(0.0f, 5.0F));

  ImGui::Text("Descriptor Bindings");

  ImGui::Dummy(ImVec2(0.0f, 5.0F));

  uint64_t pipeline_descriptor_binding_index = 0;
  uint64_t pipeline_descriptor_binding_count = vector_count(&g_catalog_pipeline_descriptor_bindings);

  while (pipeline_descriptor_binding_index < pipeline_descriptor_binding_count) {

    pipeline_descriptor_binding_t *pipeline_descriptor_binding = (pipeline_descriptor_binding_t *)vector_at(&g_catalog_pipeline_descriptor_bindings, pipeline_descriptor_binding_index);

    ImGui::PushID(pipeline_descriptor_binding);

    if (ImGui::TreeNodeEx(pipeline_descriptor_binding->binding_name, ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding)) {

      ImGui::Dummy(ImVec2(0.0f, 5.0F));

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

      ImGui::Dummy(ImVec2(0.0f, 5.0F));

      ImGui::TreePop();
    }

    ImGui::PopID();

    pipeline_descriptor_binding_index++;
  }

  ImGui::Dummy(ImVec2(0.0f, 5.0F));
}
static void detail_draw_model(void) {
  model_asset_t *model_asset = (model_asset_t *)vector_at(&g_catalog_model_assets, g_catalog_selected_model_asset);

  ImGui::Dummy(ImVec2(0.0f, 5.0F));

  if (ImGui::Button("Save")) {
    database_store_model_asset(model_asset);
  }

  ImGui::Dummy(ImVec2(0.0f, 5.0F));

  ImGui::Text(model_asset->name);

  ImGui::Dummy(ImVec2(0.0f, 5.0F));

  ImGui::Text("Meshes");

  ImGui::Dummy(ImVec2(0.0f, 5.0F));

  uint64_t model_mesh_index = 0;
  uint64_t model_mesh_count = vector_count(&g_catalog_model_meshes);

  while (model_mesh_index < model_mesh_count) {

    model_mesh_t *model_mesh = (model_mesh_t *)vector_at(&g_catalog_model_meshes, model_mesh_index);

    ImGui::PushID(model_mesh);

    if (ImGui::TreeNodeEx(model_mesh->name, ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding)) {

      ImGui::TreePop();
    }

    ImGui::PopID();

    model_mesh_index++;
  }

  ImGui::Dummy(ImVec2(0.0f, 5.0F));
}
