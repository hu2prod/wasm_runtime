#include "runtime.h"
#include "array64f.h"
const u32 RTTI_ARRAY64F = 3;

__attribute__((visibility("default")))
void** array64f_alloc(size_t size) {
  void** array_ptr = __alloc_array(sizeof(f64)*size, "array64f_alloc");
  size_t* size_t_view = (size_t*)array_ptr;
  size_t_view[RTTI_OFFSET] = RTTI_ARRAY64F; // rtti
  size_t_view[REF_COUNT_OFFSET]  = 1;    // ref_count
  size_t_view[CAPACITY_OFFSET]   = size; // capacity
  size_t_view[LENGTH_OFFSET]     = 0;    // length
  
  return array_ptr;
}
__attribute__((visibility("default")))
void array64f_free(void** array_ptr) {
  size_t* size_t_view = (size_t*)array_ptr;
  if (size_t_view[RTTI_OFFSET] != RTTI_ARRAY64F) {
    putsi("pointer  = ", (size_t)array_ptr);
    putsi("RTTI     = ", size_t_view[RTTI_OFFSET]);
    user_throw("array64f_free bad RTTI");
    return;
  }
  if (size_t_view[REF_COUNT_OFFSET] > 1) {
    size_t_view[REF_COUNT_OFFSET]--;
    return;
  }
  __free_array(array_ptr);
}

// inline
// void array64f_clear(void** array_ptr) {
//   array64f_length_set_unsafe(array_ptr, 0);
// }
void array64f_alloc_or_clear(void*** array_ptr, size_t size) {
  if (*array_ptr == 0) {
    *array_ptr = array64f_alloc(size);
    return;
  }
  array64f_length_set_unsafe(*array_ptr, 0); // don't move data that we don't need
  *array_ptr = array64f_length_set_realloc(*array_ptr, size); // the only function that makes proper grow to next size
  // set 1 more time bacause length set setted wrong size
  array64f_length_set_unsafe(*array_ptr, 0);
}


// inline
// size_t array64f_capacity_get(void** array_ptr) {
//   size_t* size_t_view = (size_t*)array_ptr;
//   return size_t_view[CAPACITY_OFFSET];
// }
// 
// inline
// size_t array64f_length_get(void** array_ptr) {
//   size_t* size_t_view = (size_t*)array_ptr;
//   return size_t_view[LENGTH_OFFSET];
// }

size_t export_array64f_capacity_get(void** array_ptr) {
  size_t* size_t_view = (size_t*)array_ptr;
  return size_t_view[CAPACITY_OFFSET];
}
size_t export_array64f_length_get(void** array_ptr) {
  size_t* size_t_view = (size_t*)array_ptr;
  return size_t_view[LENGTH_OFFSET];
}

// DO not set to zero all data if length > old_length
// DO not free data            if length < old_length
// inline
// void array64f_length_set_unsafe(void** array_ptr, size_t length) {
//   size_t* size_t_view = (size_t*)array_ptr;
//   size_t_view[LENGTH_OFFSET] = length;
// }

void** array64f_length_set_realloc(void** array_ptr, size_t length) {
  size_t* size_t_view = (size_t*)array_ptr;
  size_t capacity   = size_t_view[CAPACITY_OFFSET];
  if (length > capacity) {
    size_t new_capacity = capacity * 2;
    while (new_capacity < length) {
      new_capacity *= 2;
    }
    
    array_ptr = array64f_realloc(array_ptr, new_capacity);
    size_t_view = (size_t*)array_ptr;
  }
  size_t_view[LENGTH_OFFSET] = length;
  return array_ptr;
}

void** array64f_realloc(void** array_ptr, size_t size) {
  // COPYPASTE array64f_length_get
  size_t* size_t_view = (size_t*)array_ptr;
  size_t old_length = size_t_view[LENGTH_OFFSET];
  if (size < old_length) {
    putsi("new size", size);
    putsi("old size", old_length);
    user_throw("bad realloc");
  }
  
  void** res = array64f_alloc(size);
  f64* res_f64_view = (f64*)res;
  f64* f64_view = (f64*)array_ptr;
  array64f_length_set_unsafe(res, old_length);
  
  // TODO? memcpy
  for(size_t i=0;i<old_length;i++) {
    res_f64_view[i] = f64_view[i];
  }
  
  __free_array(array_ptr);
  return res;
}

void** array64f_push(void** array_ptr, f64 item) {
  size_t capacity= array64f_capacity_get(array_ptr);
  size_t length  = array64f_length_get(array_ptr);
  if (length >= capacity) {
    array_ptr = array64f_realloc(array_ptr, capacity*2);
  }
  f64* f64_view = (f64*)array_ptr;
  array64f_length_set_unsafe(array_ptr, length+1);
  f64_view[length] = item;
  
  return array_ptr;
}

// inline
// void array64f_push_unsafe(void** array_ptr, f64 item) {
//   size_t capacity = array64f_capacity_get(array_ptr);
//   size_t length = array64f_length_get(array_ptr);
//   f64* f64_view = (f64*)array_ptr;
//   array64f_length_set_unsafe(array_ptr, length+1);
//   f64_view[length] = item;
// }

void array64f_push_cap(void** array_ptr, f64 item) {
  size_t capacity = array64f_capacity_get(array_ptr);
  size_t length = array64f_length_get(array_ptr);
  if (length >= capacity) {
    user_throw("array64f_push_cap assert fail");
    return;
  }
  f64* f64_view = (f64*)array_ptr;
  array64f_length_set_unsafe(array_ptr, length+1);
  f64_view[length] = item;
}

// inline
// f64 array64f_pop_unsafe(void** array_ptr) {
//   size_t* size_t_view = (size_t*)array_ptr;
//   f64* f64_view = (f64*)array_ptr;
//   size_t length = --size_t_view[LENGTH_OFFSET];
//   return f64_view[length];
// }
// // will return 0 if no elements (prevent corrupt of length to 2**32-1)
// inline
// f64 array64f_pop(void** array_ptr) {
//   size_t* size_t_view = (size_t*)array_ptr;
//   size_t length = size_t_view[LENGTH_OFFSET];
//   if (length == 0) return 0;
//   size_t_view[LENGTH_OFFSET]--;
//   f64* f64_view = (f64*)array_ptr;
//   return f64_view[length];
// }
