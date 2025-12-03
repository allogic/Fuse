#include <engine/eg_pch.h>

struct eg_terrain_t {
  void *dummy;
};

eg_terrain_t *eg_terrain_create(void) {
  eg_terrain_t *terrain = (eg_terrain_t *)eg_heap_alloc(sizeof(eg_terrain_t), 1, 0);

  return terrain;
}
void eg_terrain_rebuild(eg_terrain_t *terrain) {
  // TODO
}
void eg_terrain_destroy(eg_terrain_t *terrain) {
  eg_heap_free(terrain);
}
