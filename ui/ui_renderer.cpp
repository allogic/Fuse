#include <ui/ui_pch.h>
#include <ui/ui_renderer.h>

#include <imgui/imgui.h>

void ui_renderer_create() {
  // TODO
}
void ui_renderer_draw() {
  ImGui::Begin("Renderer");

  ImGui::Dummy(ImVec2(0.0f, 10.0F));

  if (ImGui::Button("Rebuild")) {
    g_globals.renderer_is_dirty = 1;
  }

  static ImGuiTableFlags pipeline_descriptor_binding_table_flags =
    ImGuiTableFlags_Borders |
    ImGuiTableFlags_RowBg |
    ImGuiTableFlags_Resizable |
    ImGuiTableFlags_SizingStretchProp;

  ImGui::Text("Descriptor Bindings");
  if (ImGui::BeginTable("##RendererDescriptorBindingTable", 2, pipeline_descriptor_binding_table_flags)) {
    ImGui::TableSetupColumn("BINDING", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("BINDING_NAME", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableHeadersRow();

    ImGui::TableNextRow();

    ImGui::TableSetColumnIndex(0);
    ImGui::Text("%d", 0);

    ImGui::TableSetColumnIndex(1);
    ImGui::Text("%s", "kekw");

    ImGui::EndTable();
  }

  ImGui::Dummy(ImVec2(0.0f, 10.0F));

  ImGui::End();
}
void ui_renderer_destroy() {
  // TODO
}
