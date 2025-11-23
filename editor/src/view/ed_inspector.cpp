#include <editor/ed_pch.h>
#include <editor/ed_main.h>
#include <editor/ed_titlebar.h>

#include <editor/view/ed_inspector.h>
#include <editor/view/ed_hierarchy.h>

static void ed_inspector_view_draw_camera(eg_camera_t *camera);
static void ed_inspector_view_draw_editor_controller(eg_editor_controller_t *editor_controller);
static void ed_inspector_view_draw_rigidbody(eg_rigidbody_t *rigidbody);
static void ed_inspector_view_draw_transform(eg_transform_t *transform);

ed_inspector_view_t *ed_inspector_view_create(eg_context_t *context) {
  ed_inspector_view_t *inspector = (ed_inspector_view_t *)heap_alloc(sizeof(ed_inspector_view_t), 1, 0);

  inspector->base.context = context;
  inspector->base.is_dirty = 0;
  inspector->base.is_open = 1;
  inspector->base.is_docked = 0;

  return inspector;
}
void ed_inspector_view_refresh(ed_inspector_view_t *inspector) {
  // TODO
}
void ed_inspector_view_draw(ed_inspector_view_t *inspector) {
  switch (g_dockspace_selected_type) {
    case ED_DOCKSPACE_TYPE_SCENE: {

      ecs_world_t *world = inspector->base.context->scene->world;

      if (ecs_is_valid(world, g_hierarchy_scene->selected_entity)) {

        eg_camera_t *camera = eg_scene_camera_mut(inspector->base.context->scene, g_hierarchy_scene->selected_entity);

        if (camera) {
          ed_inspector_view_draw_camera(camera);
        }

        eg_editor_controller_t *editor_controller = eg_scene_editor_controller_mut(inspector->base.context->scene, g_hierarchy_scene->selected_entity);

        if (editor_controller) {
          ed_inspector_view_draw_editor_controller(editor_controller);
        }

        eg_rigidbody_t *rigidbody = eg_scene_rigidbody_mut(inspector->base.context->scene, g_hierarchy_scene->selected_entity);

        if (rigidbody) {
          ed_inspector_view_draw_rigidbody(rigidbody);
        }

        eg_transform_t *transform = eg_scene_transform_mut(inspector->base.context->scene, g_hierarchy_scene->selected_entity);

        if (transform) {
          ed_inspector_view_draw_transform(transform);
        }
      }

      break;
    }
  }
}
void ed_inspector_view_destroy(ed_inspector_view_t *inspector) {
  heap_free(inspector);
}

static void ed_inspector_view_draw_camera(eg_camera_t *camera) {
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
static void ed_inspector_view_draw_editor_controller(eg_editor_controller_t *editor_controller) {
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
static void ed_inspector_view_draw_rigidbody(eg_rigidbody_t *rigidbody) {
  ImGui::PushItemWidth(ImGui::GetWindowWidth() - ImGui::GetTreeNodeToLabelSpacing() - 70);

  if (ImGui::TreeNodeEx("Rigidbody", ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding)) {

    // TODO

    ImGui::TreePop();
  }

  ImGui::PopItemWidth();
}
static void ed_inspector_view_draw_transform(eg_transform_t *transform) {
  ImGui::PushItemWidth(ImGui::GetWindowWidth() - ImGui::GetTreeNodeToLabelSpacing() - 80);

  if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding)) {

    vector3_t local_position = transform->local_position;

    if (ImGui::DragFloat3("Position", (float *)&local_position, 0.1F, 0.0F, 0.0F, "%.3F")) {
      eg_transform_set_position(transform, local_position);
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

      eg_transform_set_euler_angles(transform, euler_angles_rad);
    }

    vector3_t local_scale = transform->local_scale;

    if (ImGui::DragFloat3("Scale", (float *)&local_scale, 0.1F, 0.0F, 0.0F, "%.3F", 0)) {
      eg_transform_set_scale(transform, local_scale);
    }

    ImGui::TreePop();
  }

  ImGui::PopItemWidth();
}
