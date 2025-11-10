#include <editor/ed_pch.h>
#include <editor/ed_modelview.h>
#include <editor/ed_main.h>

static void modelview_build_layout(modelview_t *modelview);

modelview_t *modelview_create(context_t *context, char const *name) {
  modelview_t *modelview = (modelview_t *)heap_alloc(sizeof(modelview_t), 1, 0);

  modelview->context = context;
  modelview->width = 1;
  modelview->height = 1;
  modelview->prev_width = 1;
  modelview->prev_height = 1;
  modelview->is_dirty = 0;
  modelview->is_open = 1;
  modelview->is_docked = 0;
  // modelview->viewport = viewport_create(modelview->context, modelview->width, modelview->height);

  strcpy(modelview->name, name);

  while (g_editor_modelviews[modelview->link_index]) {

    modelview->link_index++;
  }

  ASSERT(modelview->link_index < 0xFF, "Invalid link index");

  g_editor_modelviews[modelview->link_index] = modelview;

  return modelview;
}
void modelview_refresh(modelview_t *modelview) {
  if (modelview->is_dirty) {

    modelview->is_dirty = 0;

    viewport_resize(modelview->viewport, modelview->width, modelview->height);
  }
}
void modelview_draw(modelview_t *modelview) {
  ImGui::Begin(modelview->name);

  ImGuiID dockspace_id = ImGui::GetID("ModelView");

  ImGui::DockSpace(dockspace_id, ImVec2(0.0F, 0.0F), ImGuiDockNodeFlags_None);

  static uint8_t is_init = 1;

  if (is_init) {
    is_init = 0;

    modelview_build_layout(modelview);
  }

  ImGui::End();

  ImGui::Begin("LeftPanel");
  ImGui::Text("This is the left panel");
  ImGui::End();

  ImGui::Begin("RightPanel");
  ImGui::Text("This is the right panel");
  ImGui::End();

  ImGui::Begin("BottomPanel");
  ImGui::Text("This is the bottom panel");
  ImGui::End();

  ImGui::Begin("MainView");
  ImGui::Text("This is the main viewport");
  ImGui::End();
}
void modelview_destroy(modelview_t *modelview) {
  ASSERT(modelview->link_index < 0xFF, "Invalid link index");

  g_editor_modelviews[modelview->link_index] = 0;

  // viewport_destroy(modelview->viewport);

  heap_free(modelview);
}

static void modelview_build_layout(modelview_t *modelview) {
  ImGuiID dockspace_id = ImGui::GetID("ModelView");

  ImGui::DockBuilderRemoveNode(dockspace_id);
  ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
  ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetWindowSize());

  ImGuiID dock_id_main = dockspace_id;
  ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dock_id_main, ImGuiDir_Left, 0.2F, 0, &dock_id_main);
  ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dock_id_main, ImGuiDir_Right, 0.25F, 0, &dock_id_main);
  ImGuiID dock_id_bottom = ImGui::DockBuilderSplitNode(dock_id_main, ImGuiDir_Down, 0.25F, 0, &dock_id_main);

  ImGui::DockBuilderDockWindow("LeftPanel", dock_id_left);
  ImGui::DockBuilderDockWindow("RightPanel", dock_id_right);
  ImGui::DockBuilderDockWindow("BottomPanel", dock_id_bottom);
  ImGui::DockBuilderDockWindow("MainView", dock_id_main);

  ImGui::DockBuilderFinish(dockspace_id);
}
