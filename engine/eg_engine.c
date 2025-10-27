#include <engine/eg_pch.h>
#include <engine/eg_context.h>
#include <engine/eg_engine.h>
#include <engine/eg_renderer.h>

#include <engine/scene/eg_game.h>

int32_t main(int32_t argc, char **argv, char **envp) {
  context_t *context = context_create(1920, 1080, 1);

  // TODO: move scene stuff somewhere else
  context->renderer->scene = game_create(context);

  while (context_is_running(context)) {
    context_begin_frame(context);

    game_update(context->renderer->scene);

    // TODO: remove these render calls, inline them into the context itself..
    renderer_update(context->renderer);
    renderer_draw(context->renderer);

    context_end_frame(context);
  }

  game_destroy(context->renderer->scene);

  context_destroy(context);

  heap_reset();

  return 0;
}
