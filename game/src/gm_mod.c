#include <game/gm_pch.h>
#include <game/gm_mod.h>

typedef struct character_t {
  float hp;
  float xp;
  float dexterity_over_time;
  float tenecity_over_distance;
  float base_holy_dmg;
} character_t;

static character_t on_create() {
  character_t character = {
    .hp = 100.0F,
    .xp = 0.0F,
    .dexterity_over_time = 1.2F,
    .tenecity_over_distance = 1.0F,
    .base_holy_dmg = 666.0F,
  };

  return character;
}
static void on_damage(character_t a, character_t b) {
  a.hp -= b.base_holy_dmg * (b.dexterity_over_time - b.tenecity_over_distance);
}

// ENGINE_HOOK_CREATE(on_create)
// ENGINE_HOOK_DAMAGE(on_damage)
