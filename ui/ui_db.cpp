#include <ui/ui_pch.h>
#include <ui/ui_pipeline.h>
#include <ui/ui_db.h>

#include <imgui/imgui.h>

void ui_db_create() {
  ui_pipeline_create();
}
void ui_db_draw() {
  ImGui::Begin("Database");

  ui_pipeline_draw();

  ImGui::End();
}
void ui_db_destroy() {
  ui_pipeline_destroy();
}
