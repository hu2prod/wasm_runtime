#pragma once
#include "type.h"
#include "math.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// special env
////////////////////////////////////////////////////////////////////////////////////////////////////
extern u32 HISTORY_DEPTH ;
extern u32 SIZE_X;
extern u32 SIZE_Y;
extern u32 SIZE_XY;
extern u32 SIZE_XY_PX;
////////////////////////////////////////////////////////////////////////////////////////////////////
//  env
////////////////////////////////////////////////////////////////////////////////////////////////////

// for temp
void logi( i32 n );
void logsi( const char* str, i32 n );
void logssi( const char* str, char* str2, i32 n );
void logsf( const char* str, f32 n );

// for persistant
void puti( i32 n );
void putsi( const char* str, i32 n );
void putssi( const char* str, char* str2, i32 n );
void putsf( const char* str, f32 n );

void user_throw( const char* str );

// BGR_FRAME -> dst
i32 h2d(u8* dst, u32 history_offset);
// src -> WASM_RGB_FRAME
i32 d2h(u8* src);

////////////////////////////////////////////////////////////////////////////////////////////////////
// bump allocator
////////////////////////////////////////////////////////////////////////////////////////////////////

#define MEMORY_ALIGN (8)

// TODO get __heap_base from global
extern size_t bump_pointer;
extern size_t __heap_start; // т.к. heap_base не доступна из под clang
extern void __heap_set(size_t _bump_pointer);

extern u32 total_alloc_size;
extern u32 total_alloc_size_page;
extern u32 page_size;
extern void* ws_malloc(u32 size, char* tag);
extern void ws_free(void* ptr);

extern u32 __alloc_log2_bucket_size;
extern u32 __alloc_log2_bucket_count;
extern void*** __array_log2_list;
extern void __alloc_init();
////////////////////////////////////////////////////////////////////////////////////////////////////

extern const i32 CAPACITY_OFFSET ;
extern const i32 LENGTH_OFFSET   ;
extern const i32 RTTI_OFFSET     ;
extern const i32 REF_COUNT_OFFSET;

extern void* __alloc_ref(u32 size);

extern void** __alloc_array(u32 size, const char* tag);
extern void __free_array(void** ptr);

////////////////////////////////////////////////////////////////////////////////////////////////////

inline
void* __REF(void* ptr) {
  size_t* size_t_view = (size_t*)ptr;
  size_t_view[REF_COUNT_OFFSET]++;
  return ptr;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
// EXPERIMENTAL
////////////////////////////////////////////////////////////////////////////////////////////////////

#define PPCAT_NX(A, B) A ## B
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define OBJECT_POOL_CONSTRUCTOR(CLASS_NAME)                                                     \
  static struct CLASS_NAME* PPCAT_NX(__free_list_, CLASS_NAME) = 0;                             \
                                                                                                \
  struct CLASS_NAME* PPCAT_NX(new_, CLASS_NAME)() {                                             \
    if (!PPCAT_NX(__free_list_, CLASS_NAME)) {                                                  \
      PPCAT_NX(__free_list_, CLASS_NAME) = (struct CLASS_NAME*)array_size_t_alloc(1024);        \
    } else {                                                                                    \
      if (array_size_t_length_get((void**)PPCAT_NX(__free_list_, CLASS_NAME))) {                \
        return (struct CLASS_NAME*)array_size_t_pop_unsafe((void**)PPCAT_NX(__free_list_, CLASS_NAME)); \
      }                                                                                         \
    }                                                                                           \
    void* ref = __alloc_ref(sizeof(struct CLASS_NAME));                                         \
    size_t* size_t_view = (size_t*)ref;                                                         \
    size_t_view[RTTI_OFFSET] = PPCAT_NX(RTTI_, CLASS_NAME);                                     \
    size_t_view[REF_COUNT_OFFSET] = 1;                                                          \
    return (struct CLASS_NAME*)ref;                                                             \
  }                                                                                             \
                                                                                                \
  void PPCAT_NX(free_, CLASS_NAME)(struct CLASS_NAME* ref) {                                    \
    u32* size_t_view = (u32*)ref;                                                               \
    if (size_t_view[RTTI_OFFSET] != PPCAT_NX(RTTI_, CLASS_NAME)) {                              \
      putsi("pointer  = ", (size_t)ref);                                                        \
      putsi("RTTI     = ", size_t_view[RTTI_OFFSET]);                                           \
      user_throw(TOSTRING(PPCAT_NX(free_bad_RTTI_, CLASS_NAME)));                               \
      return;                                                                                   \
    }                                                                                           \
    if (size_t_view[REF_COUNT_OFFSET] > 1) {                                                    \
      size_t_view[REF_COUNT_OFFSET]--;                                                          \
      return;                                                                                   \
    }                                                                                           \
    PPCAT_NX(__free_list_, CLASS_NAME) = (struct CLASS_NAME*)array_size_t_push((void**)PPCAT_NX(__free_list_, CLASS_NAME), (size_t)ref); \
  }

////////////////////////////////////////////////////////////////////////////////////////////////////
// outside helper
////////////////////////////////////////////////////////////////////////////////////////////////////

extern u32 debug_counter;

extern u32 u32_get(u32* t);
extern void u32_set(u32* t, u32 val);
