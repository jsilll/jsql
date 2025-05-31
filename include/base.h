#ifndef BASE_H
#define BASE_H

// =================================================================================================
// :: Standard Includes ::
// =================================================================================================

#include <assert.h>
#include <ctype.h>
#include <float.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uchar.h>

// =================================================================================================
// :: Basic Types & Aliases ::
// =================================================================================================

// --- Integer Types ---
typedef int8_t i8;
typedef uint8_t u8;
typedef int16_t i16;
typedef uint16_t u16;
typedef int32_t i32;
typedef uint32_t u32;
typedef int64_t i64;
typedef uint64_t u64;

// --- Floating Point Types ---
typedef float f32;
typedef double f64;

// --- Sized Boolean Types ---
typedef int8_t b8;
typedef int16_t b16;
typedef int32_t b32;
typedef int64_t b64;

// --- Character Types ---
typedef unsigned char uchar;
typedef char8_t char8;
typedef char16_t char16;
typedef char32_t char32;

// --- Size and Pointer Types ---
typedef size_t usize;
typedef ptrdiff_t isize;

// =================================================================================================
// :: Arena Configuration Macros ::
// =================================================================================================

#ifndef BASE_ARENA_DEFAULT_ALIGNMENT
#define BASE_ARENA_DEFAULT_ALIGNMENT (2 * sizeof(usize))
#endif

// =================================================================================================
// :: Vector Configuration Macros ::
// =================================================================================================

#ifndef BASE_VECTOR_BOUNDS_CHECK
#define BASE_VECTOR_BOUNDS_CHECK 1
#endif

#ifndef BASE_VECTOR_GROWTH_FACTOR
#define BASE_VECTOR_GROWTH_FACTOR 2.0
#endif

#ifndef BASE_VECTOR_DEFAULT_CAPACITY
#define BASE_VECTOR_DEFAULT_CAPACITY 8
#endif

// =================================================================================================
// :: Hash Table (Open Addressing) Configuration Macros ::
// =================================================================================================

#ifndef BASE_HT_OA_DEFAULT_CAPACITY
#define BASE_HT_OA_DEFAULT_CAPACITY 16
#endif

#ifndef BASE_HT_OA_MAX_LOAD_FACTOR
#define BASE_HT_OA_MAX_LOAD_FACTOR                                             \
  0.7f // Rehash when (item_count + tombstone_count) / bucket_count > this
#endif

// =================================================================================================
// :: Useful Constants ::
// =================================================================================================

#define PI_F32 3.14159265358979323846f
#define PI_F64 3.14159265358979323846264338327950288

#define E_F32 2.71828182845904523536f
#define E_F64 2.71828182845904523536028747135266250

#define SQRT2_F32 1.41421356237309504880f
#define SQRT2_F64 1.41421356237309504880168872420969808

#define SQRT3_F32 1.73205080756887729352f
#define SQRT3_F64 1.73205080756887729352744634150587236

// =================================================================================================
// :: Helper Macros ::
// =================================================================================================

// --- Math ---
// WARNING: Macros like SIGN, ABS, MIN, MAX, CLAMP evaluate arguments multiple
// times. Use with caution if arguments have side effects (e.g., SIGN(i++)).
#define SIGN(x) (((x) > 0) - ((x) < 0))
#define ABS(a) (((a) < 0) ? -(a) : (a))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define LERP(a, b, t) ((a) + ((b) - (a)) * (t))
#define CLAMP(x, low, high)                                                    \
  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

// --- Array ---
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

// --- Bit Manipulation ---
#define BIT(n) (1 << (n))
#define SET_BIT(val, bit_mask) ((val) |= (bit_mask))
#define CLEAR_BIT(val, bit_mask) ((val) &= ~(bit_mask))
#define TOGGLE_BIT(val, bit_mask) ((val) ^= (bit_mask))
#define IS_BIT_SET(val, bit_mask) (((val) & (bit_mask)) != 0)

// --- Pointer & Memory ---
#define nullptr ((void *)0)
#define ZERO_STRUCT(s) memset(&(s), 0, sizeof(s))
#define ZERO_ARRAY(a, count) memset((a), 0, (count) * sizeof((a)[0]))
#define ALIGN_UP(value, alignment)                                             \
  (((value) + (alignment) - 1) & ~((alignment) - 1))

// --- Stringification ---
#define STRINGIFY_INTERNAL(x) #x
#define STRINGIFY(x) STRINGIFY_INTERNAL(x)

// --- Concatenation ---
#define CONCAT_INTERNAL(a, b) a##b
#define CONCAT(a, b) CONCAT_INTERNAL(a, b)

// --- Compiler Detection ---
#if defined(__clang__)
#define COMPILER_CLANG
#elif defined(_MSC_VER)
#define COMPILER_CL
#elif defined(__GNUC__)
#define COMPILER_GCC
#endif

#if defined(COMPILER_CLANG)
#define FILE_NAME __FILE_NAME__
#else
#define FILE_NAME __FILE__
#endif

// =================================================================================================
// :: Assertion and Logging Macros ::
// =================================================================================================

