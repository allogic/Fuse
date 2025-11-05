#include <editor/ed_pch.h>
#include <editor/ed_titlebar.h>
#include <editor/ed_main.h>

static void titlebar_reset_drag_state(context_t *context);

static void titlebar_draw_window_controls(context_t *context);
static void titlebar_draw_scene_controls(context_t *context);

uint8_t g_titlebar_catalog_is_open = 1;
uint8_t g_titlebar_hierarchy_is_open = 1;
uint8_t g_titlebar_inspector_is_open = 1;
uint8_t g_titlebar_detail_is_open = 1;

uint8_t g_titlebar_catalog_is_docked = 0;
uint8_t g_titlebar_hierarchy_is_docked = 0;
uint8_t g_titlebar_inspector_is_docked = 0;
uint8_t g_titlebar_detail_is_docked = 0;

void titlebar_create(context_t *context) {
}
void titlebar_refresh(context_t *context) {
}
void titlebar_draw(context_t *context) {
  ImGui::SetNextWindowPos(ImVec2(0.0F, 0.0F));
  ImGui::SetNextWindowSize(ImVec2((float)context->window_width, (float)context->window_titlebar_height));

  ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(30, 30, 30, 255));
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(70, 70, 70, 255));
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(90, 90, 90, 255));

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
  ImGui::Begin("Titlebar", 0, titlebar_flags);
  ImGui::PopStyleVar(1);

  ImGui::PushFont(g_editor_material_symbols_h1);
  ImGui::Text(ICON_MS_DEPLOYED_CODE);
  ImGui::PopFont();

  ImGui::SameLine(50);

  if (ImGui::Button("Catalog")) {
    g_titlebar_catalog_is_open = 1;
  }
  ImGui::SameLine();
  if (ImGui::Button("Hierarchy")) {
    g_titlebar_hierarchy_is_open = 1;
  }
  ImGui::SameLine();
  if (ImGui::Button("Inspector")) {
    g_titlebar_inspector_is_open = 1;
  }
  ImGui::SameLine();
  if (ImGui::Button("Detail")) {
    g_titlebar_detail_is_open = 1;
  }
  ImGui::SameLine();
  if (ImGui::Button("Viewport 1")) {
    // TODO
  }
  ImGui::SameLine();
  if (ImGui::Button("Viewport 2")) {
    // TODO
  }
  ImGui::SameLine();
  if (ImGui::Button("Viewport 3")) {
    // TODO
  }
  ImGui::SameLine();
  if (ImGui::Button("Viewport 4")) {
    // TODO
  }
  ImGui::SameLine();
  if (ImGui::Button("Import")) {
    importer_import_default_assets();
  }

  titlebar_draw_window_controls(context);
  titlebar_draw_scene_controls(context);

  titlebar_reset_drag_state(context);

  ImGui::End();
  ImGui::PopStyleColor(3);
}
void titlebar_destroy(context_t *context) {
}

static void titlebar_reset_drag_state(context_t *context) {
  if (ImGui::IsWindowHovered() &&
      !ImGui::IsAnyItemHovered() &&
      ImGui::IsMouseDown(ImGuiMouseButton_Left)) {

    ReleaseCapture();

    SendMessage(context->window_handle, WM_NCLBUTTONDOWN, HTCAPTION, 0);
  }

  ImGui::GetIO().MouseDown[0] = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
}

static void titlebar_draw_window_controls(context_t *context) {
  ImVec2 window_size = ImGui::GetWindowSize();

  ImGui::SetCursorPos(ImVec2(window_size.x - 90.0F, 5.0F));

  ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0F);
  ImGui::PushFont(g_editor_material_symbols_h6);

  if (ImGui::Button(ICON_MS_REMOVE)) {
  }

  ImGui::SameLine();
  if (ImGui::Button(ICON_MS_RECTANGLE)) {
  }

  ImGui::SameLine();

  if (ImGui::Button(ICON_MS_CLOSE)) {
    context->is_window_running = 0;
  }

  ImGui::PopFont();
  ImGui::PopStyleVar();
}
static void titlebar_draw_scene_controls(context_t *context) {
  ImVec2 window_size = ImGui::GetWindowSize();

  ImGui::SetCursorPos(ImVec2((window_size.x / 2.0F) - 40.0F, 30.0F));

  ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0F);
  ImGui::PushFont(g_editor_material_symbols_h5);

  if (ImGui::Button(ICON_MS_PLAY_ARROW)) {
  }

  ImGui::SameLine();

  if (ImGui::Button(ICON_MS_PAUSE)) {
  }

  ImGui::SameLine();

  if (ImGui::Button(ICON_MS_STOP)) {
  }

  ImGui::PopFont();
  ImGui::PopStyleVar();
}
