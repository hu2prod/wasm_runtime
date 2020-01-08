#pragma once
#include "type.h"
extern const u32 RTTI_ARRAY32;

extern void** array32_alloc(size_t size);
extern void array32_free(void** array_ptr);
extern void** array32_realloc(void** array_ptr, size_t size);
inline void array32_length_set_unsafe(void** array_ptr, size_t length);
extern void** array32_length_set_realloc(void** array_ptr, size_t length);

inline
void array32_clear(void** array_ptr) {
  array32_length_set_unsafe(array_ptr, 0);
}
void array32_alloc_or_clear(void*** array_ptr, size_t size);

inline
size_t array32_capacity_get(void** array_ptr) {
  size_t* size_t_view = (size_t*)array_ptr;
  return size_t_view[CAPACITY_OFFSET];
}

inline
size_t array32_length_get(void** array_ptr) {
  size_t* size_t_view = (size_t*)array_ptr;
  return size_t_view[LENGTH_OFFSET];
}

extern size_t export_array32_capacity_get(void** array_ptr);
extern size_t export_array32_length_get(void** array_ptr);

// DO not set to zero all data if length > old_length
// DO not free data            if length < old_length
inline
void array32_length_set_unsafe(void** array_ptr, size_t length) {
  size_t* size_t_view = (size_t*)array_ptr;
  size_t_view[LENGTH_OFFSET] = length;
}

extern void** array32_push(void** array_ptr, u32 item);

inline
void array32_push_unsafe(void** array_ptr, u32 item) {
  size_t capacity = array32_capacity_get(array_ptr);
  size_t length = array32_length_get(array_ptr);
  u32* u32_view = (u32*)array_ptr;
  array32_length_set_unsafe(array_ptr, length+1);
  u32_view[length] = item;
}

extern void array32_push_cap(void** array_ptr, u32 item);

inline
u32 array32_pop_unsafe(void** array_ptr) {
  size_t* size_t_view = (size_t*)array_ptr;
  u32* u32_view = (u32*)array_ptr;
  size_t length = --size_t_view[LENGTH_OFFSET];
  return u32_view[length];
}
// will return 0 if no elements (prevent corrupt of length to 2**32-1)
inline
u32 array32_pop(void** array_ptr) {
  size_t* size_t_view = (size_t*)array_ptr;
  size_t length = size_t_view[LENGTH_OFFSET];
  if (length == 0) return 0;
  size_t_view[LENGTH_OFFSET]--;
  u32* u32_view = (u32*)array_ptr;
  return u32_view[length];
}