// --- Assertion ---
#ifndef NDEBUG
#define ASSERT(expr)                                                           \
  do {                                                                         \
    if (!(expr)) {                                                             \
      fprintf(                                                                 \
          stderr,                                                              \
          "Assertion failed: %s\n\tFile: %s\n\tLine: %d\n\tFunction: %s\n",    \
          #expr, __FILE__, __LINE__, __func__);                                \
      fflush(stderr);                                                          \
      exit(EXIT_FAILURE);                                                      \
    }                                                                          \
  } while (0)

#define ASSERT_MSG(expr, msg, ...)                                             \
  do {                                                                         \
    if (!(expr)) {                                                             \
      fprintf(stderr,                                                          \
              "Assertion failed: %s\n\tFile: %s\n\tLine: %d\n\tFunction: "     \
              "%s\n\tMessage: " msg "\n",                                      \
              #expr, __FILE__, __LINE__, __func__, ##__VA_ARGS__);             \
      fflush(stderr);                                                          \
      exit(EXIT_FAILURE);                                                      \
    }                                                                          \
  } while (0)
#else
#define ASSERT(expr) ((void)0)
#define ASSERT_MSG(expr, msg, ...) ((void)0)
#endif

// --- Logging ---
typedef enum {
  LOG_LEVEL_DEBUG = 0,
  LOG_LEVEL_INFO = 1,
  LOG_LEVEL_WARNING = 2,
  LOG_LEVEL_ERROR = 3,
  LOG_LEVEL_FATAL = 4,
} LogLevel;

extern LogLevel
    g_log_level; // Must be defined in one .c file via BASE_IMPLEMENTATION

#define LOG_MSG_STREAM(stream, level, prefix, fmt, ...)                        \
  do {                                                                         \
    if (level >= g_log_level) {                                                \
      fprintf(stream, "[%s] [%s:%d] " fmt "\n", prefix, __func__, __LINE__,    \
              ##__VA_ARGS__);                                                  \
      fflush(stream);                                                          \
    }                                                                          \
  } while (0)

#define LOG_DEBUG(fmt, ...)                                                    \
  LOG_MSG_STREAM(stdout, LOG_LEVEL_DEBUG, "DEBUG", fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)                                                     \
  LOG_MSG_STREAM(stdout, LOG_LEVEL_INFO, "INFO", fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)                                                     \
  LOG_MSG_STREAM(stderr, LOG_LEVEL_WARNING, "WARNING", fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...)                                                    \
  LOG_MSG_STREAM(stderr, LOG_LEVEL_ERROR, "ERROR", fmt, ##__VA_ARGS__)
#define LOG_FATAL(fmt, ...)                                                    \
  do {                                                                         \
    fprintf(stderr, "[FATAL] [%s:%d] " fmt "\n", __func__, __LINE__,           \
            ##__VA_ARGS__);                                                    \
    fflush(stderr);                                                            \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

// =================================================================================================
// :: Arena Allocator ::
// =================================================================================================

typedef struct {
  u8 *buffer;           // The allocated memory block
  usize total_size;     // Total size of the buffer
  usize prev_offset;    // Previous allocation offset, for temporary allocations
  usize current_offset; // Current allocation offset from the beginning of the
                        // buffer
} Arena;

Arena arena_init(usize total_size_bytes);
void arena_free_all(Arena *arena);
void *arena_alloc_aligned(Arena *arena, usize item_size, usize alignment);

static inline void *arena_alloc(Arena *arena, usize item_size) {
  return arena_alloc_aligned(arena, item_size, BASE_ARENA_DEFAULT_ALIGNMENT);
}

static inline void arena_reset(Arena *arena) {
  ASSERT(arena);
  arena->prev_offset = 0;
  arena->current_offset = 0;
}

static inline void arena_mark_temp(Arena *arena) {
  ASSERT(arena);
  arena->prev_offset = arena->current_offset;
}

static inline void arena_release_temp(Arena *arena) {
  ASSERT(arena);
  arena->current_offset = arena->prev_offset;
}

// =================================================================================================
// :: Slices ::
// =================================================================================================

#define SLICE_DECLARE(SliceName, Type)                                         \
  typedef struct {                                                             \
    Type *data;                                                                \
    usize size;                                                                \
  } SliceName;                                                                 \
                                                                               \
  static inline SliceName slice_##SliceName##_init(Type *data, usize size) {   \
    return (SliceName){.data = data, .size = size};                            \
  }                                                                            \
                                                                               \
  static inline Type *slice_##SliceName##_begin(SliceName *slice) {            \
    ASSERT(slice);                                                             \
    return slice->data;                                                        \
  }                                                                            \
                                                                               \
  static inline const Type *slice_##SliceName##_begin_const(                   \
      const SliceName *slice) {                                                \
    ASSERT(slice);                                                             \
    return slice->data;                                                        \
  }                                                                            \
                                                                               \
  static inline Type *slice_##SliceName##_end(SliceName *slice) {              \
    ASSERT(slice);                                                             \
    return slice->data + slice->size;                                          \
  }                                                                            \
                                                                               \
  static inline const Type *slice_##SliceName##_end_const(                     \
      const SliceName *slice) {                                                \
    ASSERT(slice);                                                             \
    return slice->data + slice->size;                                          \
  }                                                                            \
                                                                               \
  static inline usize slice_##SliceName##_size(const SliceName *slice) {       \
    ASSERT(slice);                                                             \
    return slice->size;                                                        \
  }

// =================================================================================================
// :: Dynamic Array ::
// =================================================================================================

#define VECTOR_DECLARE(VecName, Type)                                          \
  typedef struct {                                                             \
    Type *data;                                                                \
    usize size;                                                                \
    usize capacity;                                                            \
  } VecName;                                                                   \
                                                                               \
  VecName vec_##VecName##_init(usize initial_capacity);                        \
  void vec_##VecName##_free(VecName *vec);                                     \
  bool vec_##VecName##_push(VecName *vec, Type value);                         \
  bool vec_##VecName##_pop(VecName *vec, Type *out_value);                     \
  Type *vec_##VecName##_get(VecName *vec, usize index);                        \
  const Type *vec_##VecName##_get_const(const VecName *vec, usize index);      \
  void vec_##VecName##_set(VecName *vec, usize index, Type value);             \
  bool vec_##VecName##_insert(VecName *vec, usize index, Type value);          \
  bool vec_##VecName##_remove(VecName *vec, usize index, Type *out_value);     \
  bool vec_##VecName##_reserve(VecName *vec, usize new_capacity);              \
  bool vec_##VecName##_resize(VecName *vec, usize new_size,                    \
                              Type default_value_if_expanding);                \
  void vec_##VecName##_clear(VecName *vec);                                    \
  bool vec_##VecName##_shrink_to_fit(VecName *vec);                            \
  VecName vec_##VecName##_copy(const VecName *vec);                            \
  bool vec_##VecName##_append_array(VecName *vec, const Type *arr,             \
                                    usize count);                              \
                                                                               \
  static inline Type *vec_##VecName##_begin(VecName *vec) {                    \
    ASSERT(vec);                                                               \
    return vec->data;                                                          \
  }                                                                            \
                                                                               \
  static inline const Type *vec_##VecName##_begin_const(const VecName *vec) {  \
    ASSERT(vec);                                                               \
    return vec->data;                                                          \
  }                                                                            \
                                                                               \
  static inline Type *vec_##VecName##_end(VecName *vec) {                      \
    ASSERT(vec);                                                               \
    return (vec->data + vec->size);                                            \
  }                                                                            \
                                                                               \
  static inline const Type *vec_##VecName##_end_const(const VecName *vec) {    \
    ASSERT(vec);                                                               \
    return (vec->data + vec->size);                                            \
  }                                                                            \
                                                                               \
  static inline usize vec_##VecName##_size(const VecName *vec) {               \
    ASSERT(vec);                                                               \
    return vec->size;                                                          \
  }                                                                            \
                                                                               \
  static inline usize vec_##VecName##_capacity(const VecName *vec) {           \
    ASSERT(vec);                                                               \
    return vec->capacity;                                                      \
  }                                                                            \
                                                                               \
  static inline bool vec_##VecName##_is_empty(const VecName *vec) {            \
    ASSERT(vec);                                                               \
    return vec->size == 0;                                                     \
  }

// =================================================================================================
// :: String View ::
// =================================================================================================

typedef struct {
  const char *data;
  usize length;
} StringView;

#define SV(s) ((StringView){.data = (s), .length = sizeof(s) - 1})

static inline StringView sv_from_cstr(const char *cstr) {
  return (StringView){.data = cstr, .length = cstr ? strlen(cstr) : 0};
}

static inline StringView sv_from_parts(const char *data, usize length) {
  return (StringView){.data = data, .length = length};
}

static inline bool sv_equals(StringView sv1, StringView sv2) {
  if (sv1.length != sv2.length)
    return false;
  if (sv1.data == sv2.data)
    return true;
  if (!sv1.data || !sv2.data)
    return false;
  return memcmp(sv1.data, sv2.data, sv1.length) == 0;
}

static inline bool sv_equals_cstr(StringView sv, const char *cstr) {
  if (!cstr)
    return sv.length == 0 && !sv.data;
  usize cstr_len = strlen(cstr);
  if (sv.length != cstr_len)
    return false;
  if (!sv.data)
    return false;
  return memcmp(sv.data, cstr, sv.length) == 0;
}

static inline StringView sv_slice(StringView sv, usize start, usize end) {
  if (start >= sv.length || !sv.data)
    return (StringView){.data = NULL, .length = 0};
  if (end > sv.length)
    end = sv.length;
  if (start >= end)
    return (StringView){.data = sv.data + start, .length = 0};
  return (StringView){.data = sv.data + start, .length = end - start};
}

