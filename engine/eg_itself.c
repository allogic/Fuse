#include <engine/eg_pch.h>
#include <engine/eg_context.h>
#include <engine/eg_itself.h>
#include <engine/eg_renderer.h>
#include <engine/eg_scene_manager.h>

#include <engine/scenes/eg_game_scene.h>

int32_t main(int32_t argc, char **argv, char **envp) {
  g_globals.renderer_enable_debug = 1; // TODO

  context_create(1920, 1080);
  scene_manager_create();

  scene_t *game_scene = game_scene_create();

  scene_manager_set_active_scene(0);

  while (context_is_running()) {
    context_begin_frame();
    renderer_begin_frame();

    renderer_update();
    renderer_draw();

    renderer_end_frame();
    context_end_frame();
  }

  scene_manager_create();
  context_destroy();

  heap_reset();

  return 0;
}
