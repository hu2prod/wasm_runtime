#pragma once
#include "type.h"
extern const u32 RTTI_ARRAY64;

extern void** array64_alloc(size_t size);
extern void array64_free(void** array_ptr);
extern void** array64_realloc(void** array_ptr, size_t size);
inline void array64_length_set_unsafe(void** array_ptr, size_t length);
extern void** array64_length_set_realloc(void** array_ptr, size_t length);

inline
void array64_clear(void** array_ptr) {
  array64_length_set_unsafe(array_ptr, 0);
}
void array64_alloc_or_clear(void*** array_ptr, size_t size);

inline
size_t array64_capacity_get(void** array_ptr) {
  size_t* size_t_view = (size_t*)array_ptr;
  return size_t_view[CAPACITY_OFFSET];
}

inline
size_t array64_length_get(void** array_ptr) {
  size_t* size_t_view = (size_t*)array_ptr;
  return size_t_view[LENGTH_OFFSET];
}

extern size_t export_array64_capacity_get(void** array_ptr);
extern size_t export_array64_length_get(void** array_ptr);

// DO not set to zero all data if length > old_length
// DO not free data            if length < old_length
inline
void array64_length_set_unsafe(void** array_ptr, size_t length) {
  size_t* size_t_view = (size_t*)array_ptr;
  size_t_view[LENGTH_OFFSET] = length;
}

extern void** array64_push(void** array_ptr, u64 item);

inline
void array64_push_unsafe(void** array_ptr, u64 item) {
  size_t capacity = array64_capacity_get(array_ptr);
  size_t length = array64_length_get(array_ptr);
  u64* u64_view = (u64*)array_ptr;
  array64_length_set_unsafe(array_ptr, length+1);
  u64_view[length] = item;
}

extern void array64_push_cap(void** array_ptr, u64 item);

inline
u64 array64_pop_unsafe(void** array_ptr) {
  size_t* size_t_view = (size_t*)array_ptr;
  u64* u64_view = (u64*)array_ptr;
  size_t length = --size_t_view[LENGTH_OFFSET];
  return u64_view[length];
}
// will return 0 if no elements (prevent corrupt of length to 2**32-1)
inline
u64 array64_pop(void** array_ptr) {
  size_t* size_t_view = (size_t*)array_ptr;
  size_t length = size_t_view[LENGTH_OFFSET];
  if (length == 0) return 0;
  size_t_view[LENGTH_OFFSET]--;
  u64* u64_view = (u64*)array_ptr;
  return u64_view[length];
}