static inline StringView sv_trim_left(StringView sv) {
  if (!sv.data)
    return sv;
  usize i = 0;
  while (i < sv.length && isspace((uchar)sv.data[i])) {
    i++;
  }
  return sv_slice(sv, i, sv.length);
}

static inline StringView sv_trim_right(StringView sv) {
  if (!sv.data)
    return sv;
  isize i = (isize)sv.length - 1;
  while (i >= 0 && isspace((uchar)sv.data[i])) {
    i--;
  }
  return sv_slice(sv, 0, (usize)(i + 1));
}

static inline StringView sv_trim(StringView sv) {
  return sv_trim_left(sv_trim_right(sv));
}

static inline void sv_fprint(FILE *stream, StringView sv) {
  if (sv.data && sv.length > 0) {
    fprintf(stream, "%.*s", (i32)sv.length, sv.data);
  }
}

// =================================================================================================
// :: Dynamic String ::
// =================================================================================================

VECTOR_DECLARE(StringData, char)

typedef StringData BaseString;

BaseString bstr_from_cstr(const char *cstr);
BaseString bstr_from_sv(StringView sv);
bool bstr_append_cstr(BaseString *bstr, const char *cstr);
bool bstr_append_sv(BaseString *bstr, StringView sv);

// Helper to ensure capacity for string operations (including null terminator)
bool bstr_ensure_capacity(BaseString *bstr, usize additional_chars_needed);

static inline BaseString bstr_init(usize initial_capacity_hint) {
  BaseString bstr = vec_StringData_init(initial_capacity_hint > 0
                                            ? initial_capacity_hint + 1
                                            : BASE_VECTOR_DEFAULT_CAPACITY + 1);
  if (bstr.data) {
    bstr.data[0] = '\0'; // Empty string is null-terminated
    // vec_StringData_init sets size to 0, which is correct for an empty
    // string's length
  }
  return bstr;
}

static inline void bstr_free(BaseString *bstr) { vec_StringData_free(bstr); }

static inline bool bstr_append_char(BaseString *bstr, char c) {
  ASSERT(bstr);
  if (!bstr_ensure_capacity(bstr, 1)) {
    return false;
  }
  bstr->data[bstr->size] = c;
  bstr->size++;
  bstr->data[bstr->size] = '\0';
  return true;
}

static inline void bstr_clear(BaseString *bstr) {
  ASSERT(bstr);
  vec_StringData_clear(bstr); // This just sets size to 0
  if (bstr->data && bstr->capacity > 0) {
    bstr->data[0] = '\0'; // Ensure null terminator for empty string
  }
}

static inline usize bstr_len(const BaseString *bstr) {
  ASSERT(bstr);
  return bstr->size;
}

static inline const char *bstr_c_str(BaseString *bstr) {
  ASSERT(bstr);
  ASSERT(bstr->data);
  return bstr->data;
}

// =================================================================================================
// :: Hash Table (Open Addressing) Types & Callbacks ::
// =================================================================================================

typedef struct BaseHashTableOA BaseHashTableOA;

typedef u64 (*BaseHashFunction)(const void *key);
typedef bool (*BaseKeyEqualFunction)(const void *key1, const void *key2);
typedef void *(*BaseCopyFunction)(const void *original, Arena *arena);
typedef void (*BaseFreeFunction)(void *data, Arena *arena);

typedef enum {
  HT_ENTRY_EMPTY = 0,    // Slot is and has always been empty
  HT_ENTRY_OCCUPIED = 1, // Slot contains an active key-value pair
  HT_ENTRY_TOMBSTONE = 2 // Slot previously held data but was deleted
} HashTableEntryStateOA;

typedef struct {
  void *key;
  void *value;
  HashTableEntryStateOA state;
} HashTableEntryOA;

struct BaseHashTableOA {
  HashTableEntryOA *entries; // Array of entries
  usize bucket_count; // Total number of slots in the 'entries' array (capacity)
  usize item_count;   // Number of active key-value pairs (occupied slots)
  usize tombstone_count; // Number of tombstone slots

  BaseHashFunction hash_fn;
  BaseKeyEqualFunction key_equal_fn;

  BaseCopyFunction key_copy_fn;
  BaseFreeFunction key_free_fn;
  BaseCopyFunction value_copy_fn;
  BaseFreeFunction value_free_fn;

  Arena *arena; // Optional: if set, 'entries' array (and maybe keys/values) are
                // allocated from here
};

// =================================================================================================
// :: Hash Table (Open Addressing) API ::
// =================================================================================================

BaseHashTableOA ht_oa_init(usize initial_bucket_count, BaseHashFunction hash_fn,
                           BaseKeyEqualFunction key_equal_fn,
                           Arena *optional_arena);

void ht_oa_set_managed_kv(BaseHashTableOA *ht, BaseCopyFunction key_copy_fn,
                          BaseFreeFunction key_free_fn,
                          BaseCopyFunction value_copy_fn,
                          BaseFreeFunction value_free_fn);

void ht_oa_free(BaseHashTableOA *ht);

bool ht_oa_insert(BaseHashTableOA *ht, const void *key,
                  void *value); // Fails if key exists
bool ht_oa_put(BaseHashTableOA *ht, const void *key, void *value); // Upsert

void *ht_oa_get(const BaseHashTableOA *ht, const void *key);
bool ht_oa_contains(const BaseHashTableOA *ht, const void *key);
bool ht_oa_remove(BaseHashTableOA *ht, const void *key);

usize ht_oa_size(const BaseHashTableOA *ht);
bool ht_oa_is_empty(const BaseHashTableOA *ht);
void ht_oa_clear(
    BaseHashTableOA *ht); // Marks all as empty, frees K/V, keeps bucket array

// Iterator for Open Addressing
typedef struct {
  const BaseHashTableOA *ht;
  usize current_idx; // Current index in the entries array
} HashTableIteratorOA;

HashTableIteratorOA ht_oa_iterator_begin(const BaseHashTableOA *ht);
bool ht_oa_iterator_next(HashTableIteratorOA *iter, const void **out_key,
                         void **out_value);

// (Common hash/equal functions like base_hash_string, base_key_equal_string
// remain the same)

#endif // BASE_H

// =================================================================================================
// :: Implementation Section ::
// =================================================================================================

#ifdef BASE_IMPLEMENTATION

LogLevel g_log_level = LOG_LEVEL_INFO; // Default log level

// --- Arena Allocator Implementation ---
Arena arena_init(usize total_size_bytes) {
  Arena arena = {0};
  arena.buffer = (u8 *)malloc(total_size_bytes);
  if (!arena.buffer) {
    LOG_FATAL("Failed to allocate memory for Arena (size %zu bytes)",
              total_size_bytes);
    arena.total_size = 0; // Mark as unusable
  } else {
    arena.total_size = total_size_bytes;
  }
  arena.current_offset = 0;
  arena.prev_offset = 0;
  return arena;
}

void arena_free_all(Arena *arena) {
  ASSERT(arena);
  free(arena->buffer);
  arena->buffer = NULL;
  arena->total_size = 0;
  arena->prev_offset = 0;
  arena->current_offset = 0;
}

