#ifndef MISSION_HASHTABLE_H
#define MISSION_HASHTABLE_H

#include "api_types.h"

enum { UNUSED_KEY_VALUE = 0 };

typedef struct hash_entry_t {
  u64 key;
  u64 value;
} hash_entry_t;

typedef struct hash_t {
  u32 mask;
  u32 load;
  hash_entry_t* entries;
} hash_t;

bool hash_find(const hash_t* table, u64 key, u64* value);
void hash_add(hash_t* table, u64 key, u64 value);
bool hash_rem(hash_t* table, u64 key);

void* hash_find_object(const hash_t* table, u64 key);
void hash_add_object(hash_t* table, u64 key, void* value);

typedef struct hash_iter_t {
  const hash_t* table;
  u64 index;

  union {
    u64 value;
    void* ptr;
  };
} hash_iter_t;

MISSION_INLINE hash_iter_t hash_iter(const hash_t* table) {
  hash_iter_t it = {
      .table = table,
      .index = 0,
  };

  return it;
}

MISSION_INLINE bool hash_iter_next(hash_iter_t* it) {
  bool result = false;

  const hash_t* table = it->table;

  if (!table->entries) {
    return false;
  }
  while (it->index <= table->mask) {
    if (table->entries[it->index].key != UNUSED_KEY_VALUE) {
      result = true;

      it->value = table->entries[it->index].value;
      it->index += 1;

      break;
    }

    it->index += 1;
  }

  return result;
}

#endif