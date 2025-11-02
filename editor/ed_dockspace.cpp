#include <editor/ed_pch.h>
#include <editor/ed_catalog.h>
#include <editor/ed_detail.h>
#include <editor/ed_hierarchy.h>
#include <editor/ed_dockspace.h>
#include <editor/ed_inspector.h>
#include <editor/ed_viewport.h>

static void dockspace_setup_default_layout();

bool dockspace_layout_is_dirty = true;

void dockspace_create() {
}
void dockspace_refresh() {
}
void dockspace_draw(context_t *context) {
  ImGui::SetNextWindowPos(ImVec2(0.0F, (float)context->window_titlebar_height));
  ImGui::SetNextWindowSize(ImVec2((float)context->window_width, (float)context->window_height - (float)context->window_titlebar_height - context->window_statusbar_height));

  ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(30, 30, 30, 255));
  ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(50, 50, 50, 255));
  ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(30, 30, 30, 255));
  ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(30, 30, 30, 255));
  ImGui::PushStyleColor(ImGuiCol_TitleBg, IM_COL32(30, 30, 30, 255));
  ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, IM_COL32(30, 30, 30, 255));
  ImGui::PushStyleColor(ImGuiCol_TitleBgActive, IM_COL32(30, 30, 30, 255));
  ImGui::PushStyleColor(ImGuiCol_ResizeGrip, IM_COL32(30, 30, 30, 255));
  ImGui::PushStyleColor(ImGuiCol_ResizeGripHovered, IM_COL32(30, 30, 30, 255));
  ImGui::PushStyleColor(ImGuiCol_ResizeGripActive, IM_COL32(30, 30, 30, 255));
  ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, IM_COL32(30, 30, 30, 255));
  ImGui::PushStyleColor(ImGuiCol_DockingPreview, IM_COL32(30, 30, 30, 255));
  ImGui::PushStyleColor(ImGuiCol_Tab, IM_COL32(50, 50, 50, 255));
  ImGui::PushStyleColor(ImGuiCol_TabHovered, IM_COL32(50, 50, 50, 255));
  ImGui::PushStyleColor(ImGuiCol_TabActive, IM_COL32(50, 50, 50, 255));
  ImGui::PushStyleColor(ImGuiCol_TabUnfocused, IM_COL32(50, 50, 50, 255));
  ImGui::PushStyleColor(ImGuiCol_TabUnfocusedActive, IM_COL32(50, 50, 50, 255));
  ImGui::PushStyleColor(ImGuiCol_Separator, IM_COL32(50, 50, 50, 255));
  ImGui::PushStyleColor(ImGuiCol_SeparatorActive, IM_COL32(50, 50, 50, 255));
  ImGui::PushStyleColor(ImGuiCol_SeparatorHovered, IM_COL32(50, 50, 50, 255));
  ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, IM_COL32(30, 30, 30, 255));
  ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, IM_COL32(50, 50, 50, 255));
  ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabHovered, IM_COL32(30, 30, 30, 255));
  ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabActive, IM_COL32(30, 30, 30, 255));

  ImGuiWindowFlags docking_flags =
    ImGuiWindowFlags_NoTitleBar |
    ImGuiWindowFlags_NoResize |
    ImGuiWindowFlags_NoMove |
    ImGuiWindowFlags_NoScrollbar |
    ImGuiWindowFlags_NoScrollWithMouse |
    ImGuiWindowFlags_NoSavedSettings |
    ImGuiWindowFlags_NoCollapse |
    ImGuiWindowFlags_NoBringToFrontOnFocus;

  ImGuiID dockspace_id = ImGui::GetID("Dockspace");

  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.0F, 5.0F));
  ImGui::Begin("Root", 0, docking_flags);
  ImGui::DockSpace(dockspace_id);
  ImGui::PopStyleVar(1);

  catalog_draw(context);
  detail_draw(context);
  hierarchy_draw(context);
  inspector_draw(context);
  viewport_draw(context);

  ImGui::End();
  ImGui::PopStyleColor(24);
}
void dockspace_destroy() {
}

static void dockspace_setup_default_layout() {
  if (dockspace_layout_is_dirty) {
    dockspace_layout_is_dirty = false;

    ImGuiID dockspace_id = ImGui::GetID("Dockspace");

    ImGui::DockBuilderRemoveNode(dockspace_id);
    ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetWindowSize());

    ImGuiID dock_main_id = dockspace_id;
    ImGuiID dock_left_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.2f, nullptr, &dock_main_id);
    ImGuiID dock_right_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.25f, nullptr, &dock_main_id);
    ImGuiID dock_bottom_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.25f, nullptr, &dock_main_id);

    ImGui::DockBuilderDockWindow("Hierarchy", dock_main_id);
    ImGui::DockBuilderDockWindow("Catalog", dock_left_id);
    ImGui::DockBuilderDockWindow("Inspector", dock_right_id);
    ImGui::DockBuilderDockWindow("Detail", dock_bottom_id);

    ImGui::DockBuilderFinish(dockspace_id);
  }
}
