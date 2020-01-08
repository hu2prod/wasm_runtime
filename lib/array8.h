#pragma once
#include "type.h"
extern const u32 RTTI_ARRAY8;

extern void** array8_alloc(size_t size);
extern void array8_free(void** array_ptr);
extern void** array8_realloc(void** array_ptr, size_t size);
inline void array8_length_set_unsafe(void** array_ptr, size_t length);
extern void** array8_length_set_realloc(void** array_ptr, size_t length);

inline
void array8_clear(void** array_ptr) {
  array8_length_set_unsafe(array_ptr, 0);
}
void array8_alloc_or_clear(void*** array_ptr, size_t size);

inline
size_t array8_capacity_get(void** array_ptr) {
  size_t* size_t_view = (size_t*)array_ptr;
  return size_t_view[CAPACITY_OFFSET];
}

inline
size_t array8_length_get(void** array_ptr) {
  size_t* size_t_view = (size_t*)array_ptr;
  return size_t_view[LENGTH_OFFSET];
}

extern size_t export_array8_capacity_get(void** array_ptr);
extern size_t export_array8_length_get(void** array_ptr);

// DO not set to zero all data if length > old_length
// DO not free data            if length < old_length
inline
void array8_length_set_unsafe(void** array_ptr, size_t length) {
  size_t* size_t_view = (size_t*)array_ptr;
  size_t_view[LENGTH_OFFSET] = length;
}

extern void** array8_push(void** array_ptr, u8 item);

inline
void array8_push_unsafe(void** array_ptr, u8 item) {
  size_t capacity = array8_capacity_get(array_ptr);
  size_t length = array8_length_get(array_ptr);
  u8* u8_view = (u8*)array_ptr;
  array8_length_set_unsafe(array_ptr, length+1);
  u8_view[length] = item;
}

extern void array8_push_cap(void** array_ptr, u8 item);

inline
u8 array8_pop_unsafe(void** array_ptr) {
  size_t* size_t_view = (size_t*)array_ptr;
  u8* u8_view = (u8*)array_ptr;
  size_t length = --size_t_view[LENGTH_OFFSET];
  return u8_view[length];
}
// will return 0 if no elements (prevent corrupt of length to 2**32-1)
inline
u8 array8_pop(void** array_ptr) {
  size_t* size_t_view = (size_t*)array_ptr;
  size_t length = size_t_view[LENGTH_OFFSET];
  if (length == 0) return 0;
  size_t_view[LENGTH_OFFSET]--;
  u8* u8_view = (u8*)array_ptr;
  return u8_view[length];
}
