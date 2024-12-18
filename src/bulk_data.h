#ifndef BULK_DATA_H
#define BULK_DATA_H

#include "api_types.h"

enum { BD_FREE_BIT = 1 << 31 };

typedef struct bulk_item_t {
  u32 generation;
} bulk_item_t;

typedef struct free_item_t {
  u32 generation;
  u32 next;
} free_item_t;

// typedef enum bulk_flags_t {
//   BULK_FLAGS_CONCURRENT = (1 << 0),
// } bulk_flags_t;

typedef struct bulk_t {
  size_t watermark;
  u32 item_size;
  u32 align;
  char* buffer;
  u32 flags;
  // mutex_t lock;
} bulk_t;

resource_handle_t bd_get_handle(bulk_t* bd, void* item);

void* bd_add(bulk_t* bd);
resource_handle_t bd_add_item(bulk_t* bd, const void* item);
void* bd_get(bulk_t* bd, resource_handle_t handle);
bool bd_rem(bulk_t* bd, resource_handle_t handle);
bool bd_rem_item(bulk_t* bd, void* item);

#define INIT_BULK_DATA(type) {.item_size = sizeof(type), .align = alignof(type)}
// #define INIT_BULK_DATA_EX(Type, Flags) \
//   {.item_size = sizeof(Type), .align = alignof(Type), .flags = Flags}

typedef struct bd_iter_t {
  bulk_t* bd;
  void* data;
} bd_iter_t;

bd_iter_t bd_iter(bulk_t* bd);
bool bd_iter_valid(bd_iter_t* it);
void bd_iter_next(bd_iter_t* it);
void bd_iter_rem(bd_iter_t* it);

#endif  // BULK_DATA_H