#include <editor/ed_pch.h>
#include <editor/ed_main.h>
#include <editor/ed_titlebar.h>

#include <editor/dockable/ed_catalog.h>
#include <editor/dockable/ed_hierarchy.h>
#include <editor/dockable/ed_inspector.h>
#include <editor/dockable/ed_profiler.h>

static void titlebar_reset_drag_state(context_t *context);

static void titlebar_draw_icon(context_t *context);
static void titlebar_draw_main_menu(context_t *context);
static void titlebar_draw_window_controls(context_t *context);
static void titlebar_draw_scene_controls(context_t *context);

void titlebar_create(context_t *context) {
  g_editor_scenes = database_load_scene_assets();
}
void titlebar_refresh(context_t *context) {
}
void titlebar_draw(context_t *context) {
  ImGui::SetNextWindowPos(ImVec2(0.0F, 0.0F));
  ImGui::SetNextWindowSize(ImVec2((float)context->window_width, (float)context->window_titlebar_height));

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
  ImGui::Begin("Titlebar", 0, titlebar_flags);
  ImGui::PopStyleVar(1);

  titlebar_draw_icon(context);
  titlebar_draw_main_menu(context);
  titlebar_draw_window_controls(context);
  titlebar_draw_scene_controls(context);

  titlebar_reset_drag_state(context);

  ImGui::End();
  ImGui::PopStyleColor(3);
}
void titlebar_destroy(context_t *context) {
  database_destroy_scene_assets(&g_editor_scenes);
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

static void titlebar_draw_icon(context_t *context) {
  ImGui::SetCursorPos(ImVec2(10.0F, 10.0F));
  ImGui::PushFont(g_editor_material_symbols_h1);
  ImGui::Text(ICON_MS_TOKEN);
  ImGui::PopFont();
}
static void titlebar_draw_main_menu(context_t *context) {
  ImGui::SetCursorPos(ImVec2(50.0F, 5.0F));

  ImGui::PushID("Titlebar");

  if (ImGui::Button("Catalog")) {
    g_catalog_is_open = !g_catalog_is_open;
  }

  ImGui::SameLine();
  if (ImGui::Button("Hierarchy")) {
    g_hierarchy_is_open = !g_hierarchy_is_open;
  }

  ImGui::SameLine();
  if (ImGui::Button("Inspector")) {
    g_inspector_is_open = !g_inspector_is_open;
  }

  ImGui::SameLine();
  if (ImGui::Button("Profiler")) {
    g_profiler_is_open = !g_profiler_is_open;
  }

  ImGui::SameLine();
  if (ImGui::Button("Import")) {
    importer_import_default_assets();
  }

  ImGui::PopID();
}
static void titlebar_draw_window_controls(context_t *context) {
  ImVec2 window_size = ImGui::GetWindowSize();

  ImGui::SetCursorPos(ImVec2(window_size.x - 90.0F, 5.0F));

  ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0F);
  ImGui::PushFont(g_editor_material_symbols_h6);

  if (ImGui::Button(ICON_MS_REMOVE)) {
    ShowWindow(context->window_handle, SW_MINIMIZE);
  }

  ImGui::SameLine();
  if (ImGui::Button(ICON_MS_RECTANGLE)) {
    static uint8_t is_maximized = 0;

    if (is_maximized) {
      ShowWindow(context->window_handle, SW_RESTORE);
    } else {
      ShowWindow(context->window_handle, SW_MAXIMIZE);
    }

    is_maximized = !is_maximized;
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

  ImGui::SetCursorPos(ImVec2(50.0F, 30.0F));
  ImGui::SetNextItemWidth(150.0F);

  scene_asset_t *selected_scene_asset = (scene_asset_t *)vector_at(&g_editor_scenes, g_editor_selected_scene_asset);

  if (ImGui::BeginCombo("Scene", selected_scene_asset->name)) {

    uint64_t scene_index = 0;
    uint64_t scene_count = vector_count(&g_editor_scenes);

    while (scene_index < scene_count) {

      scene_asset_t *scene_asset = (scene_asset_t *)vector_at(&g_editor_scenes, scene_index);

      uint8_t is_selected = (g_editor_selected_scene_asset == scene_index);

      if (ImGui::Selectable(scene_asset->name, is_selected)) {
        g_editor_selected_scene_asset = scene_index;
      }

      if (is_selected) {
        ImGui::SetItemDefaultFocus();
      }

      scene_index++;
    }

    ImGui::EndCombo();
  }

  ImGui::SetCursorPos(ImVec2(window_size.x - 300.0F, 30.0F));

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
