#include <game/gm_pch.h>
#include <game/gm_main.h>

int32_t main(int32_t argc, char **argv) {

  eg_context_create(1920, 1080, 0);
  eg_context_run();
  eg_context_destroy();

  eg_heap_reset();

  return 0;
}
