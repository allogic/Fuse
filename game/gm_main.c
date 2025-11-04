#include <game/gm_pch.h>
#include <game/gm_main.h>

int32_t main(int32_t argc, char **argv) {
  context_t *context = context_create(1920, 1080, 0);

  // TODO: move scene stuff somewhere else
  context->scene = scene_create(context);

  while (context_is_running(context)) {

    context_begin_frame(context);

    scene_update(context->scene);

    // TODO: remove these render calls, inline them into the context itself..
    renderer_update(context->renderer);
    renderer_draw(context->renderer);

    context_end_frame(context);
  }

  scene_destroy(context->scene);

  context_destroy(context);

  heap_reset();

  return 0;
}