void *arena_alloc_aligned(Arena *arena, usize item_size, usize alignment) {
  ASSERT(arena);
  ASSERT(alignment > 0 &&
         (alignment & (alignment - 1)) == 0); // Alignment must be power of two

  usize aligned_current_offset = ALIGN_UP(arena->current_offset, alignment);
  usize new_current_offset = aligned_current_offset + item_size;

  if (new_current_offset > arena->total_size) {
    LOG_ERROR("Arena out of memory: requested %zu bytes (aligned to %zu), "
              "available %zu bytes at offset %zu (aligned %zu)",
              item_size, alignment, arena->total_size - arena->current_offset,
              arena->current_offset, aligned_current_offset);
    return NULL;
  }

  void *ptr = arena->buffer + aligned_current_offset;
  arena->current_offset = new_current_offset;
  // By default, C memory from malloc is not zeroed. Arena does not zero by
  // default either. memset(ptr, 0, item_size); // If you want to zero memory by
  // default
  return ptr;
}

// --- Vector Implementation ---
#if BASE_VECTOR_BOUNDS_CHECK
#define IF_BASE_VECTOR_BOUNDS_CHECK(check) check
#else
#define IF_BASE_VECTOR_BOUNDS_CHECK(check) ((void)0)
#endif

#define VECTOR_DEFINE(VecName, Type)                                           \
  VecName vec_##VecName##_init(usize initial_capacity) {                       \
    VecName vec;                                                               \
    vec.size = 0;                                                              \
    vec.capacity = (initial_capacity > 0) ? initial_capacity                   \
                                          : BASE_VECTOR_DEFAULT_CAPACITY;      \
    if (vec.capacity == 0 &&                                                   \
        BASE_VECTOR_DEFAULT_CAPACITY == 0) { /* Edge case for 0 cap */         \
      vec.capacity =                                                           \
          1; /* Ensure malloc(0) is not the primary path unless intended */    \
    }                                                                          \
    vec.data = (Type *)malloc(vec.capacity * sizeof(Type));                    \
    if (!vec.data &&                                                           \
        vec.capacity >                                                         \
            0) { /* Check if malloc actually failed for non-zero request */    \
      LOG_FATAL("Failed to allocate memory for " STRINGIFY(                    \
                    VecName) " (capacity %zu)",                                \
                vec.capacity);                                                 \
      vec.capacity = 0; /* Mark as unusable */                                 \
    } else if (!vec.data &&                                                    \
               vec.capacity ==                                                 \
                   0) { /* malloc(0) might return NULL, which is fine */       \
                        /* Or it might return a unique pointer. Behavior is    \
                         * implementation-defined. */                          \
      /* Consider vec.data = NULL explicitly if capacity is 0. */              \
    }                                                                          \
    return vec;                                                                \
  }                                                                            \
                                                                               \
  void vec_##VecName##_free(VecName *vec) {                                    \
    if (vec) {                                                                 \
      free(vec->data);                                                         \
      vec->data = NULL;                                                        \
      vec->size = 0;                                                           \
      vec->capacity = 0;                                                       \
    }                                                                          \
  }                                                                            \
                                                                               \
  bool vec_##VecName##_reserve(VecName *vec, usize new_capacity) {             \
    ASSERT(vec);                                                               \
    if (new_capacity <= vec->capacity) {                                       \
      return true;                                                             \
    }                                                                          \
    /* realloc(NULL, size) is like malloc(size) */                             \
    Type *new_data = (Type *)realloc(vec->data, new_capacity * sizeof(Type));  \
    if (!new_data &&                                                           \
        new_capacity > 0) { /* Check if realloc failed for non-zero request */ \
      LOG_ERROR("Failed to reallocate memory for " STRINGIFY(                  \
                    VecName) " to new capacity %zu",                           \
                new_capacity);                                                 \
      return false;                                                            \
    }                                                                          \
    vec->data = new_data;                                                      \
    vec->capacity = new_capacity;                                              \
    return true;                                                               \
  }                                                                            \
                                                                               \
  bool vec_##VecName##_push(VecName *vec, Type value) {                        \
    ASSERT(vec);                                                               \
    if (vec->size >= vec->capacity) {                                          \
      usize new_capacity =                                                     \
          vec->capacity > 0                                                    \
              ? (usize)((f64)vec->capacity * BASE_VECTOR_GROWTH_FACTOR)        \
              : BASE_VECTOR_DEFAULT_CAPACITY;                                  \
      if (new_capacity <=                                                      \
          vec->capacity) { /* Handle growth factor <= 1 or 0 capacity */       \
        new_capacity = vec->capacity + 1;                                      \
      }                                                                        \
      if (new_capacity == 0 && BASE_VECTOR_DEFAULT_CAPACITY == 0)              \
        new_capacity = 1; /* Ensure minimum capacity if default is 0 */        \
      if (!vec_##VecName##_reserve(vec, new_capacity)) {                       \
        return false;                                                          \
      }                                                                        \
    }                                                                          \
    /* If vec->data is NULL here and capacity > 0, reserve should have failed. \
     */                                                                        \
    /* If capacity is 0, vec->data might be NULL. This case is tricky if push  \
     * is attempted. */                                                        \
    /* The reserve call above should handle getting valid memory if capacity   \
     * was 0. */                                                               \
    ASSERT(vec->data ||                                                        \
           vec->capacity == 0); /* Data should be valid if capacity > 0 */     \
    if (!vec->data &&                                                          \
        vec->capacity > 0) { /* Should not happen if reserve worked */         \
      LOG_ERROR("Vector data is NULL despite non-zero capacity before push."); \
      return false;                                                            \
    }                                                                          \
    vec->data[vec->size++] = value;                                            \
    return true;                                                               \
  }                                                                            \
                                                                               \
  bool vec_##VecName##_pop(VecName *vec, Type *out_value) {                    \
    ASSERT(vec);                                                               \
    if (vec->size == 0) {                                                      \
      return false;                                                            \
    }                                                                          \
    vec->size--;                                                               \
    if (out_value) {                                                           \
      ASSERT(vec->data);                                                       \
      *out_value = vec->data[vec->size];                                       \
    }                                                                          \
    return true;                                                               \
  }                                                                            \
                                                                               \
  Type *vec_##VecName##_get(VecName *vec, usize index) {                       \
    ASSERT(vec);                                                               \
    IF_BASE_VECTOR_BOUNDS_CHECK(                                               \
        ASSERT_MSG(index < vec->size, "Index %zu out of bounds (size %zu)",    \
                   index, vec->size);)                                         \
    ASSERT(vec->data);                                                         \
    return &(vec->data[index]);                                                \
  }                                                                            \
                                                                               \
  const Type *vec_##VecName##_get_const(const VecName *vec, usize index) {     \
    ASSERT(vec);                                                               \
    IF_BASE_VECTOR_BOUNDS_CHECK(                                               \
        ASSERT_MSG(index < vec->size, "Index %zu out of bounds (size %zu)",    \
                   index, vec->size);)                                         \
    ASSERT(vec->data);                                                         \
    return &(vec->data[index]);                                                \
  }                                                                            \
                                                                               \
  void vec_##VecName##_set(VecName *vec, usize index, Type value) {            \
    ASSERT(vec);                                                               \
    IF_BASE_VECTOR_BOUNDS_CHECK(                                               \
        ASSERT_MSG(index < vec->size, "Index %zu out of bounds (size %zu)",    \
                   index, vec->size);)                                         \
    ASSERT(vec->data);                                                         \
    vec->data[index] = value;                                                  \
  }                                                                            \
                                                                               \
  bool vec_##VecName##_insert(VecName *vec, usize index, Type value) {         \
    ASSERT(vec);                                                               \
    IF_BASE_VECTOR_BOUNDS_CHECK(                                               \
        ASSERT_MSG(index <= vec->size, /* Allow insert at end */               \
                   "Insert index %zu out of bounds (size %zu)", index,         \
                   vec->size);)                                                \
    if (vec->size >= vec->capacity) {                                          \
      usize new_capacity =                                                     \
          vec->capacity > 0                                                    \
              ? (usize)((f64)vec->capacity * BASE_VECTOR_GROWTH_FACTOR)        \
              : BASE_VECTOR_DEFAULT_CAPACITY;                                  \
      if (new_capacity <= vec->capacity) {                                     \
        new_capacity = vec->capacity + 1;                                      \
      }                                                                        \
      if (new_capacity == 0 && BASE_VECTOR_DEFAULT_CAPACITY == 0)              \
        new_capacity = 1;                                                      \
      if (!vec_##VecName##_reserve(vec, new_capacity)) {                       \
        return false;                                                          \
      }                                                                        \
    }                                                                          \
    ASSERT(vec->data || vec->capacity == 0);                                   \
    if (!vec->data && vec->capacity > 0)                                       \
      return false;          /* Should not happen */                           \
    if (index < vec->size) { /* Shift elements if not inserting at the end */  \
      memmove(&vec->data[index + 1], &vec->data[index],                        \
              (vec->size - index) * sizeof(Type));                             \
    }                                                                          \
    vec->data[index] = value;                                                  \
    vec->size++;                                                               \
    return true;                                                               \
  }                                                                            \
                                                                               \
  bool vec_##VecName##_remove(VecName *vec, usize index, Type *out_value) {    \
    ASSERT(vec);                                                               \
    IF_BASE_VECTOR_BOUNDS_CHECK(                                               \
        ASSERT_MSG(index < vec->size,                                          \
                   "Remove index %zu out of bounds (size %zu)", index,         \
                   vec->size);)                                                \
    if (vec->size == 0)                                                        \
      return false;                                                            \
    ASSERT(vec->data);                                                         \
    if (out_value) {                                                           \
      *out_value = vec->data[index];                                           \
    }                                                                          \
    if (index < vec->size - 1) { /* Shift elements if not removing last one */ \
      memmove(&vec->data[index], &vec->data[index + 1],                        \
              (vec->size - index - 1) * sizeof(Type));                         \
    }                                                                          \
    vec->size--;                                                               \
    return true;                                                               \
  }                                                                            \
                                                                               \
  bool vec_##VecName##_resize(VecName *vec, usize new_size,                    \
                              Type default_value_if_expanding) {               \
    ASSERT(vec);                                                               \
    if (new_size > vec->capacity) {                                            \
      if (!vec_##VecName##_reserve(vec, new_size)) {                           \
        return false;                                                          \
      }                                                                        \
    }                                                                          \
    if (new_size > vec->size) { /* Expanding */                                \
      ASSERT(vec->data ||                                                      \
             new_size == 0); /* Data must exist if new_size > 0 */             \
      if (vec->data) {                                                         \
        for (usize i = vec->size; i < new_size; ++i) {                         \
          vec->data[i] = default_value_if_expanding;                           \
        }                                                                      \
      } else if (new_size > 0) { /* Should not happen if reserve worked */     \
        LOG_ERROR("Vector data is NULL during resize expansion.");             \
        return false;                                                          \
      }                                                                        \
    }                                                                          \
    vec->size = new_size;                                                      \
    return true;                                                               \
  }                                                                            \
                                                                               \
  void vec_##VecName##_clear(VecName *vec) {                                   \
    ASSERT(vec);                                                               \
    vec->size = 0; /* Does not free memory, just resets size */                \
  }                                                                            \
                                                                               \
  bool vec_##VecName##_shrink_to_fit(VecName *vec) {                           \
    ASSERT(vec);                                                               \
    if (vec->size == vec->capacity) {                                          \
      return true;                                                             \
    }                                                                          \
    if (vec->size == 0) {                                                      \
      free(vec->data);                                                         \
      vec->data = NULL;                                                        \
      vec->capacity = 0;                                                       \
    } else {                                                                   \
      Type *new_data = (Type *)realloc(vec->data, vec->size * sizeof(Type));   \
      if (!new_data) { /* realloc can fail even when shrinking if size is 0,   \
                          though unlikely for >0 */                            \
        LOG_WARN("Failed to shrink " STRINGIFY(VecName) " to size %zu",        \
                 vec->size);                                                   \
        return false; /* Failed to shrink, but vector is still valid */        \
      }                                                                        \
      vec->data = new_data;                                                    \
      vec->capacity = vec->size;                                               \
    }                                                                          \
    return true;                                                               \
  }                                                                            \
                                                                               \
  VecName vec_##VecName##_copy(const VecName *src_vec) {                       \
    ASSERT(src_vec);                                                           \
    VecName dst_vec = vec_##VecName##_init(src_vec->size);                     \
    if (!dst_vec.data && src_vec->size > 0) {                                  \
      /* Initialization failed (LOG_FATAL would have been called by _init) */  \
      /* but if init changes, this is a fallback. */                           \
      LOG_ERROR("Failed to initialize copy of " STRINGIFY(VecName));           \
      dst_vec.size = dst_vec.capacity = 0; /* Ensure it's invalid */           \
      return dst_vec;                                                          \
    }                                                                          \
    if (src_vec->size > 0 && src_vec->data) {                                  \
      memcpy(dst_vec.data, src_vec->data, src_vec->size * sizeof(Type));       \
      dst_vec.size = src_vec->size;                                            \
    } else {                                                                   \
      dst_vec.size = 0; /* Source was empty */                                 \
    }                                                                          \
    return dst_vec;                                                            \
  }                                                                            \
                                                                               \
  bool vec_##VecName##_append_array(VecName *vec, const Type *arr,             \
                                    usize count) {                             \
    ASSERT(vec);                                                               \
    if (count == 0)                                                            \
      return true;                                                             \
    ASSERT(arr);                                                               \
    usize new_size = vec->size + count;                                        \
    if (new_size > vec->capacity) {                                            \
      usize new_capacity = vec->capacity;                                      \
      if (new_capacity == 0)                                                   \
        new_capacity = BASE_VECTOR_DEFAULT_CAPACITY;                           \
      if (new_capacity == 0)                                                   \
        new_capacity = 1; /* Min capacity */                                   \
      while (new_capacity < new_size) {                                        \
        new_capacity = (usize)((f64)new_capacity * BASE_VECTOR_GROWTH_FACTOR); \
        if (new_capacity <= vec->capacity)                                     \
          new_capacity = vec->capacity + 1; /* Ensure growth */                \
      }                                                                        \
      if (!vec_##VecName##_reserve(vec, new_capacity)) {                       \
        return false;                                                          \
      }                                                                        \
    }                                                                          \
    ASSERT(vec->data);                                                         \
    memcpy(&vec->data[vec->size], arr, count * sizeof(Type));                  \
    vec->size = new_size;                                                      \
    return true;                                                               \
  }

