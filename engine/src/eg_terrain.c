#include <engine/eg_pch.h>
#include <engine/eg_terrain.h>

struct eg_terrain_t {
  eg_context_t *context;
};

eg_terrain_t *eg_terrain_create(eg_context_t *context) {
  eg_terrain_t *terrain = (eg_terrain_t *)lb_heap_alloc(sizeof(eg_terrain_t), 1, 0);

  terrain->context = context;

  return terrain;
}
void eg_terrain_rebuild(eg_terrain_t *terrain) {
  // TODO
}
void eg_terrain_destroy(eg_terrain_t *terrain) {
  lb_heap_free(terrain);
}
