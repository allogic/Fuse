#include <string.h>

#include <library/core/co_config.h>
#include <library/core/co_macros.h>
#include <library/core/co_heap.h>
#include <library/core/co_map.h>

map_t map_create(void) {
  map_t map = {0};

  map.table = (map_record_t **)heap_alloc(MAP_TABLE_COUNT * sizeof(map_record_t *));
  map.table_size = MAP_TABLE_COUNT * sizeof(map_record_t *);
  map.table_count = MAP_TABLE_COUNT;
  map.record_count = 0;
  memset(map.table, 0, MAP_TABLE_COUNT * sizeof(map_record_t *));

  return map;
}
map_t map_copy(map_t *reference) {
  map_t map = {0};

  map.table = (map_record_t **)heap_alloc(reference->table_size);
  map.table_size = reference->table_size;
  map.table_count = reference->table_count;
  map.record_count = reference->record_count;
  memset(map.table, 0, reference->table_size);

  uint64_t table_index = 0;
  while (table_index < map.table_count) {
    map_record_t *curr_reference = reference->table[table_index];
    map_record_t *curr = map.table[table_index];

    if (curr_reference) {
      uint64_t hash = map_hash(&map, curr_reference->key, curr_reference->key_size, map.table_count);

      curr = (map_record_t *)heap_alloc(sizeof(map_record_t));

      memset(curr, 0, sizeof(map_record_t));

      curr->next = map.table[hash];
      curr->key = (uint8_t *)heap_alloc(curr_reference->key_size);
      curr->value = (uint8_t *)heap_alloc(curr_reference->value_size);
      curr->key_size = curr_reference->key_size;
      curr->value_size = curr_reference->value_size;

      memcpy(curr->key, curr_reference->key, curr_reference->key_size);
      memcpy(curr->value, curr_reference->value, curr_reference->value_size);

      map.table[hash] = curr;
    }

    table_index++;
  }

  return map;
}
uint8_t map_equal(map_t *map, map_t *reference) {
  uint8_t not_equal = 0;

  not_equal |= map->record_count != reference->record_count;

  if (map->record_count == reference->record_count) {
    uint64_t table_index = 0;
    while (table_index < map->table_count) {
      map_record_t *curr_reference = reference->table[table_index];
      map_record_t *curr = map->table[table_index];

      not_equal |= (curr_reference == 0) != (curr == 0);

      while (curr && curr_reference) {
        not_equal |= curr->key_size != curr_reference->key_size;
        not_equal |= curr->value_size != curr_reference->value_size;

        if (not_equal == 0) {
          not_equal |= memcmp(curr->key, curr_reference->key, MIN(curr->key_size, curr_reference->key_size));
          not_equal |= memcmp(curr->value, curr_reference->value, MIN(curr->value_size, curr_reference->value_size));
        }

        curr_reference = curr_reference->next;
        curr = curr->next;
      }

      table_index++;
    }
  }

  return not_equal == 0;
}
uint8_t map_insert(map_t *map, void const *key, uint64_t key_size, void const *value, uint64_t value_size) {
  uint8_t key_exists = 0;
  uint8_t load_factor = map_load_factor(map);

  if (load_factor > MAP_LOAD_FACTOR) {
    map_expand(map);
  }

  uint64_t hash = map_hash(map, key, key_size, map->table_count);

  map_record_t *curr = map->table[hash];
  while (curr) {
    if (memcmp(curr->key, key, MIN(curr->key_size, key_size)) == 0) {
      key_exists = 1;

      break;
    }

    curr = curr->next;
  }

  if (key_exists == 0) {
    curr = (map_record_t *)heap_alloc(sizeof(map_record_t));

    memset(curr, 0, sizeof(map_record_t));

    curr->next = map->table[hash];
    curr->key = (uint8_t *)heap_alloc(key_size);

    if (value) {
      curr->value = (uint8_t *)heap_alloc(value_size);
    }

    curr->key_size = key_size;

    memcpy(curr->key, key, key_size);

    if (value) {
      memcpy(curr->value, value, value_size);
    }

    curr->value_size = value_size;

    map->table[hash] = curr;
    map->record_count++;
  }

  return key_exists;
}
uint8_t map_remove(map_t *map, void const *key, uint64_t key_size, void *value, uint64_t value_size) {
  uint64_t hash = map_hash(map, key, key_size, map->table_count);

  map_record_t *curr = map->table[hash];
  map_record_t *prev = 0;
  while (curr) {
    if (memcmp(curr->key, key, MIN(curr->key_size, key_size)) == 0) {
      if (prev) {
        prev->next = curr->next;
      } else {
        map->table[hash] = curr->next;
      }

      if (value) {
        memcpy(value, curr->value, MIN(value_size, curr->value_size));
      }

      heap_free(curr->key);
      heap_free(curr->value);
      heap_free(curr);

      map->record_count--;

      return 1;
    }

    prev = curr;
    curr = curr->next;
  }

  return 0;
}
uint8_t map_contains(map_t *map, void const *key, uint64_t key_size) {
  uint64_t hash = map_hash(map, key, key_size, map->table_count);

  map_record_t *curr = map->table[hash];
  while (curr) {
    if (memcmp(curr->key, key, MIN(curr->key_size, key_size)) == 0) {
      return 1;
    }

    curr = curr->next;
  }

  return 0;
}
uint64_t map_count(map_t *map) {
  return map->record_count;
}
void *map_at(map_t *map, void const *key, uint64_t key_size) {
  uint64_t hash = map_hash(map, key, key_size, map->table_count);

  map_record_t *curr = map->table[hash];
  while (curr) {
    if (memcmp(curr->key, key, MIN(curr->key_size, key_size)) == 0) {
      return curr->value;
    }

    curr = curr->next;
  }

  return 0;
}
void map_expand(map_t *map) {
  uint64_t table_index = 0;
  uint64_t table_size = map->table_size * 2;
  uint64_t table_count = map->table_count * 2;

  map_record_t **table = (map_record_t **)heap_alloc(table_size);

  memset(table, 0, table_size);

  while (table_index < map->table_count) {
    map_record_t *curr = map->table[table_index];
    while (curr) {
      uint64_t hash = map_hash(map, curr->key, curr->key_size, table_count);

      curr->next = table[hash];
      table[hash] = curr;
      curr = curr->next;
    }

    table_index++;
  }

  heap_free(map->table);

  map->table = table;
  map->table_size = table_size;
  map->table_count = table_count;
}
void map_clear(map_t *map) {
  uint64_t table_index = 0;
  while (table_index < map->table_count) {
    map_record_t *curr = map->table[table_index];
    while (curr) {
      map_record_t *tmp = curr;

      curr = curr->next;

      heap_free(tmp->key);
      heap_free(tmp->value);
      heap_free(tmp);
    }

    table_index++;
  }

  memset(map->table, 0, map->table_size);

  map->record_count = 0;
}
uint64_t map_hash(map_t *map, void const *key, uint64_t key_size, uint64_t modulus) {
  uint64_t hash = MAP_HASH_VALUE;
  uint64_t key_index = 0;

  while (key_index < key_size) {
    hash = ((hash << 5) + hash) + *(((uint8_t *)key) + key_index);

    key_index++;
  }

  return hash % modulus;
}
uint8_t map_load_factor(map_t *map) {
  return (uint8_t)(((map->record_count + 1) / map->table_count) * 100);
}
void map_destroy(map_t *map) {
  uint64_t table_index = 0;
  while (table_index < map->table_count) {
    map_record_t *curr = map->table[table_index];
    while (curr) {
      map_record_t *tmp = curr;

      curr = curr->next;

      heap_free(tmp->key);
      heap_free(tmp->value);
      heap_free(tmp);
    }

    table_index++;
  }

  heap_free(map->table);
}