// Instantiate vector for char (used by BaseString)
VECTOR_DEFINE(StringData, char)

// --- BaseString Implementation ---
bool bstr_ensure_capacity(BaseString *bstr, usize additional_chars_needed) {
  ASSERT(bstr);
  // Need space for current_length + additional_chars + null_terminator
  usize required_capacity = bstr->size + additional_chars_needed + 1;
  if (required_capacity > bstr->capacity) {
    usize new_capacity = bstr->capacity;
    if (new_capacity == 0)
      new_capacity = BASE_VECTOR_DEFAULT_CAPACITY > 0
                         ? BASE_VECTOR_DEFAULT_CAPACITY
                         : 8; // Sensible minimum if default is 0
    while (new_capacity < required_capacity) {
      new_capacity = (usize)((f64)new_capacity * BASE_VECTOR_GROWTH_FACTOR);
      if (new_capacity <= bstr->capacity)
        new_capacity = bstr->capacity + 1; // Ensure growth
    }
    return vec_StringData_reserve(bstr, new_capacity);
  }
  return true;
}

BaseString bstr_from_cstr(const char *cstr) {
  ASSERT(cstr);
  usize len = strlen(cstr);
  BaseString bstr = vec_StringData_init(len + 1); // +1 for null terminator
  if (bstr.data) {
    memcpy(bstr.data, cstr, len);
    bstr.data[len] = '\0';
    bstr.size = len; // Vector size is actual string length
  }
  return bstr;
}

