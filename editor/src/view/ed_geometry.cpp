#include <editor/ed_pch.h>

#include <editor/view/ed_geometry.h>

/*
ed_geometry_view_t *ed_geometry_view_create(eg_context_t *context) {
  ed_geometry_view_t *geometry = (ed_geometry_view_t *)eg_heap_alloc(sizeof(ed_geometry_view_t), 1, 0);

  geometry->base.context = context;
  geometry->base.is_dirty = 0;
  geometry->base.is_open = 1;
  geometry->base.is_docked = 0;

  return geometry;
}
void ed_geometry_view_refresh(ed_geometry_view_t *geometry) {
  // TODO
}
void ed_geometry_view_draw(ed_geometry_view_t *geometry) {
  static ImGuiTableFlags table_flags =
    ImGuiTableFlags_BordersInner |
    ImGuiTableFlags_RowBg |
    ImGuiTableFlags_Resizable |
    ImGuiTableFlags_SizingStretchProp;

  if (ImGui::BeginTable("##GeometryTable", 15, table_flags)) {

    ImGui::TableSetupColumn("P[0]", ImGuiTableColumnFlags_WidthFixed, 50.0F);
    ImGui::TableSetupColumn("P[1]", ImGuiTableColumnFlags_WidthFixed, 50.0F);
    ImGui::TableSetupColumn("P[2]", ImGuiTableColumnFlags_WidthFixed, 50.0F);

    ImGui::TableSetupColumn("N[0]", ImGuiTableColumnFlags_WidthFixed, 50.0F);
    ImGui::TableSetupColumn("N[1]", ImGuiTableColumnFlags_WidthFixed, 50.0F);
    ImGui::TableSetupColumn("N[2]", ImGuiTableColumnFlags_WidthFixed, 50.0F);

    ImGui::TableSetupColumn("T[0]", ImGuiTableColumnFlags_WidthFixed, 50.0F);
    ImGui::TableSetupColumn("T[1]", ImGuiTableColumnFlags_WidthFixed, 50.0F);
    ImGui::TableSetupColumn("T[2]", ImGuiTableColumnFlags_WidthFixed, 50.0F);

    ImGui::TableSetupColumn("UV_CH0[0]", ImGuiTableColumnFlags_WidthFixed, 50.0F);
    ImGui::TableSetupColumn("UV_CH0[1]", ImGuiTableColumnFlags_WidthFixed, 50.0F);

    ImGui::TableSetupColumn("C_CH0[0]", ImGuiTableColumnFlags_WidthFixed, 50.0F);
    ImGui::TableSetupColumn("C_CH0[1]", ImGuiTableColumnFlags_WidthFixed, 50.0F);
    ImGui::TableSetupColumn("C_CH0[2]", ImGuiTableColumnFlags_WidthFixed, 50.0F);
    ImGui::TableSetupColumn("C_CH0[3]", ImGuiTableColumnFlags_WidthFixed, 50.0F);

    ImGui::TableHeadersRow();

    // if (g_canvas_pcg->selected_node != -1) {
    //   ed_node_t *node = *(ed_node_t **)vector_at(&g_canvas_pcg->handle.nodes, g_canvas_pcg->selected_node);
    //
    //   if (node->type == ED_NODE_TYPE_MESH) {
    //
    //     ed_mesh_node_t *mesh_node = (ed_mesh_node_t *)node;
    //
    //     uint64_t vertex_index = 0;
    //     uint64_t vertex_count = mesh_node->vertex_buffer->size / sizeof(eg_brdf_vertex_t);
    //
    //     while (vertex_index < vertex_count) {
    //
    //       eg_brdf_vertex_t *vertex = ((eg_brdf_vertex_t *)mesh_node->vertex_buffer->mapped_memory) + vertex_index;
    //
    //       ImGui::TableNextRow();
    //
    //       ImGui::TableSetColumnIndex(0);
    //       ImGui::Text("%.3F", vertex->position.x);
    //       ImGui::TableSetColumnIndex(1);
    //       ImGui::Text("%.3F", vertex->position.y);
    //       ImGui::TableSetColumnIndex(2);
    //       ImGui::Text("%.3F", vertex->position.z);
    //
    //       ImGui::TableSetColumnIndex(3);
    //       ImGui::Text("%.3F", vertex->normal.x);
    //       ImGui::TableSetColumnIndex(4);
    //       ImGui::Text("%.3F", vertex->normal.y);
    //       ImGui::TableSetColumnIndex(5);
    //       ImGui::Text("%.3F", vertex->normal.z);
    //
    //       ImGui::TableSetColumnIndex(6);
    //       ImGui::Text("%.3F", vertex->tangent.x);
    //       ImGui::TableSetColumnIndex(7);
    //       ImGui::Text("%.3F", vertex->tangent.y);
    //       ImGui::TableSetColumnIndex(8);
    //       ImGui::Text("%.3F", vertex->tangent.z);
    //
    //       ImGui::TableSetColumnIndex(9);
    //       ImGui::Text("%.3F", vertex->texcoord_channel_0.x);
    //       ImGui::TableSetColumnIndex(10);
    //       ImGui::Text("%.3F", vertex->texcoord_channel_0.y);
    //
    //       ImGui::TableSetColumnIndex(11);
    //       ImGui::Text("%.3F", vertex->color_channel_0.x);
    //       ImGui::TableSetColumnIndex(12);
    //       ImGui::Text("%.3F", vertex->color_channel_0.y);
    //       ImGui::TableSetColumnIndex(13);
    //       ImGui::Text("%.3F", vertex->color_channel_0.z);
    //       ImGui::TableSetColumnIndex(14);
    //       ImGui::Text("%.3F", vertex->color_channel_0.w);
    //
    //       vertex_index++;
    //     }
    //   }
    // }

    ImGui::EndTable();
  }
}
void ed_geometry_view_destroy(ed_geometry_view_t *geometry) {
  eg_heap_free(geometry);
}
*/