void *map_record_key(map_record_t *record) {
  return record->key;
}
uint64_t map_record_key_size(map_record_t *record) {
  return record->key_size;
}
void *map_record_value(map_record_t *record) {
  return record->value;
}
uint64_t map_record_value_size(map_record_t *record) {
  return record->value_size;
}

map_iter_t map_iter_create_from(map_t *map) {
  map_iter_t iter = {0};

  iter.table = map->table;
  iter.table_count = map->table_count;

  uint64_t table_index = 0;
  while (table_index < map->table_count) {

    map_record_t *curr = map->table[table_index];
    if (curr) {
      iter.table_index = table_index;
      iter.table_record = curr;

      break;
    }

    table_index++;
  }

  return iter;
}
uint8_t map_iter_valid(map_iter_t *iter) {
  return (iter->table_index < iter->table_count) && iter->table_record;
}
void map_iter_advance(map_iter_t *iter) {
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
}
void *map_iter_key(map_iter_t *iter) {
  return iter->table_record->key;
}
uint64_t map_iter_key_size(map_iter_t *iter) {
  return iter->table_record->key_size;
}
void *map_iter_value(map_iter_t *iter) {
  return iter->table_record->value;
}
uint64_t map_iter_value_size(map_iter_t *iter) {
  return iter->table_record->value_size;
}
