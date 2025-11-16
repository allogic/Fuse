#include <editor/ed_pch.h>
#include <editor/ed_main.h>
#include <editor/ed_statusbar.h>

static void ed_statusbar_draw_dockspace_controls(context_t *context);

void ed_statusbar_draw(context_t *context) {
  ImGui::SetNextWindowPos(ImVec2(0.0F, (float)context->window_height - (float)context->window_statusbar_height));
  ImGui::SetNextWindowSize(ImVec2((float)context->window_width, (float)context->window_statusbar_height));

  ImGui::PushStyleColor(ImGuiCol_Button, EDITOR_BACKGROUND_COLOR);
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, EDITOR_HIGHLIGHT_COLOR);
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, EDITOR_ACTIVE_COLOR);

  ImGuiWindowFlags titlebar_flags =
    ImGuiWindowFlags_NoDocking |
    ImGuiWindowFlags_NoTitleBar |
    ImGuiWindowFlags_NoResize |
    ImGuiWindowFlags_NoMove |
    ImGuiWindowFlags_NoScrollbar |
    ImGuiWindowFlags_NoScrollWithMouse |
    ImGuiWindowFlags_NoSavedSettings |
    ImGuiWindowFlags_NoCollapse |
    ImGuiWindowFlags_NoBringToFrontOnFocus;

  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.0F, 5.0F));
  ImGui::Begin("Statusbar", 0, titlebar_flags);
  ImGui::PopStyleVar(1);

  ed_statusbar_draw_dockspace_controls(context);

  ImGui::End();
  ImGui::PopStyleColor(3);
}

static void ed_statusbar_draw_dockspace_controls(context_t *context) {
  ImVec2 window_size = ImGui::GetWindowSize();

  static ImGuiComboFlags combo_flags =
    ImGuiComboFlags_NoArrowButton |
    ImGuiComboFlags_WidthFitPreview;

  ImGui::SetCursorPos(ImVec2(5.0F, 2.5F));

  if (ImGui::BeginCombo("##DockspaceType", g_dockspace_type_names[g_dockspace_selected_type], combo_flags)) {

    uint64_t type_index = 0;
    uint64_t type_count = DOCKSPACE_TYPE_COUNT;

    while (type_index < type_count) {

      uint8_t is_selected = (g_dockspace_selected_type == type_index);

      if (ImGui::Selectable(g_dockspace_type_names[type_index], is_selected)) {
        g_dockspace_selected_type = (ed_dockspace_type_t)type_index;
        g_dockspace_is_initialized = 0;
      }

      if (is_selected) {
        ImGui::SetItemDefaultFocus();
      }

      type_index++;
    }

    ImGui::EndCombo();
  }
}
