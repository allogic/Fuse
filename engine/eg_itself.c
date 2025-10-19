#include <engine/eg_pch.h>
#include <engine/eg_context.h>
#include <engine/eg_itself.h>
#include <engine/eg_renderer.h>

#include <engine/scenes/eg_game_scene.h>

int32_t main(int32_t argc, char **argv, char **envp) {
  g_globals.renderer_enable_debug = 1; // TODO

  context_create(1920, 1080);

  g_globals.scene_curr_active = game_scene_create();

  while (context_is_running()) {
    context_begin_frame();

    game_scene_update(g_globals.scene_curr_active);

    renderer_update();
    renderer_draw();

    context_end_frame();
  }

  game_scene_destroy(g_globals.scene_curr_active);

  context_destroy();

  heap_reset();

  return 0;
}
