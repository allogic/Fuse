#include <ui/ui_pch.h>
#include <ui/ui_db.h>
#include <ui/ui_pipeline.h>
#include <ui/ui_model.h>

#include <imgui/imgui.h>

void ui_db_create() {
  ui_pipeline_create();
  ui_model_create();
}
void ui_db_draw() {
  ImGui::Begin("Database");

  if (ImGui::Button("Import Default Assets")) {
    importer_import_default_assets();
  }

  ui_pipeline_draw();
  ui_model_draw();

  ImGui::End();
}
void ui_db_destroy() {
  ui_pipeline_destroy();
  ui_model_destroy();
}
