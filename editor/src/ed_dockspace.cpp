#include <editor/ed_pch.h>
#include <editor/ed_main.h>
#include <editor/ed_dockspace.h>

#include <editor/view/ed_canvas.h>
#include <editor/view/ed_catalog.h>
#include <editor/view/ed_geometry.h>
#include <editor/view/ed_hierarchy.h>
#include <editor/view/ed_inspector.h>
#include <editor/view/ed_profiler.h>
#include <editor/view/ed_viewport.h>

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

  uint32_t window_width = eg_context_window_width(context);
  uint32_t window_height = eg_context_window_height(context);
  uint32_t window_titlebar_height = eg_context_titlebar_height(context);
  uint32_t window_statusbar_height = eg_context_statusbar_height(context);

  ImGui::SetNextWindowPos(ImVec2(0.0F, (float)window_titlebar_height));
  ImGui::SetNextWindowSize(ImVec2((float)window_width, (float)window_height - (float)window_titlebar_height - window_statusbar_height));

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

          ImGuiID dock_id_viewport = dockspace_id;
          ImGuiID dock_id_inspector = 0;
          ImGuiID dock_id_catalog = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.15F, 0, &dockspace_id);
          ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.25F, 0, &dockspace_id);
          ImGuiID dock_id_hierarchy = ImGui::DockBuilderSplitNode(dock_id_right, ImGuiDir_Up, 0.25F, 0, &dock_id_inspector);

          ImGui::DockBuilderDockWindow("Viewport", dock_id_viewport);
          ImGui::DockBuilderDockWindow("Catalog", dock_id_catalog);
          ImGui::DockBuilderDockWindow("Hierarchy", dock_id_hierarchy);
          ImGui::DockBuilderDockWindow("Inspector", dock_id_inspector);

          break;
        }
        case ED_DOCKSPACE_TYPE_MODEL: {

          ImGuiID dock_id_viewport = dockspace_id;
          ImGuiID dock_id_inspector = 0;
          ImGuiID dock_id_catalog = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.15F, 0, &dockspace_id);
          ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.25F, 0, &dockspace_id);
          ImGuiID dock_id_hierarchy = ImGui::DockBuilderSplitNode(dock_id_right, ImGuiDir_Up, 0.25F, 0, &dock_id_inspector);

          ImGui::DockBuilderDockWindow("Viewport", dock_id_viewport);
          ImGui::DockBuilderDockWindow("Catalog", dock_id_catalog);
          ImGui::DockBuilderDockWindow("Hierarchy", dock_id_hierarchy);
          ImGui::DockBuilderDockWindow("Inspector", dock_id_inspector);

          break;
        }
        case ED_DOCKSPACE_TYPE_PCG: {

          ImGuiID dock_id_canvas = 0;
          ImGuiID dock_id_catalog = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.15F, 0, &dockspace_id);
          ImGuiID dock_id_geometry = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.2F, 0, &dockspace_id);
          ImGuiID dock_id_viewport = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.5F, 0, &dock_id_canvas);

          ImGui::DockBuilderDockWindow("Viewport", dock_id_viewport);
          ImGui::DockBuilderDockWindow("Canvas", dock_id_canvas);
          ImGui::DockBuilderDockWindow("Catalog", dock_id_catalog);
          ImGui::DockBuilderDockWindow("Geometry", dock_id_geometry);

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

      ed_viewport_view_draw(g_viewport_game, 0);

      ImGui::End();

      break;
    }
    case ED_DOCKSPACE_TYPE_SCENE: {

      if (ed_dockspace_begin_child("Catalog", &g_catalog_scene->base.is_open, &g_catalog_scene->base.is_docked)) {

        ed_catalog_view_draw(g_catalog_scene);

        ed_dockspace_end_child(g_catalog_scene->base.is_docked);
      }

      if (ed_dockspace_begin_child("Hierarchy", &g_hierarchy_scene->base.is_open, &g_hierarchy_scene->base.is_docked)) {

        ed_hierarchy_view_draw(g_hierarchy_scene);

        ed_dockspace_end_child(g_hierarchy_scene->base.is_docked);
      }

      if (ed_dockspace_begin_child("Inspector", &g_inspector_scene->base.is_open, &g_inspector_scene->base.is_docked)) {

        ed_inspector_view_draw(g_inspector_scene);

        ed_dockspace_end_child(g_inspector_scene->base.is_docked);
      }

      if (ed_dockspace_begin_child("Viewport", &g_viewport_scene->base.is_open, &g_viewport_scene->base.is_docked)) {

        ed_viewport_view_draw(g_viewport_scene, 1);

        ed_dockspace_end_child(g_viewport_scene->base.is_docked);
      }

      break;
    }
    case ED_DOCKSPACE_TYPE_MODEL: {

      if (ed_dockspace_begin_child("Catalog", &g_catalog_model->base.is_open, &g_catalog_model->base.is_docked)) {

        ed_catalog_view_draw(g_catalog_model);

        ed_dockspace_end_child(g_catalog_model->base.is_docked);
      }

      if (ed_dockspace_begin_child("Hierarchy", &g_hierarchy_model->base.is_open, &g_hierarchy_model->base.is_docked)) {

        ed_hierarchy_view_draw(g_hierarchy_model);

        ed_dockspace_end_child(g_hierarchy_model->base.is_docked);
      }

      if (ed_dockspace_begin_child("Inspector", &g_inspector_model->base.is_open, &g_inspector_model->base.is_docked)) {

        ed_inspector_view_draw(g_inspector_model);

        ed_dockspace_end_child(g_inspector_model->base.is_docked);
      }

      if (ed_dockspace_begin_child("Viewport", &g_viewport_model->base.is_open, &g_viewport_model->base.is_docked)) {

        ed_viewport_view_draw(g_viewport_model, 1);

        ed_dockspace_end_child(g_viewport_model->base.is_docked);
      }

      break;
    }
    case ED_DOCKSPACE_TYPE_PCG: {

      if (ed_dockspace_begin_child("Catalog", &g_catalog_pcg->base.is_open, &g_catalog_pcg->base.is_docked)) {

        ed_catalog_view_draw(g_catalog_pcg);

        ed_dockspace_end_child(g_catalog_pcg->base.is_docked);
      }

      if (ed_dockspace_begin_child("Viewport", &g_viewport_pcg->base.is_open, &g_viewport_pcg->base.is_docked)) {

        ed_viewport_view_draw(g_viewport_pcg, 1);

        ed_dockspace_end_child(g_viewport_pcg->base.is_docked);
      }

      if (ed_dockspace_begin_child("Canvas", &g_canvas_pcg->base.is_open, &g_canvas_pcg->base.is_docked)) {

        ed_canvas_view_draw(g_canvas_pcg);

        ed_dockspace_end_child(g_canvas_pcg->base.is_docked);
      }

      if (ed_dockspace_begin_child("Geometry", &g_geometry_pcg->base.is_open, &g_geometry_pcg->base.is_docked)) {

        ed_geometry_view_draw(g_geometry_pcg);

        ed_dockspace_end_child(g_geometry_pcg->base.is_docked);
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
      ImGui::PushStyleColor(ImGuiCol_ChildBg, ED_LIGHT_GRAY_COLOR);
      ImGui::BeginChild("FirstChild");

      ImVec2 second_window_size = ImGui::GetWindowSize();
      ImVec2 second_cursor_position = ImVec2(1.0F, 1.0F);

      second_window_size.x -= 2.0F;
      second_window_size.y -= 2.0F;

      ImGui::SetCursorPos(second_cursor_position);

      ImGui::PushStyleColor(ImGuiCol_ChildBg, ED_DARK_GREY);
      ImGui::BeginChild("SecondChild", second_window_size);
    }

    return 1;
  }

  return 0;
}
void ed_dockspace_end_child(uint8_t is_docked) {
  if (is_docked) {
    ImGui::EndChild();
    ImGui::PopStyleColor(1);
    ImGui::EndChild();
    ImGui::PopStyleColor(1);
  }

  ImGui::End();
}
