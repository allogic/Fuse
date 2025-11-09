#include <editor/ed_pch.h>
#include <editor/ed_inspector.h>
#include <editor/ed_dockspace.h>
#include <editor/ed_titlebar.h>
#include <editor/ed_hierarchy.h>
#include <editor/ed_ecs_compat.h>

#include <imgui/imgui.h>

static void inspector_draw_camera(camera_t *camera);
static void inspector_draw_editor_controller(editor_controller_t *editor_controller);
static void inspector_draw_rigidbody(rigidbody_t *rigidbody);
static void inspector_draw_transform(transform_t *transform);

uint8_t g_inspector_is_open = 0;
uint8_t g_inspector_is_docked = 0;

void inspector_create(context_t *context) {
}
void inspector_refresh(context_t *context) {
}
void inspector_draw(context_t *context) {
  if (dockspace_begin_child("Inspector", &g_inspector_is_open, &g_inspector_is_docked)) {

    ecs_world_t *world = context->scene->world;

    if (ecs_is_valid(world, g_hierarchy_selected_entity)) {

      camera_t *camera = ecs_camera_mut(world, g_hierarchy_selected_entity);

      if (camera) {
        inspector_draw_camera(camera);
      }

      editor_controller_t *editor_controller = ecs_editor_controller_mut(world, g_hierarchy_selected_entity);

      if (editor_controller) {
        inspector_draw_editor_controller(editor_controller);
      }

      rigidbody_t *rigidbody = ecs_rigidbody_mut(world, g_hierarchy_selected_entity);

      if (rigidbody) {
        inspector_draw_rigidbody(rigidbody);
      }

      transform_t *transform = ecs_transform_mut(world, g_hierarchy_selected_entity);

      if (transform) {
        inspector_draw_transform(transform);
      }
    }

    dockspace_end_child(g_inspector_is_docked);
  }
}
void inspector_destroy(context_t *context) {
}

static void inspector_draw_camera(camera_t *camera) {
  ImGui::PushItemWidth(ImGui::GetWindowWidth() - ImGui::GetTreeNodeToLabelSpacing() - 100);

  if (ImGui::TreeNodeEx("Camera", ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding)) {

    if (ImGui::DragFloat("Fov", &camera->fov, 0.1F, 0.1F, 90.0F, "%.3F")) {
      // TODO
    }

    if (ImGui::DragFloat("Near Plane", &camera->near_z, 0.1F, 0.1F, 100.0F, "%.6F")) {
      // TODO
    }

    if (ImGui::DragFloat("Far Plane", &camera->far_z, 1.0F, 100.0F, 100000.0F, "%.6F")) {
      // TODO
    }

    ImGui::TreePop();
  }

  ImGui::PopItemWidth();
}
static void inspector_draw_editor_controller(editor_controller_t *editor_controller) {
  ImGui::PushItemWidth(ImGui::GetWindowWidth() - ImGui::GetTreeNodeToLabelSpacing() - 240);

  if (ImGui::TreeNodeEx("Editor Controller", ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding)) {

    if (ImGui::DragFloat("Keyboard Move Speed Fast", &editor_controller->keyboard_move_speed_fast, 0.1F, 0.0F, 100.0F, "%.6F")) {
      // TODO
    }

    if (ImGui::DragFloat("Keyboard Move Speed Normal", &editor_controller->keyboard_move_speed_normal, 1.0F, 0.0F, 1000.0F, "%.6F")) {
      // TODO
    }

    if (ImGui::DragFloat("Mouse Move Speed Fast", &editor_controller->mouse_move_speed_fast, 0.01F, 0.0F, 1.0F, "%.6F")) {
      // TODO
    }

    if (ImGui::DragFloat("Mouse Move Speed Normal", &editor_controller->mouse_move_speed_normal, 0.01F, 0.0F, 1.0F, "%.6F")) {
      // TODO
    }

    if (ImGui::DragFloat("Mouse Rotation Speed", &editor_controller->mouse_rotation_speed, 0.01F, 0.0F, 10.0F, "%.6F")) {
      // TODO
    }

    if (ImGui::DragFloat("Mouse Rotation Drag Damping", &editor_controller->mouse_rotation_drag_damping, 0.01F, 0.0F, 100.0F, "%.6F")) {
      // TODO
    }

    ImGui::TreePop();
  }

  ImGui::PopItemWidth();
}
static void inspector_draw_rigidbody(rigidbody_t *rigidbody) {
  ImGui::PushItemWidth(ImGui::GetWindowWidth() - ImGui::GetTreeNodeToLabelSpacing() - 70);

  if (ImGui::TreeNodeEx("Rigidbody", ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding)) {

    // TODO

    ImGui::TreePop();
  }

  ImGui::PopItemWidth();
}
static void inspector_draw_transform(transform_t *transform) {
  ImGui::PushItemWidth(ImGui::GetWindowWidth() - ImGui::GetTreeNodeToLabelSpacing() - 80);

  if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding)) {

    vector3_t local_position = transform->local_position;

    if (ImGui::DragFloat3("Position", (float *)&local_position, 0.1F, 0.0F, 0.0F, "%.3F")) {
      transform_set_position(transform, local_position);
    }

    quaternion_t local_rotation = transform->local_rotation;

    vector3_t euler_angles_rad = quaternion_to_euler_angles(local_rotation);
    vector3_t euler_angles_deg = {
      rad_to_deg(euler_angles_rad.x),
      rad_to_deg(euler_angles_rad.y),
      rad_to_deg(euler_angles_rad.z)};

    if (ImGui::DragFloat3("Rotation", (float *)&euler_angles_deg, 0.1F, 0.0F, 0.0F, "%.3F", 0)) {
      euler_angles_rad.x = deg_to_rad(euler_angles_deg.x);
      euler_angles_rad.y = deg_to_rad(euler_angles_deg.y);
      euler_angles_rad.z = deg_to_rad(euler_angles_deg.z);

      transform_set_euler_angles(transform, euler_angles_rad);
    }

    vector3_t local_scale = transform->local_scale;

    if (ImGui::DragFloat3("Scale", (float *)&local_scale, 0.1F, 0.0F, 0.0F, "%.3F", 0)) {
      transform_set_scale(transform, local_scale);
    }

    ImGui::TreePop();
  }

  ImGui::PopItemWidth();
}
