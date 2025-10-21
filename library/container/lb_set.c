#include <library/lb_pch.h>

#include <library/core/lb_api.h>

#include <library/container/lb_config.h>
#include <library/container/lb_macros.h>
#include <library/container/lb_set.h>

void *set_record_key(set_record_t *record) {
  return record->key;
}
uint64_t set_record_key_size(set_record_t *record) {
  return record->key_size;
}
set_record_t *set_record_next(set_record_t *record) {
  return record->next;
}

set_t set_create(void) {
  set_t set = {0};

  set.table = (set_record_t **)heap_alloc(SET_TABLE_COUNT * sizeof(set_record_t *), 1, 0);
  set.table_size = SET_TABLE_COUNT * sizeof(set_record_t *);
  set.table_count = SET_TABLE_COUNT;
  set.record_count = 0;

  return set;
}
set_t set_copy(set_t *reference) {
  set_t set = {0};

  set.table = (set_record_t **)heap_alloc(reference->table_size, 1, 0);
  set.table_size = reference->table_size;
  set.table_count = reference->table_count;
  set.record_count = reference->record_count;

  uint64_t table_index = 0;

  while (table_index < set.table_count) {

    set_record_t *curr_reference = reference->table[table_index];
    set_record_t *curr = set.table[table_index];

    if (curr_reference) {

      uint64_t hash = set_hash(&set, curr_reference->key, curr_reference->key_size, set.table_count);

      curr = (set_record_t *)heap_alloc(sizeof(set_record_t), 1, 0);

      curr->next = set.table[hash];
      curr->key = (uint8_t *)heap_alloc(curr_reference->key_size, 0, curr_reference->key);
      curr->key_size = curr_reference->key_size;

      set.table[hash] = curr;
    }

    table_index++;
  }

  return set;
}
uint8_t set_equal(set_t *set, set_t *reference) {
  uint8_t not_equal = 0;

  not_equal |= set->record_count != reference->record_count;

  if (set->record_count == reference->record_count) {

    uint64_t table_index = 0;

    while (table_index < set->table_count) {

      set_record_t *curr_reference = reference->table[table_index];
      set_record_t *curr = set->table[table_index];

      not_equal |= (curr_reference == 0) != (curr == 0);

      while (curr && curr_reference) {
        not_equal |= curr->key_size != curr_reference->key_size;

        if (not_equal == 0) {
          not_equal |= memcmp(curr->key, curr_reference->key, MIN(curr->key_size, curr_reference->key_size));
        }

        curr_reference = curr_reference->next;
        curr = curr->next;
      }

      table_index++;
    }
  }

  return not_equal == 0;
}
uint8_t set_insert(set_t *set, void const *key, uint64_t key_size) {
  uint8_t key_exists = 0;
  uint8_t load_factor = set_load_factor(set);

  if (load_factor > SET_LOAD_FACTOR) {
    set_expand(set);
  }

  uint64_t hash = set_hash(set, key, key_size, set->table_count);

  set_record_t *curr = set->table[hash];

  while (curr) {
    if (memcmp(curr->key, key, MIN(curr->key_size, key_size)) == 0) {
      key_exists = 1;

      break;
    }

    curr = curr->next;
  }

  if (key_exists == 0) {

    curr = (set_record_t *)heap_alloc(sizeof(set_record_t), 1, 0);

    curr->next = set->table[hash];
    curr->key = (uint8_t *)heap_alloc(key_size, 0, key);
    curr->key_size = key_size;

    set->table[hash] = curr;
    set->record_count++;
  }

  return key_exists;
}
uint8_t set_remove(set_t *set, void const *key, uint64_t key_size) {
  uint64_t hash = set_hash(set, key, key_size, set->table_count);

  set_record_t *curr = set->table[hash];
  set_record_t *prev = 0;

  while (curr) {

    if (memcmp(curr->key, key, MIN(curr->key_size, key_size)) == 0) {

      if (prev) {
        prev->next = curr->next;
      } else {
        set->table[hash] = curr->next;
      }

      heap_free(curr->key);
      heap_free(curr);

      set->record_count--;

      return 1;
    }

    prev = curr;
    curr = curr->next;
  }

  return 0;
}
uint8_t set_contains(set_t *set, void const *key, uint64_t key_size) {
  uint64_t hash = set_hash(set, key, key_size, set->table_count);

  set_record_t *curr = set->table[hash];

  while (curr) {

    if (memcmp(curr->key, key, MIN(curr->key_size, key_size)) == 0) {
      return 1;
    }

    curr = curr->next;
  }

  return 0;
}
uint64_t set_table_size(set_t *set) {
  return set->table_size;
}
uint64_t set_table_count(set_t *set) {
  return set->table_count;
}
uint64_t set_count(set_t *set) {
  return set->record_count;
}
set_record_t *set_table_at(set_t *set, uint64_t index) {
  return set->table[index];
}
void set_expand(set_t *set) {
  uint64_t table_index = 0;
  uint64_t table_size = set->table_size * 2;
  uint64_t table_count = set->table_count * 2;

  set_record_t **table = (set_record_t **)heap_alloc(table_size, 1, 0);

  while (table_index < set->table_count) {

    set_record_t *curr = set->table[table_index];

    while (curr) {

      uint64_t hash = set_hash(set, curr->key, curr->key_size, table_count);

      curr->next = table[hash];
      table[hash] = curr;
      curr = curr->next;
    }

    table_index++;
  }

  heap_free(set->table);

  set->table = table;
  set->table_size = table_size;
  set->table_count = table_count;
}
void set_clear(set_t *set) {
  uint64_t table_index = 0;

  while (table_index < set->table_count) {

    set_record_t *curr = set->table[table_index];

    while (curr) {

      set_record_t *tmp = curr;

      curr = curr->next;

      heap_free(tmp->key);
      heap_free(tmp);
    }

    table_index++;
  }

  memset(set->table, 0, set->table_size);

  set->record_count = 0;
}
uint64_t set_hash(set_t *set, void const *key, uint64_t key_size, uint64_t modulus) {
  uint64_t hash = SET_HASH_VALUE;
  uint64_t key_index = 0;

  while (key_index < key_size) {

    hash = ((hash << 5) + hash) + *(((uint8_t *)key) + key_index);

    key_index++;
  }

  return hash % modulus;
}
uint8_t set_load_factor(set_t *set) {
  return (uint8_t)(((set->record_count + 1) / set->table_count) * 100);
}
void set_destroy(set_t *set) {
  uint64_t table_index = 0;

  while (table_index < set->table_count) {

    set_record_t *curr = set->table[table_index];

    while (curr) {

      set_record_t *tmp = curr;

      curr = curr->next;

      heap_free(tmp->key);
      heap_free(tmp);
    }

    table_index++;
  }

  heap_free(set->table);
}
