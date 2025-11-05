#include <game/gm_pch.h>
#include <game/gm_main.h>

int32_t main(int32_t argc, char **argv) {
  context_t *context = context_create(1920, 1080, 0);

  // TODO: move scene stuff somewhere else
  context->scene = scene_create(context);

  context_run(context);

  scene_destroy(context->scene);

  context_destroy(context);

  heap_reset();

  return 0;
}
