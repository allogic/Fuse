#include <engine/eg_pch.h>
#include <engine/eg_context.h>
#include <engine/eg_engine.h>
#include <engine/eg_renderer.h>

#include <engine/scene/eg_game.h>

int32_t main(int32_t argc, char **argv, char **envp) {
  g_renderer_enable_debug = 1; // TODO

  context_create(1920, 1080);

  g_scene_curr_active = game_create();

  while (context_is_running()) {
    context_begin_frame();

    game_update(g_scene_curr_active);

    renderer_update();
    renderer_draw();

    context_end_frame();
  }

  game_destroy(g_scene_curr_active);

  context_destroy();

  heap_reset();

  return 0;
}
