#pragma once
#include "type.h"
extern const u32 RTTI_ARRAY_SIZE_T;

extern void** array_size_t_alloc(size_t size);
extern void array_size_t_free(void** array_ptr);
extern void** array_size_t_realloc(void** array_ptr, size_t size);
inline void array_size_t_length_set_unsafe(void** array_ptr, size_t length);
extern void** array_size_t_length_set_realloc(void** array_ptr, size_t length);

inline
void array_size_t_clear(void** array_ptr) {
  array_size_t_length_set_unsafe(array_ptr, 0);
}
void array_size_t_alloc_or_clear(void*** array_ptr, size_t size);

inline
size_t array_size_t_capacity_get(void** array_ptr) {
  size_t* size_t_view = (size_t*)array_ptr;
  return size_t_view[CAPACITY_OFFSET];
}

inline
size_t array_size_t_length_get(void** array_ptr) {
  size_t* size_t_view = (size_t*)array_ptr;
  return size_t_view[LENGTH_OFFSET];
}

extern size_t export_array_size_t_capacity_get(void** array_ptr);
extern size_t export_array_size_t_length_get(void** array_ptr);

// DO not set to zero all data if length > old_length
// DO not free data            if length < old_length
inline
void array_size_t_length_set_unsafe(void** array_ptr, size_t length) {
  size_t* size_t_view = (size_t*)array_ptr;
  size_t_view[LENGTH_OFFSET] = length;
}

extern void** array_size_t_push(void** array_ptr, size_t item);

inline
void array_size_t_push_unsafe(void** array_ptr, size_t item) {
  size_t capacity = array_size_t_capacity_get(array_ptr);
  size_t length = array_size_t_length_get(array_ptr);
  size_t* size_t_view = (size_t*)array_ptr;
  array_size_t_length_set_unsafe(array_ptr, length+1);
  size_t_view[length] = item;
}

extern void array_size_t_push_cap(void** array_ptr, size_t item);

inline
size_t array_size_t_pop_unsafe(void** array_ptr) {
  size_t* size_t_view = (size_t*)array_ptr;
  size_t length = --size_t_view[LENGTH_OFFSET];
  return size_t_view[length];
}
// will return 0 if no elements (prevent corrupt of length to 2**32-1)
inline
size_t array_size_t_pop(void** array_ptr) {
  size_t* size_t_view = (size_t*)array_ptr;
  size_t length = size_t_view[LENGTH_OFFSET];
  if (length == 0) return 0;
  size_t_view[LENGTH_OFFSET]--;
  return size_t_view[length];
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// EXPERIMENTAL dev life quality
////////////////////////////////////////////////////////////////////////////////////////////////////

// void* нужен т.к. иногда ругается LLVM если в scope есть array_size_t_length_get 2 раза с разными типами на вход
// safe for 64
#define LEN(a) (array_size_t_length_get((void**)a))
// NOT safe for 64
// Не будет работать т.к. надо еще один cast
// #define PUSH(a, val) (a = array_size_t_push((void**)a, val));

#define FOR_COL(col, val, type) \
  for(u32 PPCAT_NX(__, val)=0, PPCAT_NX(__len_,val) = LEN(col);PPCAT_NX(__, val)<PPCAT_NX(__len_,val);PPCAT_NX(__, val)++) { \
    type val = col[PPCAT_NX(__, val)];

#define FOR_COL_END }
