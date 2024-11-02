#ifndef API_TYPES_H
#define API_TYPES_H

#include <stdint.h>
#include <stddef.h>
#include <limits.h>
#include <stdbool.h>
#include <stdarg.h>
#include <setjmp.h>
#include <stdalign.h>

#define MISSION_API static

#define MISSION_INLINE static inline

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef float f32;
typedef double f64;

#define DeferLoop(begin, end)                                        \
  for (int PASTE(_i_, __LINE__) = (begin, 0); !PASTE(_i_, __LINE__); \
       PASTE(_i_, __LINE__) += (end, 1))

#define IGNORED(x) (void)(x)
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(x, lo, hi) ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x))
#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)
#define PASTE_(a, b) a##b
#define PASTE(a, b) PASTE_(a, b)
#define ARRAY_COUNT(x) (sizeof(x) / sizeof((x)[0]))
#define KB(x) ((size_t)(x) << 10)
#define MB(x) ((size_t)(x) << 20)
#define GB(x) ((size_t)(x) << 30)
#define TB(x) ((size_t)(x) << 40)
#define SWAP(t, a, b)              \
  do {                             \
    t PASTE(temp__, __LINE__) = a; \
    a = b;                         \
    b = PASTE(temp__, __LINE__);   \
  } while (0)
#define DEG_TO_RAD (PI32 / 180.0f)
#define RAD_TO_DEG (180.0f / PI32)
#define PAD(n) char PASTE(pad__, __LINE__)[n]

#define MASK_BITS(n) ((1 << ((n) + 1)) - 1)

#define DEFAULT_STRING_ALIGN 16

typedef struct string_t {
  size_t count;
  const char* data;
} string_t;

#define STRING_STORAGE(size) \
  struct {                   \
    size_t count;            \
    char data[size];         \
  }
#define STRING_FROM_STORAGE(storage) \
  (string_t) {                       \
    (storage).count, (storage).data  \
  }
#define STRING_INTO_STORAGE(storage, string)                      \
  (copy_memory((storage).data, (string).data,                     \
               MIN(ARRAY_COUNT((storage).data), (string).count)), \
   (storage).count = (string).count)
#define STRING_STORAGE_SIZE(storage) ARRAY_COUNT((storage).data)

#define strinit(text) {sizeof(text) - 1, (const char*)("" text)}
#define strlit(text) ((string_t){sizeof(text) - 1, (const char*)("" text)})
#define S(text) strlit(text)
#define strexpand(string) (int)(string).count, (string).data
#define Sx(text) strexpand(text)
#define strnull \
  (string_t) {  \
    0           \
  }

typedef struct string16_t {
  size_t count;
  const wchar_t* data;
} string16_t;

#define strlit16(text)                                             \
  (string16_t) {                                                   \
    sizeof(text) / sizeof(wchar_t) - 1, (const wchar_t*)(L"" text) \
  }
#define strnull16 \
  (string16_t) {  \
    0             \
  }

typedef struct arena_t {
  bool owns_memory;

  char* committed;
  char* end;
  char* at;
  char* buffer;
} arena_t;

typedef struct arena_marker_t {
  arena_t* arena;
  char* at;
} arena_marker_t;

typedef union v2 {
  struct {
    f32 x, y;
  };
  f32 e[2];
} v2;

typedef union v2i {
  struct {
    s32 x, y;
  };
  s32 e[2];
} v2i;

typedef union v3 {
  struct {
    f32 x, y, z;
  };
  struct {
    v2 xy;
    f32 z0;
  };
  f32 e[3];
} v3;

typedef union v3i {
  struct {
    s32 x, y, z;
  };
  struct {
    v2i xy;
    s32 z0;
  };
  s32 e[3];
} v3i;

typedef union v4 {
  struct {
    f32 x, y, z, w;
  };
  struct {
    v3 xyz;
    f32 w0;
  };
  f32 e[4];
} v4;

typedef v4 quat;

typedef union mat4 {
  f32 e[4][4];
  v4 col[4];
} mat4;

typedef struct mat {
  int m;
  int n;
  f32* e;
} mat;

#define M(mat, row, col) (mat)->e[(mat)->n * (col) + (row)]

typedef union rect2 {
  struct {
    f32 x0, y0, x1, y1;
  };
  struct {
    v2 min;
    v2 max;
  };
} rect2;

typedef union rect2i {
  struct {
    s32 x0, y0, x1, y1;
  };
  struct {
    v2i min;
    v2i max;
  };
} rect2i;

typedef union rect3 {
  struct {
    f32 x0, y0, z0, x1, y1, z1;
  };
  struct {
    v3 min;
    v3 max;
  };
} rect3;

typedef struct plane_t {
  v3 n;
  f32 d;
} plane_t;

typedef union resource_handle_t {
  struct {
    u32 index, generation;
  };
  u64 value;
} resource_handle_t;

#define NULL_RESOURCE_HANDLE ((resource_handle_t){0, 0})
#define RESOURCE_HANDLE_VALID(x) ((x).index != 0)
#define RESOURCE_HANDLES_EQUAL(a, b) ((a).value == (b).value)

typedef struct node_t node_t;
struct node_t {
  node_t* parent;
  node_t *first_child, *last_child;
  node_t *next, *prev;
};

#define NODE_STUCTURE(type)              \
  struct type* parent;                   \
  struct type *first_child, *last_child; \
  struct type *next, *prev;

typedef enum axis2_t {
  AXIS2_X,
  AXIS2_Y,
  AXIS2_COUNT,
} axis2_t;

typedef struct random_series {
  u32 state;
} random_series;

typedef struct hires_time_t {
  u64 value;
} hires_time_t;

typedef struct mutex_t {
  void* opaque;
} mutex_t;

typedef struct cond_t {
  void* opaque;
} cond_t;

#endif  // API_TYPES_H