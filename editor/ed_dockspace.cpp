#include <editor/ed_pch.h>
#include <editor/ed_main.h>
#include <editor/ed_dockspace.h>

#include <editor/dockable/ed_hierarchy.h>
#include <editor/dockable/ed_inspector.h>
#include <editor/dockable/ed_profiler.h>

#include <editor/view/ed_sceneview.h>

static dockspace_type_t s_dockspace_type = DOCKSPACE_TYPE_NONE;

static uint8_t s_dockspace_is_initialized = 0;

void dockspace_switch(context_t *context, dockspace_type_t type) {
  s_dockspace_type = type;
  s_dockspace_is_initialized = 0;
}
void dockspace_draw(context_t *context) {
  ImGuiID dockspace_id = ImGui::GetID("dockspace");

  ImGuiWindowFlags window_flags =
    ImGuiWindowFlags_NoTitleBar |
    ImGuiWindowFlags_NoResize |
    ImGuiWindowFlags_NoMove |
    ImGuiWindowFlags_NoScrollbar |
    ImGuiWindowFlags_NoScrollWithMouse |
    ImGuiWindowFlags_NoSavedSettings |
    ImGuiWindowFlags_NoCollapse |
    ImGuiWindowFlags_NoBringToFrontOnFocus;

  ImGuiDockNodeFlags dock_node_flags =
    ImGuiDockNodeFlags_NoCloseButton;

  ImGui::SetNextWindowPos(ImVec2(0.0F, (float)context->window_titlebar_height));
  ImGui::SetNextWindowSize(ImVec2((float)context->window_width, (float)context->window_height - (float)context->window_titlebar_height - context->window_statusbar_height));
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.0F, 5.0F));
  ImGui::Begin("dockspace", 0, window_flags);
  ImGui::PopStyleVar();

  ImGui::DockSpace(dockspace_id, ImVec2(0.0F, 0.0F), dock_node_flags);

  if (s_dockspace_is_initialized == 0) {

    s_dockspace_is_initialized = 1;

    ImVec2 dockspace_size = ImGui::GetWindowSize();

    ImGui::DockBuilderRemoveNode(dockspace_id);
    ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(dockspace_id, dockspace_size);

    switch (s_dockspace_type) {
      case DOCKSPACE_TYPE_NONE: {

        ImGuiID dock_id_main = dockspace_id;

        ImGui::DockBuilderDockWindow("SceneView", dock_id_main);

        break;
      }
      case DOCKSPACE_TYPE_SCENE: {

        ImGuiID dock_id_main = dockspace_id;
        ImGuiID dock_id_right_bottom = 0;
        ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dock_id_main, ImGuiDir_Right, 0.25F, 0, &dock_id_main);
        ImGuiID dock_id_right_top = ImGui::DockBuilderSplitNode(dock_id_right, ImGuiDir_Up, 0.25F, 0, &dock_id_right_bottom);
        ImGuiID dock_id_bottom = ImGui::DockBuilderSplitNode(dock_id_main, ImGuiDir_Down, 0.25F, 0, &dock_id_main);

        ImGui::DockBuilderDockWindow("SceneView", dock_id_main);
        ImGui::DockBuilderDockWindow("Hierarchy", dock_id_right_top);
        ImGui::DockBuilderDockWindow("Inspector", dock_id_right_bottom);
        ImGui::DockBuilderDockWindow("Profiler", dock_id_bottom);

        break;
      }
      case DOCKSPACE_TYPE_MODEL: {

        // TODO

        break;
      }
    }

    ImGui::DockBuilderFinish(dockspace_id);
  }

  ImGui::End();

  switch (s_dockspace_type) {
    case DOCKSPACE_TYPE_NONE: {

      sceneview_draw(context);

      break;
    }
    case DOCKSPACE_TYPE_SCENE: {

      hierarchy_draw(context);
      inspector_draw(context);
      profiler_draw(context);

      sceneview_draw(context);

      break;
    }
    case DOCKSPACE_TYPE_MODEL: {

      // TODO

      break;
    }
  }
}

uint8_t dockspace_begin_child(char const *name, uint8_t *is_open, uint8_t *is_docked) {
  if (*is_open) {
    ImGuiWindowFlags window_flags =
      ImGuiWindowFlags_NoScrollbar |
      ImGuiWindowFlags_NoCollapse;

    ImGui::Begin(name, (bool *)is_open, window_flags);

    *is_docked = ImGui::IsWindowDocked();

    if (*is_docked) {
      ImGui::PushStyleColor(ImGuiCol_ChildBg, EDITOR_DOCKING_BACKGROUND_COLOR);
      ImGui::BeginChild("FirstChild");

      ImVec2 second_window_size = ImGui::GetWindowSize();
      ImVec2 second_cursor_position = ImVec2(3.0F, 3.0F);

      second_window_size.x -= 6.0F;
      second_window_size.y -= 6.0F;

      ImGui::SetCursorPos(second_cursor_position);
      ImGui::BeginChild("SecondChild", second_window_size);
    }

    return 1;
  }

  return 0;
}
void dockspace_end_child(uint8_t is_docked) {
  if (is_docked) {
    ImGui::EndChild();
    ImGui::EndChild();
    ImGui::PopStyleColor(1);
  }

  ImGui::End();
}
