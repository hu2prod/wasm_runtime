#pragma once
#include "type.h"
extern const u32 RTTI_ARRAY64F;

extern void** array64f_alloc(size_t size);
extern void array64f_free(void** array_ptr);
extern void** array64f_realloc(void** array_ptr, size_t size);
inline void array64f_length_set_unsafe(void** array_ptr, size_t length);
extern void** array64f_length_set_realloc(void** array_ptr, size_t length);

inline
void array64f_clear(void** array_ptr) {
  array64f_length_set_unsafe(array_ptr, 0);
}
void array64f_alloc_or_clear(void*** array_ptr, size_t size);

inline
size_t array64f_capacity_get(void** array_ptr) {
  size_t* size_t_view = (size_t*)array_ptr;
  return size_t_view[CAPACITY_OFFSET];
}

inline
size_t array64f_length_get(void** array_ptr) {
  size_t* size_t_view = (size_t*)array_ptr;
  return size_t_view[LENGTH_OFFSET];
}

extern size_t export_array64f_capacity_get(void** array_ptr);
extern size_t export_array64f_length_get(void** array_ptr);

// DO not set to zero all data if length > old_length
// DO not free data            if length < old_length
inline
void array64f_length_set_unsafe(void** array_ptr, size_t length) {
  size_t* size_t_view = (size_t*)array_ptr;
  size_t_view[LENGTH_OFFSET] = length;
}

extern void** array64f_push(void** array_ptr, f64 item);

inline
void array64f_push_unsafe(void** array_ptr, f64 item) {
  size_t capacity = array64f_capacity_get(array_ptr);
  size_t length = array64f_length_get(array_ptr);
  f64* f64_view = (f64*)array_ptr;
  array64f_length_set_unsafe(array_ptr, length+1);
  f64_view[length] = item;
}

extern void array64f_push_cap(void** array_ptr, f64 item);

inline
f64 array64f_pop_unsafe(void** array_ptr) {
  size_t* size_t_view = (size_t*)array_ptr;
  f64* f64_view = (f64*)array_ptr;
  size_t length = --size_t_view[LENGTH_OFFSET];
  return f64_view[length];
}
// will return 0 if no elements (prevent corrupt of length to 2**32-1)
inline
f64 array64f_pop(void** array_ptr) {
  size_t* size_t_view = (size_t*)array_ptr;
  size_t length = size_t_view[LENGTH_OFFSET];
  if (length == 0) return 0;
  size_t_view[LENGTH_OFFSET]--;
  f64* f64_view = (f64*)array_ptr;
  return f64_view[length];
}
