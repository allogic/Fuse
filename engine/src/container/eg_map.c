#include <engine/eg_pch.h>

#include <engine/container/eg_map.h>

#define EG_MAP_TABLE_COUNT (128ULL)
#define EG_MAP_HASH_POLY (5381ULL)
#define EG_MAP_LOAD_FACTOR (75ULL)

struct eg_map_t {
  struct eg_map_record_t **table;
  uint64_t table_size;
  uint64_t table_count;
  uint64_t record_count;
};
struct eg_map_record_t {
  struct eg_map_record_t *next;
  uint8_t *key;
  uint64_t key_size;
  uint8_t *value;
  uint64_t value_size;
};
struct eg_map_iter_t {
  struct eg_map_record_t **table;
  struct eg_map_record_t *table_record;
  uint64_t table_index;
  uint64_t table_count;
  uint8_t first_step;
};

eg_map_t *eg_map_create(void) {
  eg_map_t *map = (eg_map_t *)eg_heap_alloc(sizeof(eg_map_t), 1, 0);

  map->table = (eg_map_record_t **)eg_heap_alloc(EG_MAP_TABLE_COUNT * sizeof(eg_map_record_t *), 1, 0);
  map->table_size = EG_MAP_TABLE_COUNT * sizeof(eg_map_record_t *);
  map->table_count = EG_MAP_TABLE_COUNT;
  map->record_count = 0;

  return map;
}
eg_map_t *eg_map_copy(eg_map_t *reference) {
  eg_map_t *map = (eg_map_t *)eg_heap_alloc(sizeof(eg_map_t), 1, 0);

  map->table = (eg_map_record_t **)eg_heap_alloc(reference->table_size, 1, 0);
  map->table_size = reference->table_size;
  map->table_count = reference->table_count;
  map->record_count = reference->record_count;

  uint64_t table_index = 0;

  while (table_index < map->table_count) {

    eg_map_record_t *curr_reference = reference->table[table_index];
    eg_map_record_t *curr = map->table[table_index];

    if (curr_reference) {
      uint64_t hash = eg_map_hash(map, curr_reference->key, curr_reference->key_size, map->table_count);

      curr = (eg_map_record_t *)eg_heap_alloc(sizeof(eg_map_record_t), 1, 0);

      curr->next = map->table[hash];
      curr->key = (uint8_t *)eg_heap_alloc(curr_reference->key_size, 0, curr_reference->key);
      curr->value = (uint8_t *)eg_heap_alloc(curr_reference->value_size, 0, curr_reference->value);
      curr->key_size = curr_reference->key_size;
      curr->value_size = curr_reference->value_size;

      map->table[hash] = curr;
    }

    table_index++;
  }

  return map;
}
uint8_t eg_map_equal(eg_map_t *map, eg_map_t *reference) {
  uint8_t not_equal = 0;

  not_equal |= map->record_count != reference->record_count;

  if (map->record_count == reference->record_count) {

    uint64_t table_index = 0;

    while (table_index < map->table_count) {

      eg_map_record_t *curr_reference = reference->table[table_index];
      eg_map_record_t *curr = map->table[table_index];

      not_equal |= (curr_reference == 0) != (curr == 0);

      while (curr && curr_reference) {

        not_equal |= curr->key_size != curr_reference->key_size;
        not_equal |= curr->value_size != curr_reference->value_size;

        if (not_equal == 0) {
          not_equal |= memcmp(curr->key, curr_reference->key, EG_MIN(curr->key_size, curr_reference->key_size));
          not_equal |= memcmp(curr->value, curr_reference->value, EG_MIN(curr->value_size, curr_reference->value_size));
        }

        curr_reference = curr_reference->next;
        curr = curr->next;
      }

      table_index++;
    }
  }

  return not_equal == 0;
}
uint8_t eg_map_insert(eg_map_t *map, void const *key, uint64_t key_size, void const *value, uint64_t value_size) {
  uint8_t key_exists = 0;
  uint8_t load_factor = eg_map_load_factor(map);

  if (load_factor > EG_MAP_LOAD_FACTOR) {
    eg_map_expand(map);
  }

  uint64_t hash = eg_map_hash(map, key, key_size, map->table_count);

  eg_map_record_t *curr = map->table[hash];

  while (curr) {

    if (memcmp(curr->key, key, EG_MIN(curr->key_size, key_size)) == 0) {
      key_exists = 1;

      break;
    }

    curr = curr->next;
  }

  if (key_exists == 0) {
    curr = (eg_map_record_t *)eg_heap_alloc(sizeof(eg_map_record_t), 1, 0);

    curr->next = map->table[hash];
    curr->key = (uint8_t *)eg_heap_alloc(key_size, 0, key);
    curr->key_size = key_size;
    curr->value = (uint8_t *)eg_heap_alloc(value_size, 0, value);
    curr->value_size = value_size;

    map->table[hash] = curr;
    map->record_count++;
  }

  return key_exists;
}
uint8_t eg_map_remove(eg_map_t *map, void const *key, uint64_t key_size, void *value, uint64_t value_size) {
  uint64_t hash = eg_map_hash(map, key, key_size, map->table_count);

  eg_map_record_t *curr = map->table[hash];
  eg_map_record_t *prev = 0;

  while (curr) {

    if (memcmp(curr->key, key, EG_MIN(curr->key_size, key_size)) == 0) {

      if (prev) {
        prev->next = curr->next;
      } else {
        map->table[hash] = curr->next;
      }

      if (value) {
        memcpy(value, curr->value, EG_MIN(value_size, curr->value_size));
      }

      eg_heap_free(curr->key);
      eg_heap_free(curr->value);
      eg_heap_free(curr);

      map->record_count--;

      return 1;
    }

    prev = curr;
    curr = curr->next;
  }

  return 0;
}
uint8_t eg_map_contains(eg_map_t *map, void const *key, uint64_t key_size) {
  uint64_t hash = eg_map_hash(map, key, key_size, map->table_count);

  eg_map_record_t *curr = map->table[hash];

  while (curr) {

    if (memcmp(curr->key, key, EG_MIN(curr->key_size, key_size)) == 0) {
      return 1;
    }

    curr = curr->next;
  }

  return 0;
}
uint64_t eg_map_count(eg_map_t *map) {
  return map->record_count;
}
void *eg_map_at(eg_map_t *map, void const *key, uint64_t key_size) {
  uint64_t hash = eg_map_hash(map, key, key_size, map->table_count);

  eg_map_record_t *curr = map->table[hash];

  while (curr) {

    if (memcmp(curr->key, key, EG_MIN(curr->key_size, key_size)) == 0) {
      return curr->value;
    }

    curr = curr->next;
  }

  return 0;
}
void eg_map_expand(eg_map_t *map) {
  uint64_t table_index = 0;
  uint64_t table_size = map->table_size * 2;
  uint64_t table_count = map->table_count * 2;

  eg_map_record_t **table = (eg_map_record_t **)eg_heap_alloc(table_size, 1, 0);

  while (table_index < map->table_count) {

    eg_map_record_t *curr = map->table[table_index];

    while (curr) {
      uint64_t hash = eg_map_hash(map, curr->key, curr->key_size, table_count);

      curr->next = table[hash];
      table[hash] = curr;
      curr = curr->next;
    }

    table_index++;
  }

  eg_heap_free(map->table);

  map->table = table;
  map->table_size = table_size;
  map->table_count = table_count;
}
void eg_map_clear(eg_map_t *map) {
  uint64_t table_index = 0;

  while (table_index < map->table_count) {

    eg_map_record_t *curr = map->table[table_index];

    while (curr) {

      eg_map_record_t *tmp = curr;

      curr = curr->next;

      eg_heap_free(tmp->key);
      eg_heap_free(tmp->value);
      eg_heap_free(tmp);
    }

    table_index++;
  }

  memset(map->table, 0, map->table_size);

  map->record_count = 0;
}
uint64_t eg_map_hash(eg_map_t *map, void const *key, uint64_t key_size, uint64_t modulus) {
  uint64_t hash = EG_MAP_HASH_POLY;
  uint64_t key_index = 0;

  while (key_index < key_size) {

    hash = ((hash << 5) + hash) + *(((uint8_t *)key) + key_index);

    key_index++;
  }

  return hash % modulus;
}
uint8_t eg_map_load_factor(eg_map_t *map) {
  return (uint8_t)(((map->record_count + 1) / map->table_count) * 100);
}
void eg_map_destroy(eg_map_t *map) {
  uint64_t table_index = 0;

  while (table_index < map->table_count) {

    eg_map_record_t *curr = map->table[table_index];

    while (curr) {

      eg_map_record_t *tmp = curr;

      curr = curr->next;

      eg_heap_free(tmp->key);
      eg_heap_free(tmp->value);
      eg_heap_free(tmp);
    }

    table_index++;
  }

  eg_heap_free(map->table);
  eg_heap_free(map);
}

