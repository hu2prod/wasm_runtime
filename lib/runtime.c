#include "runtime.h"
////////////////////////////////////////////////////////////////////////////////////////////////////
// special env
////////////////////////////////////////////////////////////////////////////////////////////////////
__attribute__((visibility("default"))) u32 HISTORY_DEPTH = 1;
__attribute__((visibility("default"))) u32 SIZE_X = 1;
__attribute__((visibility("default"))) u32 SIZE_Y = 1;
__attribute__((visibility("default"))) u32 SIZE_XY = 1;
__attribute__((visibility("default"))) u32 SIZE_XY_PX = 3;
////////////////////////////////////////////////////////////////////////////////////////////////////
// bump allocator
////////////////////////////////////////////////////////////////////////////////////////////////////
// TODO get __heap_base from global
// extern unsigned char __heap_base;
// unsigned int bump_pointer = &__heap_base;

size_t bump_pointer = 0;
size_t __heap_start = 0; // т.к. heap_base не доступна из под clang

// TODO remove
__attribute__((visibility("default")))
void __heap_set(size_t _bump_pointer) {
  bump_pointer = _bump_pointer;
  __heap_start = _bump_pointer;
}

u32 total_alloc_size = 0;
u32 total_alloc_size_page = 0;
u32 page_size = 16*1024;
__attribute__((visibility("default")))
extern void* ws_malloc(u32 size, char* tag) {
  // putsi("ws_malloc  ", size);
  // putssi("ws_malloc", tag, size);
  total_alloc_size += size;
  u32 new_page_size = total_alloc_size/page_size;
  u32 page_diff = new_page_size - total_alloc_size_page;
  total_alloc_size_page = new_page_size;
  
  if (page_diff) {
    putsi("grow", page_diff);
    __builtin_wasm_memory_grow(0, page_diff);
  }
  unsigned int r = bump_pointer;
  bump_pointer += ((size / MEMORY_ALIGN) + 1) * MEMORY_ALIGN;
  return (void *)r;
}

void ws_free(void* ptr) {
  // NOPE
  putsi("free giveup memory leak", (i32)ptr);
}

u32 __alloc_log2_bucket_size  = 32;
u32 __alloc_log2_bucket_count = 32;
void*** __array_log2_list;
__attribute__((visibility("default")))
void __alloc_init() {
  // mini  inject
  u32 size  = __alloc_log2_bucket_count;
  size += 16; // mimic array (-3 length)
  __array_log2_list = (void***)ws_malloc(size, "__alloc_init");
  for(u32 i=0;i<__alloc_log2_bucket_count;i++) {
    // -1 REF_COUNT       override -> start_pointer
    // -2 RTTI            override -> end_pointer
    // -3 LEN             override (NOT USED)
    // -4 CAP (RESERVED)  override (NOT USED)
    __array_log2_list[i] = (void**)ws_malloc(sizeof(size_t)*__alloc_log2_bucket_size + 16, "__alloc_init");
  }
}
////////////////////////////////////////////////////////////////////////////////////////////////////

const i32 CAPACITY_OFFSET = -4;
const i32 LENGTH_OFFSET   = -3;
const i32 RTTI_OFFSET     = -2;
const i32 REF_COUNT_OFFSET= -1;

void* __alloc_ref(u32 size) {
  // rtti
  // ref_count
  size += 8;
  size_t r = (size_t)ws_malloc(size, "__alloc_ref");
  r += 8;
  return (void *)r;
}

void** __alloc_array(u32 size, const char* tag) {
  // super simple pool
  if (__builtin_popcount(size) != 1) {
    putsi("suboptimal alloc size", size);
  } else {
    u32 log2 = __builtin_clzl(size);
    size_t** bucket = (size_t**)__array_log2_list[log2];
    size_t* bucket_view_size_t= (size_t*)bucket;
    size_t bucket_start  = bucket_view_size_t[-1];
    size_t bucket_end    = bucket_view_size_t[-2];
    
    // if (bucket_end == bucket_start) {
      // putsi("bucket_end == bucket_start size", size);
    // }
    if (bucket_start != bucket_end) {
      bucket_view_size_t[-1] = (bucket_start + 1) % __alloc_log2_bucket_count;
      return (void**)bucket_view_size_t[bucket_start];
    }
  }
  
  // logssi("__alloc_array", tag, size);
  // capacity
  // length
  // rtti
  // ref_count
  size += 16;
  size_t r = (size_t)ws_malloc(size, "__alloc_array");
  r += 16;
  return (void **)r;
}

void __free_array(void** ptr) {
  size_t* ptr_view_size_t = (size_t*)ptr;
  size_t capacity = ptr_view_size_t[CAPACITY_OFFSET];
  if (__builtin_popcount(capacity) != 1) {
    putsi("fragmentation of bad alloc size", capacity);
    ws_free(ptr);
  } else {
    u32 log2 = __builtin_clzl(capacity);
    size_t** bucket = (size_t**)__array_log2_list[log2];
    size_t* bucket_view_size_t= (size_t*)bucket;
    size_t bucket_start  = bucket_view_size_t[-1];
    size_t bucket_end    = bucket_view_size_t[-2];
    size_t bucket_new_end = (bucket_end+1) % __alloc_log2_bucket_count;
    if (bucket_start == bucket_new_end) {
      ws_free(ptr); // give up
      return;
    }
    bucket_view_size_t[bucket_end] = (size_t)ptr;
    
    // bucket_end = bucket_new_end;
    bucket_view_size_t[-2] = bucket_new_end;
  }
}
////////////////////////////////////////////////////////////////////////////////////////////////////
// outside helper
////////////////////////////////////////////////////////////////////////////////////////////////////

u32 debug_counter = 0;

__attribute__((visibility("default")))
u32 u32_get(u32* t) {
  return *t;
}

__attribute__((visibility("default")))
void u32_set(u32* t, u32 val) {
  *t = val;
}