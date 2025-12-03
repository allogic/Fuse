#include <editor/ed_pch.h>
#include <editor/ed_node.h>

/*
void ed_node_draw_input(int attribute_id, const char *attribute_label) {
  ImNodes::BeginInputAttribute(attribute_id);

  ImGui::TextUnformatted(attribute_label);

  ImNodes::EndInputAttribute();
}
void ed_node_draw_output(int attribute_id, const char *attribute_label) {
  ImNodes::BeginOutputAttribute(attribute_id);

  ImGui::AlignTextToFramePadding();

  float text_width = ImGui::CalcTextSize(attribute_label).x;
  float cursor_x = ImGui::GetCursorPosX() + 200.0F - text_width;

  ImGui::SetCursorPosX(cursor_x);
  ImGui::TextUnformatted(attribute_label);

  ImNodes::EndOutputAttribute();
}
*/
