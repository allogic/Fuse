#include <container/lb_pch.h>
#include <container/lb_config.h>
#include <container/lb_macros.h>
#include <container/lb_set.h>

struct lb_set_t {
  lb_set_record_t **table;
  uint64_t table_size;
  uint64_t table_count;
  uint64_t record_count;
};
struct lb_set_record_t {
  lb_set_record_t *next;
  uint8_t *key;
  uint64_t key_size;
};
struct lb_set_iter_t {
  lb_set_record_t **table;
  lb_set_record_t *table_record;
  uint64_t table_index;
  uint64_t table_count;
  uint8_t first_step;
};

lb_set_t *lb_set_create(void) {
  lb_set_t *set = (lb_set_t *)lb_heap_alloc(sizeof(lb_set_t), 1, 0);

  set->table = (lb_set_record_t **)lb_heap_alloc(LB_SET_TABLE_COUNT * sizeof(lb_set_record_t *), 1, 0);
  set->table_size = LB_SET_TABLE_COUNT * sizeof(lb_set_record_t *);
  set->table_count = LB_SET_TABLE_COUNT;
  set->record_count = 0;

  return set;
}
lb_set_t *lb_set_copy(lb_set_t *reference) {
  lb_set_t *set = (lb_set_t *)lb_heap_alloc(sizeof(lb_set_t), 1, 0);

  set->table = (lb_set_record_t **)lb_heap_alloc(reference->table_size, 1, 0);
  set->table_size = reference->table_size;
  set->table_count = reference->table_count;
  set->record_count = reference->record_count;

  uint64_t table_index = 0;

  while (table_index < set->table_count) {

    lb_set_record_t *curr_reference = reference->table[table_index];
    lb_set_record_t *curr = set->table[table_index];

    if (curr_reference) {

      uint64_t hash = lb_set_hash(set, curr_reference->key, curr_reference->key_size, set->table_count);

      curr = (lb_set_record_t *)lb_heap_alloc(sizeof(lb_set_record_t), 1, 0);

      curr->next = set->table[hash];
      curr->key = (uint8_t *)lb_heap_alloc(curr_reference->key_size, 0, curr_reference->key);
      curr->key_size = curr_reference->key_size;

      set->table[hash] = curr;
    }

    table_index++;
  }

  return set;
}
uint8_t lb_set_equal(lb_set_t *set, lb_set_t *reference) {
  uint8_t not_equal = 0;

  not_equal |= set->record_count != reference->record_count;

  if (set->record_count == reference->record_count) {

    uint64_t table_index = 0;

    while (table_index < set->table_count) {

      lb_set_record_t *curr_reference = reference->table[table_index];
      lb_set_record_t *curr = set->table[table_index];

      not_equal |= (curr_reference == 0) != (curr == 0);

      while (curr && curr_reference) {
        not_equal |= curr->key_size != curr_reference->key_size;

        if (not_equal == 0) {
          not_equal |= memcmp(curr->key, curr_reference->key, LB_MIN(curr->key_size, curr_reference->key_size));
        }

        curr_reference = curr_reference->next;
        curr = curr->next;
      }

      table_index++;
    }
  }

  return not_equal == 0;
}
uint8_t lb_set_insert(lb_set_t *set, void const *key, uint64_t key_size) {
  uint8_t key_exists = 0;
  uint8_t load_factor = lb_set_load_factor(set);

  if (load_factor > LB_SET_LOAD_FACTOR) {
    lb_set_expand(set);
  }

  uint64_t hash = lb_set_hash(set, key, key_size, set->table_count);

  lb_set_record_t *curr = set->table[hash];

  while (curr) {
    if (memcmp(curr->key, key, LB_MIN(curr->key_size, key_size)) == 0) {
      key_exists = 1;

      break;
    }

    curr = curr->next;
  }

  if (key_exists == 0) {

    curr = (lb_set_record_t *)lb_heap_alloc(sizeof(lb_set_record_t), 1, 0);

    curr->next = set->table[hash];
    curr->key = (uint8_t *)lb_heap_alloc(key_size, 0, key);
    curr->key_size = key_size;

    set->table[hash] = curr;
    set->record_count++;
  }

  return key_exists;
}
uint8_t lb_set_remove(lb_set_t *set, void const *key, uint64_t key_size) {
  uint64_t hash = lb_set_hash(set, key, key_size, set->table_count);

  lb_set_record_t *curr = set->table[hash];
  lb_set_record_t *prev = 0;

  while (curr) {

    if (memcmp(curr->key, key, LB_MIN(curr->key_size, key_size)) == 0) {

      if (prev) {
        prev->next = curr->next;
      } else {
        set->table[hash] = curr->next;
      }

      lb_heap_free(curr->key);
      lb_heap_free(curr);

      set->record_count--;

      return 1;
    }

    prev = curr;
    curr = curr->next;
  }

  return 0;
}
uint8_t lb_set_contains(lb_set_t *set, void const *key, uint64_t key_size) {
  uint64_t hash = lb_set_hash(set, key, key_size, set->table_count);

  lb_set_record_t *curr = set->table[hash];

  while (curr) {

    if (memcmp(curr->key, key, LB_MIN(curr->key_size, key_size)) == 0) {
      return 1;
    }

    curr = curr->next;
  }

  return 0;
}
uint64_t lb_set_table_size(lb_set_t *set) {
  return set->table_size;
}
uint64_t lb_set_table_count(lb_set_t *set) {
  return set->table_count;
}
uint64_t lb_set_count(lb_set_t *set) {
  return set->record_count;
}
lb_set_record_t *lb_set_table_at(lb_set_t *set, uint64_t index) {
  return set->table[index];
}
void lb_set_expand(lb_set_t *set) {
  uint64_t table_index = 0;
  uint64_t table_size = set->table_size * 2;
  uint64_t table_count = set->table_count * 2;

  lb_set_record_t **table = (lb_set_record_t **)lb_heap_alloc(table_size, 1, 0);

  while (table_index < set->table_count) {

    lb_set_record_t *curr = set->table[table_index];

    while (curr) {

      uint64_t hash = lb_set_hash(set, curr->key, curr->key_size, table_count);

      curr->next = table[hash];
      table[hash] = curr;
      curr = curr->next;
    }

    table_index++;
  }

  lb_heap_free(set->table);

  set->table = table;
  set->table_size = table_size;
  set->table_count = table_count;
}
void lb_set_clear(lb_set_t *set) {
  uint64_t table_index = 0;

  while (table_index < set->table_count) {

    lb_set_record_t *curr = set->table[table_index];

    while (curr) {

      lb_set_record_t *tmp = curr;

      curr = curr->next;

      lb_heap_free(tmp->key);
      lb_heap_free(tmp);
    }

    table_index++;
  }

  memset(set->table, 0, set->table_size);

  set->record_count = 0;
}
uint64_t lb_set_hash(lb_set_t *set, void const *key, uint64_t key_size, uint64_t modulus) {
  uint64_t hash = LB_SET_HASH_POLY;
  uint64_t key_index = 0;

  while (key_index < key_size) {

    hash = ((hash << 5) + hash) + *(((uint8_t *)key) + key_index);

    key_index++;
  }

  return hash % modulus;
}
uint8_t lb_set_load_factor(lb_set_t *set) {
  return (uint8_t)(((set->record_count + 1) / set->table_count) * 100);
}
void lb_set_destroy(lb_set_t *set) {
  uint64_t table_index = 0;

  while (table_index < set->table_count) {

    lb_set_record_t *curr = set->table[table_index];

    while (curr) {

      lb_set_record_t *tmp = curr;

      curr = curr->next;

      lb_heap_free(tmp->key);
      lb_heap_free(tmp);
    }

    table_index++;
  }

  lb_heap_free(set->table);
}

void *lb_set_record_key(lb_set_record_t *record) {
  return record->key;
}
uint64_t lb_set_record_key_size(lb_set_record_t *record) {
  return record->key_size;
}

lb_set_iter_t *lb_set_iter(lb_set_t *set) {
  lb_set_iter_t *iter = (lb_set_iter_t *)lb_heap_alloc(sizeof(lb_set_iter_t), 1, 0);

  iter->table = set->table;
  iter->table_count = set->table_count;
  iter->first_step = 0;

  uint64_t table_index = 0;

  while (table_index < set->table_count) {

    lb_set_record_t *curr = set->table[table_index];

    if (curr) {

      iter->table_index = table_index;
      iter->table_record = curr;

      break;
    }

    table_index++;
  }

  return iter;
}
uint8_t lb_set_iter_step(lb_set_iter_t *iter) {
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
void *lb_set_iter_key(lb_set_iter_t *iter) {
  return iter->table_record->key;
}
uint64_t lb_set_iter_key_size(lb_set_iter_t *iter) {
  return iter->table_record->key_size;
}
void lb_set_iter_destroy(lb_set_iter_t *iter) {
  lb_heap_free(iter);
}
