#include <editor/ed_pch.h>
#include <editor/ed_main.h>
#include <editor/ed_dockspace.h>
#include <editor/ed_viewport.h>

#include <editor/dockable/ed_canvas.h>
#include <editor/dockable/ed_catalog.h>
#include <editor/dockable/ed_hierarchy.h>
#include <editor/dockable/ed_inspector.h>
#include <editor/dockable/ed_profiler.h>

void ed_dockspace_draw(eg_context_t *context) {
  ImGuiID dockspace_id = ImGui::GetID("Dockspace");

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

  if (g_dockspace_selected_type != ED_DOCKSPACE_TYPE_GAME) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.0F, 5.0F));
    ImGui::Begin("Dockspace", 0, window_flags);
    ImGui::PopStyleVar();
    ImGui::DockSpace(dockspace_id, ImVec2(0.0F, 0.0F), dock_node_flags);

    if (g_dockspace_is_dirty) {

      g_dockspace_is_dirty = 0;

      ImVec2 dockspace_size = ImGui::GetWindowSize();

      ImGui::DockBuilderRemoveNode(dockspace_id);
      ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
      ImGui::DockBuilderSetNodeSize(dockspace_id, dockspace_size);

      switch (g_dockspace_selected_type) {
        case ED_DOCKSPACE_TYPE_GAME: {

          // TODO

          break;
        }
        case ED_DOCKSPACE_TYPE_SCENE: {

          ImGuiID dock_id_main = dockspace_id;
          ImGuiID dock_id_right_bottom = 0;
          ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dock_id_main, ImGuiDir_Right, 0.25F, 0, &dock_id_main);
          ImGuiID dock_id_right_top = ImGui::DockBuilderSplitNode(dock_id_right, ImGuiDir_Up, 0.25F, 0, &dock_id_right_bottom);
          ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dock_id_main, ImGuiDir_Left, 0.2F, 0, &dock_id_main);

          ImGui::DockBuilderDockWindow("Viewport", dock_id_main);
          ImGui::DockBuilderDockWindow("Catalog", dock_id_left);
          ImGui::DockBuilderDockWindow("Hierarchy", dock_id_right_top);
          ImGui::DockBuilderDockWindow("Inspector", dock_id_right_bottom);

          break;
        }
        case ED_DOCKSPACE_TYPE_MODEL: {

          ImGuiID dock_id_main = dockspace_id;
          ImGuiID dock_id_main_bottom = 0;
          ImGuiID dock_id_main_top = ImGui::DockBuilderSplitNode(dock_id_main, ImGuiDir_Up, 0.5F, 0, &dock_id_main_bottom);
          ImGuiID dock_id_right_bottom = 0;
          ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dock_id_main, ImGuiDir_Right, 0.25F, 0, &dock_id_main);
          ImGuiID dock_id_right_top = ImGui::DockBuilderSplitNode(dock_id_right, ImGuiDir_Up, 0.25F, 0, &dock_id_right_bottom);
          ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dock_id_main, ImGuiDir_Left, 0.2F, 0, &dock_id_main);

          ImGui::DockBuilderDockWindow("Viewport", dock_id_main_top);
          ImGui::DockBuilderDockWindow("Canvas", dock_id_main_bottom);
          ImGui::DockBuilderDockWindow("Catalog", dock_id_left);
          ImGui::DockBuilderDockWindow("Hierarchy", dock_id_right_top);
          ImGui::DockBuilderDockWindow("Inspector", dock_id_right_bottom);

          break;
        }
      }

      ImGui::DockBuilderFinish(dockspace_id);
    }

    ImGui::End();
  }

  switch (g_dockspace_selected_type) {
    case ED_DOCKSPACE_TYPE_GAME: {

      ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoDecoration;

      ImGui::Begin("Game", 0, window_flags);

      ed_viewport_draw(&g_viewport_game, 0);

      ImGui::End();

      break;
    }
    case ED_DOCKSPACE_TYPE_SCENE: {

      if (ed_dockspace_begin_child("Catalog", &g_catalog_scene.is_open, &g_catalog_scene.is_docked)) {

        ed_catalog_draw(&g_catalog_scene);

        ed_dockspace_end_child(g_catalog_scene.is_docked);
      }

      if (ed_dockspace_begin_child("Hierarchy", &g_hierarchy_scene.is_open, &g_hierarchy_scene.is_docked)) {

        ed_hierarchy_draw(&g_hierarchy_scene);

        ed_dockspace_end_child(g_hierarchy_scene.is_docked);
      }

      if (ed_dockspace_begin_child("Inspector", &g_inspector_scene.is_open, &g_inspector_scene.is_docked)) {

        ed_inspector_draw(&g_inspector_scene);

        ed_dockspace_end_child(g_inspector_scene.is_docked);
      }

      if (ed_dockspace_begin_child("Viewport", &g_viewport_scene.is_open, &g_viewport_scene.is_docked)) {

        ed_viewport_draw(&g_viewport_scene, 1);

        ed_dockspace_end_child(g_viewport_scene.is_docked);
      }

      break;
    }
    case ED_DOCKSPACE_TYPE_MODEL: {

      if (ed_dockspace_begin_child("Catalog", &g_catalog_model.is_open, &g_catalog_model.is_docked)) {

        ed_catalog_draw(&g_catalog_model);

        ed_dockspace_end_child(g_catalog_model.is_docked);
      }

      if (ed_dockspace_begin_child("Hierarchy", &g_hierarchy_model.is_open, &g_hierarchy_model.is_docked)) {

        ed_hierarchy_draw(&g_hierarchy_model);

        ed_dockspace_end_child(g_hierarchy_model.is_docked);
      }

      if (ed_dockspace_begin_child("Inspector", &g_inspector_model.is_open, &g_inspector_model.is_docked)) {

        ed_inspector_draw(&g_inspector_model);

        ed_dockspace_end_child(g_inspector_model.is_docked);
      }

      if (ed_dockspace_begin_child("Viewport", &g_viewport_model.is_open, &g_viewport_model.is_docked)) {

        ed_viewport_draw(&g_viewport_model, 1);

        ed_dockspace_end_child(g_viewport_model.is_docked);
      }

      if (ed_dockspace_begin_child("Canvas", &g_canvas_model.is_open, &g_canvas_model.is_docked)) {

        ed_canvas_draw(&g_canvas_model);

        ed_dockspace_end_child(g_canvas_model.is_docked);
      }

      break;
    }
  }
}

uint8_t ed_dockspace_begin_child(char const *name, uint8_t *is_open, uint8_t *is_docked) {
  if (*is_open) {
    ImGuiWindowFlags window_flags =
      ImGuiWindowFlags_NoScrollbar |
      ImGuiWindowFlags_NoCollapse;

    ImGui::Begin(name, (bool *)is_open, window_flags);

    *is_docked = ImGui::IsWindowDocked();

    if (*is_docked) {
      ImGui::PushStyleColor(ImGuiCol_ChildBg, ED_SHALLOW_GRAY_COLOR);
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
void ed_dockspace_end_child(uint8_t is_docked) {
  if (is_docked) {
    ImGui::EndChild();
    ImGui::EndChild();
    ImGui::PopStyleColor(1);
  }

  ImGui::End();
}
