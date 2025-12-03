#include <engine/eg_pch.h>

#include <engine/container/eg_set.h>

#define EG_SET_TABLE_COUNT (128ULL)
#define EG_SET_HASH_POLY (5381ULL)
#define EG_SET_LOAD_FACTOR (75ULL)

struct eg_set_t {
  eg_set_record_t **table;
  uint64_t table_size;
  uint64_t table_count;
  uint64_t record_count;
};
struct eg_set_record_t {
  eg_set_record_t *next;
  uint8_t *key;
  uint64_t key_size;
};
struct eg_set_iter_t {
  eg_set_record_t **table;
  eg_set_record_t *table_record;
  uint64_t table_index;
  uint64_t table_count;
  uint8_t first_step;
};

eg_set_t *eg_set_create(void) {
  eg_set_t *set = (eg_set_t *)eg_heap_alloc(sizeof(eg_set_t), 1, 0);

  set->table = (eg_set_record_t **)eg_heap_alloc(EG_SET_TABLE_COUNT * sizeof(eg_set_record_t *), 1, 0);
  set->table_size = EG_SET_TABLE_COUNT * sizeof(eg_set_record_t *);
  set->table_count = EG_SET_TABLE_COUNT;
  set->record_count = 0;

  return set;
}
eg_set_t *eg_set_copy(eg_set_t *reference) {
  eg_set_t *set = (eg_set_t *)eg_heap_alloc(sizeof(eg_set_t), 1, 0);

  set->table = (eg_set_record_t **)eg_heap_alloc(reference->table_size, 1, 0);
  set->table_size = reference->table_size;
  set->table_count = reference->table_count;
  set->record_count = reference->record_count;

  uint64_t table_index = 0;

  while (table_index < set->table_count) {

    eg_set_record_t *curr_reference = reference->table[table_index];
    eg_set_record_t *curr = set->table[table_index];

    if (curr_reference) {

      uint64_t hash = eg_set_hash(set, curr_reference->key, curr_reference->key_size, set->table_count);

      curr = (eg_set_record_t *)eg_heap_alloc(sizeof(eg_set_record_t), 1, 0);

      curr->next = set->table[hash];
      curr->key = (uint8_t *)eg_heap_alloc(curr_reference->key_size, 0, curr_reference->key);
      curr->key_size = curr_reference->key_size;

      set->table[hash] = curr;
    }

    table_index++;
  }

  return set;
}
uint8_t eg_set_equal(eg_set_t *set, eg_set_t *reference) {
  uint8_t not_equal = 0;

  not_equal |= set->record_count != reference->record_count;

  if (set->record_count == reference->record_count) {

    uint64_t table_index = 0;

    while (table_index < set->table_count) {

      eg_set_record_t *curr_reference = reference->table[table_index];
      eg_set_record_t *curr = set->table[table_index];

      not_equal |= (curr_reference == 0) != (curr == 0);

      while (curr && curr_reference) {
        not_equal |= curr->key_size != curr_reference->key_size;

        if (not_equal == 0) {
          not_equal |= memcmp(curr->key, curr_reference->key, EG_MIN(curr->key_size, curr_reference->key_size));
        }

        curr_reference = curr_reference->next;
        curr = curr->next;
      }

      table_index++;
    }
  }

  return not_equal == 0;
}
uint8_t eg_set_insert(eg_set_t *set, void const *key, uint64_t key_size) {
  uint8_t key_exists = 0;
  uint8_t load_factor = eg_set_load_factor(set);

  if (load_factor > EG_SET_LOAD_FACTOR) {
    eg_set_expand(set);
  }

  uint64_t hash = eg_set_hash(set, key, key_size, set->table_count);

  eg_set_record_t *curr = set->table[hash];

  while (curr) {
    if (memcmp(curr->key, key, EG_MIN(curr->key_size, key_size)) == 0) {
      key_exists = 1;

      break;
    }

    curr = curr->next;
  }

  if (key_exists == 0) {

    curr = (eg_set_record_t *)eg_heap_alloc(sizeof(eg_set_record_t), 1, 0);

    curr->next = set->table[hash];
    curr->key = (uint8_t *)eg_heap_alloc(key_size, 0, key);
    curr->key_size = key_size;

    set->table[hash] = curr;
    set->record_count++;
  }

  return key_exists;
}
uint8_t eg_set_remove(eg_set_t *set, void const *key, uint64_t key_size) {
  uint64_t hash = eg_set_hash(set, key, key_size, set->table_count);

  eg_set_record_t *curr = set->table[hash];
  eg_set_record_t *prev = 0;

  while (curr) {

    if (memcmp(curr->key, key, EG_MIN(curr->key_size, key_size)) == 0) {

      if (prev) {
        prev->next = curr->next;
      } else {
        set->table[hash] = curr->next;
      }

      eg_heap_free(curr->key);
      eg_heap_free(curr);

      set->record_count--;

      return 1;
    }

    prev = curr;
    curr = curr->next;
  }

  return 0;
}
uint8_t eg_set_contains(eg_set_t *set, void const *key, uint64_t key_size) {
  uint64_t hash = eg_set_hash(set, key, key_size, set->table_count);

  eg_set_record_t *curr = set->table[hash];

  while (curr) {

    if (memcmp(curr->key, key, EG_MIN(curr->key_size, key_size)) == 0) {
      return 1;
    }

    curr = curr->next;
  }

  return 0;
}
uint64_t eg_set_table_size(eg_set_t *set) {
  return set->table_size;
}
uint64_t eg_set_table_count(eg_set_t *set) {
  return set->table_count;
}
uint64_t eg_set_count(eg_set_t *set) {
  return set->record_count;
}
eg_set_record_t *eg_set_table_at(eg_set_t *set, uint64_t index) {
  return set->table[index];
}
void eg_set_expand(eg_set_t *set) {
  uint64_t table_index = 0;
  uint64_t table_size = set->table_size * 2;
  uint64_t table_count = set->table_count * 2;

  eg_set_record_t **table = (eg_set_record_t **)eg_heap_alloc(table_size, 1, 0);

  while (table_index < set->table_count) {

    eg_set_record_t *curr = set->table[table_index];

    while (curr) {

      uint64_t hash = eg_set_hash(set, curr->key, curr->key_size, table_count);

      curr->next = table[hash];
      table[hash] = curr;
      curr = curr->next;
    }

    table_index++;
  }

  eg_heap_free(set->table);

  set->table = table;
  set->table_size = table_size;
  set->table_count = table_count;
}
void eg_set_clear(eg_set_t *set) {
  uint64_t table_index = 0;

  while (table_index < set->table_count) {

    eg_set_record_t *curr = set->table[table_index];

    while (curr) {

      eg_set_record_t *tmp = curr;

      curr = curr->next;

      eg_heap_free(tmp->key);
      eg_heap_free(tmp);
    }

    table_index++;
  }

  memset(set->table, 0, set->table_size);

  set->record_count = 0;
}
uint64_t eg_set_hash(eg_set_t *set, void const *key, uint64_t key_size, uint64_t modulus) {
  uint64_t hash = EG_SET_HASH_POLY;
  uint64_t key_index = 0;

  while (key_index < key_size) {

    hash = ((hash << 5) + hash) + *(((uint8_t *)key) + key_index);

    key_index++;
  }

  return hash % modulus;
}
uint8_t eg_set_load_factor(eg_set_t *set) {
  return (uint8_t)(((set->record_count + 1) / set->table_count) * 100);
}
void eg_set_destroy(eg_set_t *set) {
  uint64_t table_index = 0;

  while (table_index < set->table_count) {

    eg_set_record_t *curr = set->table[table_index];

    while (curr) {

      eg_set_record_t *tmp = curr;

      curr = curr->next;

      eg_heap_free(tmp->key);
      eg_heap_free(tmp);
    }

    table_index++;
  }

  eg_heap_free(set->table);
}

void *eg_set_record_key(eg_set_record_t *record) {
  return record->key;
}
uint64_t eg_set_record_key_size(eg_set_record_t *record) {
  return record->key_size;
}

eg_set_iter_t *eg_set_iter(eg_set_t *set) {
  eg_set_iter_t *iter = (eg_set_iter_t *)eg_heap_alloc(sizeof(eg_set_iter_t), 1, 0);

  iter->table = set->table;
  iter->table_count = set->table_count;
  iter->first_step = 0;

  uint64_t table_index = 0;

  while (table_index < set->table_count) {

    eg_set_record_t *curr = set->table[table_index];

    if (curr) {

      iter->table_index = table_index;
      iter->table_record = curr;

      break;
    }

    table_index++;
  }

  return iter;
}
uint8_t eg_set_iter_step(eg_set_iter_t *iter) {
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
void *eg_set_iter_key(eg_set_iter_t *iter) {
  return iter->table_record->key;
}
uint64_t eg_set_iter_key_size(eg_set_iter_t *iter) {
  return iter->table_record->key_size;
}
void eg_set_iter_destroy(eg_set_iter_t *iter) {
  eg_heap_free(iter);
}