void *eg_map_record_key(eg_map_record_t *record) {
  return record->key;
}
uint64_t eg_map_record_key_size(eg_map_record_t *record) {
  return record->key_size;
}
void *eg_map_record_value(eg_map_record_t *record) {
  return record->value;
}
uint64_t eg_map_record_value_size(eg_map_record_t *record) {
  return record->value_size;
}

eg_map_iter_t *eg_map_iter_create(eg_map_t *map) {
  eg_map_iter_t *iter = (eg_map_iter_t *)eg_heap_alloc(sizeof(eg_map_iter_t), 1, 0);

  iter->table = map->table;
  iter->table_count = map->table_count;
  iter->first_step = 0;

  uint64_t table_index = 0;

  while (table_index < map->table_count) {

    eg_map_record_t *curr = map->table[table_index];

    if (curr) {

      iter->table_index = table_index;
      iter->table_record = curr;

      break;
    }

    table_index++;
  }

  return iter;
}
uint8_t eg_map_iter_step(eg_map_iter_t *iter) {
  if (iter->first_step) {

    if (iter->table_record) {
      iter->table_record = iter->table_record->next;
    }

    if (iter->table_record == 0) {
      iter->table_index++;

      while (iter->table_index < iter->table_count) {

        iter->table_record = iter->table[iter->table_index];

        if (iter->table_record) {
          break;
        }

        iter->table_index++;
      }
    }
  } else {
    iter->first_step = 1;
  }

  return (iter->table_index < iter->table_count) && iter->table_record;
}
void *eg_map_iter_key(eg_map_iter_t *iter) {
  return iter->table_record->key;
}
uint64_t eg_map_iter_key_size(eg_map_iter_t *iter) {
  return iter->table_record->key_size;
}
void *eg_map_iter_value(eg_map_iter_t *iter) {
  return iter->table_record->value;
}
uint64_t eg_map_iter_value_size(eg_map_iter_t *iter) {
  return iter->table_record->value_size;
}
void eg_map_iter_destroy(eg_map_iter_t *iter) {
  eg_heap_free(iter);
}