BaseString bstr_from_sv(StringView sv) {
  BaseString bstr =
      vec_StringData_init(sv.length + 1); // +1 for null terminator
  if (bstr.data && sv.data) {
    memcpy(bstr.data, sv.data, sv.length);
    bstr.data[sv.length] = '\0';
    bstr.size = sv.length;
  } else if (bstr.data) { // sv.data is NULL or sv.length is 0
    bstr.data[0] = '\0';
    bstr.size = 0;
  }
  return bstr;
}

bool bstr_append_cstr(BaseString *bstr, const char *cstr) {
  ASSERT(bstr && cstr);
  usize len_to_append = strlen(cstr);
  if (len_to_append == 0)
    return true;

  if (!bstr_ensure_capacity(bstr, len_to_append)) {
    return false;
  }
  memcpy(bstr->data + bstr->size, cstr, len_to_append);
  bstr->size += len_to_append;
  bstr->data[bstr->size] = '\0';
  return true;
}

bool bstr_append_sv(BaseString *bstr, StringView sv) {
  ASSERT(bstr);
  if (sv.length == 0)
    return true;
  ASSERT(sv.data);

  if (!bstr_ensure_capacity(bstr, sv.length)) {
    return false;
  }
  memcpy(bstr->data + bstr->size, sv.data, sv.length);
  bstr->size += sv.length;
  bstr->data[bstr->size] = '\0';
  return true;
}

// --- Hash Table (Open Addressing) Helper: Rehashing ---
static bool ht_oa_rehash(BaseHashTableOA *ht, usize new_bucket_count) {
  ASSERT(ht);
  if (new_bucket_count == 0)
    new_bucket_count = BASE_HT_OA_DEFAULT_CAPACITY;
  if (new_bucket_count < ht->item_count)
    new_bucket_count = ht->item_count * 2; // Ensure enough space

  HashTableEntryOA *old_entries = ht->entries;
  usize old_bucket_count = ht->bucket_count;

  HashTableEntryOA *new_entries_ptr = NULL;
  if (ht->arena) {
    new_entries_ptr = (HashTableEntryOA *)arena_alloc(
        ht->arena, new_bucket_count * sizeof(HashTableEntryOA));
  } else {
    new_entries_ptr =
        (HashTableEntryOA *)malloc(new_bucket_count * sizeof(HashTableEntryOA));
  }

  if (!new_entries_ptr) {
    LOG_ERROR("Hash table (OA) rehash failed: could not allocate new entries "
              "array (count %zu).",
              new_bucket_count);
    return false;
  }
  for (usize i = 0; i < new_bucket_count; ++i) { // Initialize new entries
    new_entries_ptr[i].state = HT_ENTRY_EMPTY;
    new_entries_ptr[i].key = NULL;
    new_entries_ptr[i].value = NULL;
  }

  ht->entries = new_entries_ptr;
  ht->bucket_count = new_bucket_count;
  ht->item_count = 0;      // Will be recounted
  ht->tombstone_count = 0; // Tombstones are not carried over

  for (usize i = 0; i < old_bucket_count; ++i) {
    if (old_entries[i].state == HT_ENTRY_OCCUPIED) {
      // Re-insert into the new table. ht_oa_put handles rehashing internally if
      // needed, but for a direct rehash, we call a simplified insert logic. We
      // know the key is unique here.
      void *key = old_entries[i].key;
      void *value = old_entries[i].value;

      u64 hash = ht->hash_fn(key);
      usize start_idx = hash % ht->bucket_count;
      usize current_idx = start_idx;
      bool inserted = false;

      do {
        if (ht->entries[current_idx].state == HT_ENTRY_EMPTY) {
          ht->entries[current_idx].key = key;
          ht->entries[current_idx].value = value;
          ht->entries[current_idx].state = HT_ENTRY_OCCUPIED;
          ht->item_count++;
          inserted = true;
          break;
        }
        current_idx = (current_idx + 1) % ht->bucket_count; // Linear probe
      } while (current_idx != start_idx && !inserted);

      ASSERT_MSG(inserted, "Rehash failed to insert an existing occupied item. "
                           "Table full or bug.");
      // If !inserted, it's a bug or table became full during rehash (should not
      // happen with proper new_bucket_count).
    }
  }

  if (!ht->arena && old_entries) {
    free(old_entries);
  }
  // If old_entries from arena, memory is handled by arena.
  return true;
}

// --- Hash Table (Open Addressing) Helper: Find Slot ---
// Returns pointer to the slot where the key is or should be.
// Sets 'out_found_existing' if key was found in an OCCUPIED slot.
static HashTableEntryOA *ht_oa_find_slot(BaseHashTableOA *ht, const void *key,
                                         bool *out_found_existing) {
  ASSERT(ht && ht->entries && key);
  *out_found_existing = false;
  if (ht->bucket_count == 0)
    return NULL; // Cannot find in an uninitialized/empty table

  u64 hash = ht->hash_fn(key);
  usize start_idx = hash % ht->bucket_count;
  usize current_idx = start_idx;
  HashTableEntryOA *first_tombstone_slot = NULL;

  do {
    HashTableEntryOA *entry = &ht->entries[current_idx];
    if (entry->state == HT_ENTRY_EMPTY) {
      // Key not found. If we passed a tombstone, return that for insertion.
      return first_tombstone_slot ? first_tombstone_slot : entry;
    } else if (entry->state == HT_ENTRY_TOMBSTONE) {
      if (!first_tombstone_slot) {
        first_tombstone_slot = entry; // Remember first available tombstone
      }
    } else { // HT_ENTRY_OCCUPIED
      if (ht->key_equal_fn(entry->key, key)) {
        *out_found_existing = true;
        return entry; // Key found
      }
    }
    current_idx = (current_idx + 1) % ht->bucket_count; // Linear probe
  } while (current_idx != start_idx);

  // Table is full of occupied entries (that don't match) and/or tombstones.
  // If we found a tombstone, we can use it for insertion. Otherwise, table is
  // truly full for this key.
  return first_tombstone_slot; // Returns NULL if no empty/tombstone slot found
                               // after full scan.
}

