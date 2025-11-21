#include <library/container/lb_pch.h>
#include <library/container/lb_config.h>
#include <library/container/lb_macros.h>
#include <library/container/lb_map.h>

lb_map_t lb_map_create(void) {
  lb_map_t map = {0};

  map.table = (lb_map_record_t **)lb_heap_alloc(LB_MAP_TABLE_COUNT * sizeof(lb_map_record_t *), 1, 0);
  map.table_size = LB_MAP_TABLE_COUNT * sizeof(lb_map_record_t *);
  map.table_count = LB_MAP_TABLE_COUNT;
  map.record_count = 0;

  return map;
}
lb_map_t lb_map_copy(lb_map_t *reference) {
  lb_map_t map = {0};

  map.table = (lb_map_record_t **)lb_heap_alloc(reference->table_size, 1, 0);
  map.table_size = reference->table_size;
  map.table_count = reference->table_count;
  map.record_count = reference->record_count;

  uint64_t table_index = 0;
  while (table_index < map.table_count) {
    lb_map_record_t *curr_reference = reference->table[table_index];
    lb_map_record_t *curr = map.table[table_index];

    if (curr_reference) {
      uint64_t hash = lb_map_hash(&map, curr_reference->key, curr_reference->key_size, map.table_count);

      curr = (lb_map_record_t *)lb_heap_alloc(sizeof(lb_map_record_t), 1, 0);

      curr->next = map.table[hash];
      curr->key = (uint8_t *)lb_heap_alloc(curr_reference->key_size, 0, curr_reference->key);
      curr->value = (uint8_t *)lb_heap_alloc(curr_reference->value_size, 0, curr_reference->value);
      curr->key_size = curr_reference->key_size;
      curr->value_size = curr_reference->value_size;

      map.table[hash] = curr;
    }

    table_index++;
  }

  return map;
}
uint8_t lb_map_equal(lb_map_t *map, lb_map_t *reference) {
  uint8_t not_equal = 0;

  not_equal |= map->record_count != reference->record_count;

  if (map->record_count == reference->record_count) {
    uint64_t table_index = 0;
    while (table_index < map->table_count) {
      lb_map_record_t *curr_reference = reference->table[table_index];
      lb_map_record_t *curr = map->table[table_index];

      not_equal |= (curr_reference == 0) != (curr == 0);

      while (curr && curr_reference) {
        not_equal |= curr->key_size != curr_reference->key_size;
        not_equal |= curr->value_size != curr_reference->value_size;

        if (not_equal == 0) {
          not_equal |= memcmp(curr->key, curr_reference->key, LB_MIN(curr->key_size, curr_reference->key_size));
          not_equal |= memcmp(curr->value, curr_reference->value, LB_MIN(curr->value_size, curr_reference->value_size));
        }

        curr_reference = curr_reference->next;
        curr = curr->next;
      }

      table_index++;
    }
  }

  return not_equal == 0;
}
uint8_t lb_map_insert(lb_map_t *map, void const *key, uint64_t key_size, void const *value, uint64_t value_size) {
  uint8_t key_exists = 0;
  uint8_t load_factor = lb_map_load_factor(map);

  if (load_factor > LB_MAP_LOAD_FACTOR) {
    lb_map_expand(map);
  }

  uint64_t hash = lb_map_hash(map, key, key_size, map->table_count);

  lb_map_record_t *curr = map->table[hash];

  while (curr) {

    if (memcmp(curr->key, key, LB_MIN(curr->key_size, key_size)) == 0) {
      key_exists = 1;

      break;
    }

    curr = curr->next;
  }

  if (key_exists == 0) {
    curr = (lb_map_record_t *)lb_heap_alloc(sizeof(lb_map_record_t), 1, 0);

    curr->next = map->table[hash];
    curr->key = (uint8_t *)lb_heap_alloc(key_size, 0, key);
    curr->key_size = key_size;
    curr->value = (uint8_t *)lb_heap_alloc(value_size, 0, value);
    curr->value_size = value_size;

    map->table[hash] = curr;
    map->record_count++;
  }

  return key_exists;
}
uint8_t lb_map_remove(lb_map_t *map, void const *key, uint64_t key_size, void *value, uint64_t value_size) {
  uint64_t hash = lb_map_hash(map, key, key_size, map->table_count);

  lb_map_record_t *curr = map->table[hash];
  lb_map_record_t *prev = 0;

  while (curr) {

    if (memcmp(curr->key, key, LB_MIN(curr->key_size, key_size)) == 0) {

      if (prev) {
        prev->next = curr->next;
      } else {
        map->table[hash] = curr->next;
      }

      if (value) {
        memcpy(value, curr->value, LB_MIN(value_size, curr->value_size));
      }

      lb_heap_free(curr->key);
      lb_heap_free(curr->value);
      lb_heap_free(curr);

      map->record_count--;

      return 1;
    }

    prev = curr;
    curr = curr->next;
  }

  return 0;
}
uint8_t lb_map_contains(lb_map_t *map, void const *key, uint64_t key_size) {
  uint64_t hash = lb_map_hash(map, key, key_size, map->table_count);

  lb_map_record_t *curr = map->table[hash];

  while (curr) {

    if (memcmp(curr->key, key, LB_MIN(curr->key_size, key_size)) == 0) {
      return 1;
    }

    curr = curr->next;
  }

  return 0;
}
uint64_t lb_map_count(lb_map_t *map) {
  return map->record_count;
}
void *lb_map_at(lb_map_t *map, void const *key, uint64_t key_size) {
  uint64_t hash = lb_map_hash(map, key, key_size, map->table_count);

  lb_map_record_t *curr = map->table[hash];

  while (curr) {

    if (memcmp(curr->key, key, LB_MIN(curr->key_size, key_size)) == 0) {
      return curr->value;
    }

    curr = curr->next;
  }

  return 0;
}
void lb_map_expand(lb_map_t *map) {
  uint64_t table_index = 0;
  uint64_t table_size = map->table_size * 2;
  uint64_t table_count = map->table_count * 2;

  lb_map_record_t **table = (lb_map_record_t **)lb_heap_alloc(table_size, 1, 0);

  while (table_index < map->table_count) {

    lb_map_record_t *curr = map->table[table_index];

    while (curr) {
      uint64_t hash = lb_map_hash(map, curr->key, curr->key_size, table_count);

      curr->next = table[hash];
      table[hash] = curr;
      curr = curr->next;
    }

    table_index++;
  }

  lb_heap_free(map->table);

  map->table = table;
  map->table_size = table_size;
  map->table_count = table_count;
}
void lb_map_clear(lb_map_t *map) {
  uint64_t table_index = 0;

  while (table_index < map->table_count) {

    lb_map_record_t *curr = map->table[table_index];

    while (curr) {

      lb_map_record_t *tmp = curr;

      curr = curr->next;

      lb_heap_free(tmp->key);
      lb_heap_free(tmp->value);
      lb_heap_free(tmp);
    }

    table_index++;
  }

  memset(map->table, 0, map->table_size);

  map->record_count = 0;
}
lb_map_iter_t lb_map_iter(lb_map_t *map) {
  lb_map_iter_t iter = {0};

  iter.table = map->table;
  iter.table_count = map->table_count;
  iter.first_step = 0;

  uint64_t table_index = 0;

  while (table_index < map->table_count) {

    lb_map_record_t *curr = map->table[table_index];

    if (curr) {

      iter.table_index = table_index;
      iter.table_record = curr;

      break;
    }

    table_index++;
  }

  return iter;
}
uint64_t lb_map_hash(lb_map_t *map, void const *key, uint64_t key_size, uint64_t modulus) {
  uint64_t hash = LB_MAP_HASH_POLY;
  uint64_t key_index = 0;

  while (key_index < key_size) {

    hash = ((hash << 5) + hash) + *(((uint8_t *)key) + key_index);

    key_index++;
  }

  return hash % modulus;
}
uint8_t lb_map_load_factor(lb_map_t *map) {
  return (uint8_t)(((map->record_count + 1) / map->table_count) * 100);
}
void lb_map_destroy(lb_map_t *map) {
  uint64_t table_index = 0;

  while (table_index < map->table_count) {

    lb_map_record_t *curr = map->table[table_index];

    while (curr) {

      lb_map_record_t *tmp = curr;

      curr = curr->next;

      lb_heap_free(tmp->key);
      lb_heap_free(tmp->value);
      lb_heap_free(tmp);
    }

    table_index++;
  }

  lb_heap_free(map->table);
}

void *lb_map_record_key(lb_map_record_t *record) {
  return record->key;
}
uint64_t lb_map_record_key_size(lb_map_record_t *record) {
  return record->key_size;
}
void *lb_map_record_value(lb_map_record_t *record) {
  return record->value;
}
uint64_t lb_map_record_value_size(lb_map_record_t *record) {
  return record->value_size;
}

uint8_t lb_map_iter_step(lb_map_iter_t *iter) {
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
void lb_map_iter_advance(lb_map_iter_t *iter) {
}
void *lb_map_iter_key(lb_map_iter_t *iter) {
  return iter->table_record->key;
}
uint64_t lb_map_iter_key_size(lb_map_iter_t *iter) {
  return iter->table_record->key_size;
}
void *lb_map_iter_value(lb_map_iter_t *iter) {
  return iter->table_record->value;
}
uint64_t lb_map_iter_value_size(lb_map_iter_t *iter) {
  return iter->table_record->value_size;
}
