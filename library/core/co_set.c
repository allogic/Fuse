#include <library/lb_pch.h>

#include <library/core/co_config.h>
#include <library/core/co_macros.h>
#include <library/core/co_heap.h>
#include <library/core/co_set.h>

void *set_entry_key(set_entry_t *entry) {
  return entry->key;
}
uint64_t set_entry_key_size(set_entry_t *entry) {
  return entry->key_size;
}
set_entry_t *set_entry_next(set_entry_t *entry) {
  return entry->next;
}

set_t set_create(void) {
  set_t set = {0};

  set.table = (set_entry_t **)heap_alloc(SET_TABLE_COUNT * sizeof(set_entry_t *));
  set.table_size = SET_TABLE_COUNT * sizeof(set_entry_t *);
  set.table_count = SET_TABLE_COUNT;
  set.entry_count = 0;
  memset(set.table, 0, SET_TABLE_COUNT * sizeof(set_entry_t *));

  return set;
}
set_t set_copy(set_t *reference) {
  set_t set = {0};

  set.table = (set_entry_t **)heap_alloc(reference->table_size);
  set.table_size = reference->table_size;
  set.table_count = reference->table_count;
  set.entry_count = reference->entry_count;
  memset(set.table, 0, reference->table_size);

  uint64_t table_index = 0;
  while (table_index < set.table_count) {
    set_entry_t *curr_reference = reference->table[table_index];
    set_entry_t *curr = set.table[table_index];

    if (curr_reference) {
      uint64_t hash = set_hash(&set, curr_reference->key, curr_reference->key_size, set.table_count);

      curr = (set_entry_t *)heap_alloc(sizeof(set_entry_t));

      memset(curr, 0, sizeof(set_entry_t));

      curr->next = set.table[hash];
      curr->key = (uint8_t *)heap_alloc(curr_reference->key_size);
      curr->key_size = curr_reference->key_size;

      memcpy(curr->key, curr_reference->key, curr_reference->key_size);

      set.table[hash] = curr;
    }

    table_index++;
  }

  return set;
}
uint8_t set_equal(set_t *set, set_t *reference) {
  uint8_t not_equal = 0;

  not_equal |= set->entry_count != reference->entry_count;

  if (set->entry_count == reference->entry_count) {
    uint64_t table_index = 0;
    while (table_index < set->table_count) {
      set_entry_t *curr_reference = reference->table[table_index];
      set_entry_t *curr = set->table[table_index];

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

  set_entry_t *curr = set->table[hash];
  while (curr) {
    if (memcmp(curr->key, key, MIN(curr->key_size, key_size)) == 0) {
      key_exists = 1;

      break;
    }

    curr = curr->next;
  }

  if (key_exists == 0) {
    curr = (set_entry_t *)heap_alloc(sizeof(set_entry_t));

    memset(curr, 0, sizeof(set_entry_t));

    curr->next = set->table[hash];
    curr->key = (uint8_t *)heap_alloc(key_size);
    curr->key_size = key_size;
    memcpy(curr->key, key, key_size);

    set->table[hash] = curr;
    set->entry_count++;
  }

  return key_exists;
}
uint8_t set_remove(set_t *set, void const *key, uint64_t key_size) {
  uint64_t hash = set_hash(set, key, key_size, set->table_count);

  set_entry_t *curr = set->table[hash];
  set_entry_t *prev = 0;
  while (curr) {
    if (memcmp(curr->key, key, MIN(curr->key_size, key_size)) == 0) {
      if (prev) {
        prev->next = curr->next;
      } else {
        set->table[hash] = curr->next;
      }

      heap_free(curr->key);
      heap_free(curr);

      set->entry_count--;

      return 1;
    }

    prev = curr;
    curr = curr->next;
  }

  return 0;
}
uint8_t set_contains(set_t *set, void const *key, uint64_t key_size) {
  uint64_t hash = set_hash(set, key, key_size, set->table_count);

  set_entry_t *curr = set->table[hash];
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
  return set->entry_count;
}
set_entry_t *set_table_at(set_t *set, uint64_t index) {
  return set->table[index];
}
void set_expand(set_t *set) {
  uint64_t table_index = 0;
  uint64_t table_size = set->table_size * 2;
  uint64_t table_count = set->table_count * 2;

  set_entry_t **table = (set_entry_t **)heap_alloc(table_size);

  memset(table, 0, table_size);

  while (table_index < set->table_count) {
    set_entry_t *curr = set->table[table_index];
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
    set_entry_t *curr = set->table[table_index];
    while (curr) {
      set_entry_t *tmp = curr;

      curr = curr->next;

      heap_free(tmp->key);
      heap_free(tmp);
    }

    table_index++;
  }

  memset(set->table, 0, set->table_size);

  set->entry_count = 0;
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
  return (uint8_t)(((set->entry_count + 1) / set->table_count) * 100);
}
void set_destroy(set_t *set) {
  uint64_t table_index = 0;
  while (table_index < set->table_count) {
    set_entry_t *curr = set->table[table_index];
    while (curr) {
      set_entry_t *tmp = curr;

      curr = curr->next;

      heap_free(tmp->key);
      heap_free(tmp);
    }

    table_index++;
  }

  heap_free(set->table);
}