// --- Hash Table (Open Addressing) API Implementation ---
BaseHashTableOA ht_oa_init(usize initial_bucket_count, BaseHashFunction hash_fn,
                           BaseKeyEqualFunction key_equal_fn,
                           Arena *optional_arena) {
  ASSERT(hash_fn && key_equal_fn);
  BaseHashTableOA ht = {0};
  ht.hash_fn = hash_fn;
  ht.key_equal_fn = key_equal_fn;
  ht.arena = optional_arena;

  if (initial_bucket_count == 0)
    initial_bucket_count = BASE_HT_OA_DEFAULT_CAPACITY;
  ht.bucket_count = initial_bucket_count;

  if (ht.arena) {
    ht.entries = (HashTableEntryOA *)arena_alloc(
        ht.arena, ht.bucket_count * sizeof(HashTableEntryOA));
  } else {
    ht.entries =
        (HashTableEntryOA *)malloc(ht.bucket_count * sizeof(HashTableEntryOA));
  }

  if (!ht.entries) {
    LOG_FATAL(
        "Hash table (OA) init failed: could not allocate entries (count %zu).",
        ht.bucket_count);
    ht.bucket_count = 0;
  } else {
    for (usize i = 0; i < ht.bucket_count; ++i) {
      ht.entries[i].state = HT_ENTRY_EMPTY;
      ht.entries[i].key = NULL;
      ht.entries[i].value = NULL;
    }
  }
  return ht;
}

void ht_oa_set_managed_kv(BaseHashTableOA *ht, BaseCopyFunction key_copy_fn,
                          BaseFreeFunction key_free_fn,
                          BaseCopyFunction value_copy_fn,
                          BaseFreeFunction value_free_fn) {
  ASSERT(ht);
  ht->key_copy_fn = key_copy_fn;
  ht->key_free_fn = key_free_fn;
  ht->value_copy_fn = value_copy_fn;
  ht->value_free_fn = value_free_fn;
}

void ht_oa_clear(BaseHashTableOA *ht) {
  ASSERT(ht);
  for (usize i = 0; i < ht->bucket_count; ++i) {
    if (ht->entries[i].state == HT_ENTRY_OCCUPIED) {
      if (ht->key_free_fn && ht->entries[i].key)
        ht->key_free_fn(ht->entries[i].key, ht->arena);
      if (ht->value_free_fn && ht->entries[i].value)
        ht->value_free_fn(ht->entries[i].value, ht->arena);
    }
    ht->entries[i].state = HT_ENTRY_EMPTY; // Mark as empty, not tombstone
    ht->entries[i].key = NULL;
    ht->entries[i].value = NULL;
  }
  ht->item_count = 0;
  ht->tombstone_count = 0;
}

void ht_oa_free(BaseHashTableOA *ht) {
  ASSERT(ht);
  ht_oa_clear(ht); // Free managed K/V
  if (!ht->arena && ht->entries) {
    free(ht->entries);
  }
  ht->entries = NULL;
  ht->bucket_count = 0;
  // item_count and tombstone_count already zeroed by clear
}

bool ht_oa_insert(BaseHashTableOA *ht, const void *key, void *value) {
  ASSERT(ht && key);
  if (ht->bucket_count == 0) { // Uninitialized or fully freed table
    if (!ht_oa_rehash(ht, BASE_HT_OA_DEFAULT_CAPACITY))
      return false; // Attempt to initialize
  }

  // Check load factor, including tombstones as they affect probe length
  f64 current_load =
      (f64)(ht->item_count + ht->tombstone_count) / (f64)ht->bucket_count;
  if (ht->item_count + 1 >= ht->bucket_count ||
      current_load > BASE_HT_OA_MAX_LOAD_FACTOR) {
    if (!ht_oa_rehash(ht, ht->bucket_count > 0 ? ht->bucket_count * 2
                                               : BASE_HT_OA_DEFAULT_CAPACITY)) {
      return false; // Rehash failed
    }
  }

  bool found_existing;
  HashTableEntryOA *slot = ht_oa_find_slot(ht, key, &found_existing);

  if (found_existing) {
    return false; // Key already exists
  }

  if (!slot || slot->state == HT_ENTRY_OCCUPIED) {
    // No empty/tombstone slot found OR ht_oa_find_slot returned an occupied
    // slot that wasn't the target This implies table is full or a logic error
    // in find_slot if it returns occupied on non-found. After a rehash due to
    // load factor, we should always find a slot unless it's a bug. If we reach
    // here, it might mean the table is full of non-matching items and no
    // tombstones.
    LOG_ERROR("Hash table (OA) insert: No available slot found after load "
              "check (table full or bug).");
    // Attempt one more rehash if all slots are occupied (no tombstones were
    // usable)
    if (ht->item_count == ht->bucket_count && ht->tombstone_count == 0) {
      if (!ht_oa_rehash(ht, ht->bucket_count * 2))
        return false;
      slot =
          ht_oa_find_slot(ht, key, &found_existing); // Try finding slot again
      if (found_existing || !slot || slot->state == HT_ENTRY_OCCUPIED)
        return false; // Still no luck
    } else {
      return false;
    }
  }

  // Found an empty or tombstone slot for insertion
  void *new_key_ptr = (void *)key;
  void *new_value_ptr = value;

  if (ht->key_copy_fn) {
    new_key_ptr = ht->key_copy_fn(key, ht->arena);
    if (!new_key_ptr) {
      LOG_ERROR("HT (OA) insert: key_copy_fn failed.");
      return false;
    }
  }
  if (ht->value_copy_fn) {
    new_value_ptr = ht->value_copy_fn(value, ht->arena);
    if (!new_value_ptr) {
      LOG_ERROR("HT (OA) insert: value_copy_fn failed.");
      if (ht->key_copy_fn && ht->key_free_fn && new_key_ptr != key) {
        ht->key_free_fn(new_key_ptr, ht->arena);
      }
      return false;
    }
  }

  if (slot->state == HT_ENTRY_TOMBSTONE) {
    ht->tombstone_count--; // Reusing a tombstone
  }
  slot->key = new_key_ptr;
  slot->value = new_value_ptr;
  slot->state = HT_ENTRY_OCCUPIED;
  ht->item_count++;
  return true;
}

