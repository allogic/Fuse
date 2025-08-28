#include <library/core/api.h>

#include <engine/context.h>
#include <engine/engine.h>

#include <engine/renderer/api.h>

player_t *g_player_0 = {0};

int main(int argc, char **argv, char **envp) {
  context_create(1920, 1080);

  g_player_0 = player_create();

  transform_set_position_xyz(g_player_0->transform, 0.0F, 0.0F, -10.0F);

  while (context_is_running()) {
    context_begin_frame();

    player_update(g_player_0);

    // renderer_update();
    // renderer_draw(g_player_0->transform, g_player_0->camera);

    context_end_frame();
  }

  player_destroy(g_player_0);

  context_destroy();

  heap_reset();

  return 0;
}
