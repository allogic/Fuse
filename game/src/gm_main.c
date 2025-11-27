#include <game/gm_pch.h>
#include <game/gm_main.h>

int32_t main(int32_t argc, char **argv) {
  eg_context_t *context = eg_context_create(1920, 1080, 0);

  // TODO: move scene stuff somewhere else
  context->scene = eg_scene_create(context);

  eg_context_run(context);

  eg_scene_destroy(context->scene);

  eg_context_destroy(context);

  heap_reset();

  return 0;
}