bool ht_oa_put(BaseHashTableOA *ht, const void *key, void *value) { // Upsert
  ASSERT(ht && key);
  if (ht->bucket_count == 0) {
    if (!ht_oa_rehash(ht, BASE_HT_OA_DEFAULT_CAPACITY))
      return false;
  }

  // Load factor check, similar to insert
  // Add 1 only if key is not present (item_count would increase)
  // For simplicity, check assuming a new item might be added.
  f64 current_load = (f64)(ht->item_count + ht->tombstone_count +
                           (ht_oa_contains(ht, key) ? 0 : 1)) /
                     (f64)ht->bucket_count;
  if (ht->item_count + (ht_oa_contains(ht, key) ? 0 : 1) >= ht->bucket_count ||
      current_load > BASE_HT_OA_MAX_LOAD_FACTOR) {
    if (!ht_oa_rehash(ht, ht->bucket_count > 0 ? ht->bucket_count * 2
                                               : BASE_HT_OA_DEFAULT_CAPACITY)) {
      return false;
    }
  }

  bool found_existing;
  HashTableEntryOA *slot = ht_oa_find_slot(ht, key, &found_existing);

  if (found_existing) { // Key exists, update value
    void *old_value = slot->value;
    void *new_value_ptr = value;

    if (ht->value_copy_fn) {
      new_value_ptr = ht->value_copy_fn(value, ht->arena);
      if (!new_value_ptr) {
        LOG_ERROR("HT (OA) put: value_copy_fn failed for update.");
        return false;
      }
    }

    slot->value = new_value_ptr;

    if (ht->value_free_fn && old_value && old_value != value) {
      if (!ht->value_copy_fn ||
          (ht->value_copy_fn && old_value != new_value_ptr)) {
        ht->value_free_fn(old_value, ht->arena);
      }
    }
    return true;
  }

  // Key does not exist, insert (similar to ht_oa_insert)
  if (!slot || slot->state == HT_ENTRY_OCCUPIED) {
    LOG_ERROR("Hash table (OA) put: No available slot for new key (table full "
              "or bug).");
    if (ht->item_count == ht->bucket_count &&
        ht->tombstone_count == 0) { // Try one more rehash if truly full
      if (!ht_oa_rehash(ht, ht->bucket_count * 2))
        return false;
      slot = ht_oa_find_slot(ht, key, &found_existing);
      if (found_existing || !slot || slot->state == HT_ENTRY_OCCUPIED)
        return false;
    } else {
      return false;
    }
  }

  void *new_key_ptr = (void *)key;
  void *new_value_ptr = value;

  if (ht->key_copy_fn) {
    new_key_ptr = ht->key_copy_fn(key, ht->arena);
    if (!new_key_ptr) {
      LOG_ERROR("HT (OA) put: key_copy_fn failed.");
      return false;
    }
  }
  if (ht->value_copy_fn) {
    new_value_ptr = ht->value_copy_fn(value, ht->arena);
    if (!new_value_ptr) {
      LOG_ERROR("HT (OA) put: value_copy_fn failed.");
      if (ht->key_copy_fn && ht->key_free_fn && new_key_ptr != key) {
        ht->key_free_fn(new_key_ptr, ht->arena);
      }
      return false;
    }
  }

  if (slot->state == HT_ENTRY_TOMBSTONE) {
    ht->tombstone_count--;
  }
  slot->key = new_key_ptr;
  slot->value = new_value_ptr;
  slot->state = HT_ENTRY_OCCUPIED;
  ht->item_count++;
  return true;
}

void *ht_oa_get(const BaseHashTableOA *ht, const void *key) {
  ASSERT(ht && key);
  if (ht->bucket_count == 0 || ht->item_count == 0)
    return NULL;

  // Need a const version of find_slot or replicate logic carefully
  u64 hash = ht->hash_fn(key);
  usize start_idx = hash % ht->bucket_count;
  usize current_idx = start_idx;

  do {
    const HashTableEntryOA *entry = &ht->entries[current_idx];
    if (entry->state == HT_ENTRY_EMPTY) {
      return NULL; // Key not found
    } else if (entry->state == HT_ENTRY_OCCUPIED) {
      if (ht->key_equal_fn(entry->key, key)) {
        return entry->value; // Key found
      }
    }
    // If TOMBSTONE, continue probing
    current_idx = (current_idx + 1) % ht->bucket_count;
  } while (current_idx != start_idx);

  return NULL; // Scanned full table
}

bool ht_oa_contains(const BaseHashTableOA *ht, const void *key) {
  return ht_oa_get(ht, key) != NULL;
}

bool ht_oa_remove(BaseHashTableOA *ht, const void *key) {
  ASSERT(ht && key);
  if (ht->bucket_count == 0 || ht->item_count == 0)
    return false;

  bool found_existing;
  HashTableEntryOA *slot = ht_oa_find_slot(ht, key, &found_existing);

  if (found_existing && slot && slot->state == HT_ENTRY_OCCUPIED) {
    if (ht->key_free_fn && slot->key)
      ht->key_free_fn(slot->key, ht->arena);
    if (ht->value_free_fn && slot->value)
      ht->value_free_fn(slot->value, ht->arena);

    slot->state = HT_ENTRY_TOMBSTONE;
    slot->key = NULL;   // Good practice to nullify
    slot->value = NULL; // Good practice
    ht->item_count--;
    ht->tombstone_count++;
    return true;
  }
  return false; // Key not found
}

usize ht_oa_size(const BaseHashTableOA *ht) {
  ASSERT(ht);
  return ht->item_count;
}

bool ht_oa_is_empty(const BaseHashTableOA *ht) {
  ASSERT(ht);
  return ht->item_count == 0;
}

// --- Iterator for Open Addressing ---
HashTableIteratorOA ht_oa_iterator_begin(const BaseHashTableOA *ht) {
  ASSERT(ht);
  HashTableIteratorOA iter = {.ht = ht, .current_idx = 0};
  // Advance to first occupied slot
  while (iter.current_idx < ht->bucket_count &&
         ht->entries[iter.current_idx].state != HT_ENTRY_OCCUPIED) {
    iter.current_idx++;
  }
  return iter;
}

bool ht_oa_iterator_next(HashTableIteratorOA *iter, const void **out_key,
                         void **out_value) {
  ASSERT(iter && iter->ht);

  if (iter->current_idx >= iter->ht->bucket_count) {
    return false; // Already past the end
  }

  // Current slot at iter->current_idx is known to be OCCUPIED from previous
  // call or begin
  if (out_key)
    *out_key = iter->ht->entries[iter->current_idx].key;
  if (out_value)
    *out_value = iter->ht->entries[iter->current_idx].value;

  // Advance to find the next occupied slot
  iter->current_idx++;
  while (iter->current_idx < iter->ht->bucket_count &&
         iter->ht->entries[iter->current_idx].state != HT_ENTRY_OCCUPIED) {
    iter->current_idx++;
  }
  return true; // Return true because current item was valid.
               // If loop finishes and current_idx >= bucket_count, next call
               // will return false.
}

// --- Common Hash/Equal Functions Implementation ---
#define FNV_OFFSET_BASIS_64 0xCBF29CE484222325ULL
#define FNV_PRIME_64 0x100000001B3ULL

u64 base_hash_string(const void *key) { // Assumes key is const char*
  const char *str = (const char *)key;
  u64 hash = FNV_OFFSET_BASIS_64;
  if (!str)
    return hash; // Or a specific hash for NULL if that's a valid distinct key
  for (const char *p = str; *p; p++) {
    hash ^= (u64)(*p);
    hash *= FNV_PRIME_64;
  }
  return hash;
}

bool base_key_equal_string(const void *key1,
                           const void *key2) { // Assumes const char*
  const char *s1 = (const char *)key1;
  const char *s2 = (const char *)key2;
  if (s1 == s2)
    return true;
  if (!s1 || !s2)
    return false;
  return strcmp(s1, s2) == 0;
}

u64 base_hash_bytes(const void *key, usize len) {
  const u8 *bytes = (const u8 *)key;
  u64 hash = FNV_OFFSET_BASIS_64;
  for (usize i = 0; i < len; ++i) {
    hash ^= bytes[i];
    hash *= FNV_PRIME_64;
  }
  return hash;
}

#endif // BASE_IMPLEMENTATION
