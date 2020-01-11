#include "runtime.h"
#include "array64.h"
const u32 RTTI_ARRAY64 = 2;

__attribute__((visibility("default")))
void** array64_alloc(size_t size) {
  void** array_ptr = __alloc_array(sizeof(u64)*size, "array64_alloc");
  size_t* size_t_view = (size_t*)array_ptr;
  size_t_view[RTTI_OFFSET] = RTTI_ARRAY64; // rtti
  size_t_view[REF_COUNT_OFFSET]  = 1;    // ref_count
  size_t_view[CAPACITY_OFFSET]   = size; // capacity
  size_t_view[LENGTH_OFFSET]     = 0;    // length
  
  return array_ptr;
}
__attribute__((visibility("default")))
void array64_free(void** array_ptr) {
  size_t* size_t_view = (size_t*)array_ptr;
  if (size_t_view[RTTI_OFFSET] != RTTI_ARRAY64) {
    putsi("pointer  = ", (size_t)array_ptr);
    putsi("RTTI     = ", size_t_view[RTTI_OFFSET]);
    user_throw("array64_free bad RTTI");
    return;
  }
  if (size_t_view[REF_COUNT_OFFSET] > 1) {
    size_t_view[REF_COUNT_OFFSET]--;
    return;
  }
  __free_array(array_ptr);
}

// inline
// void array64_clear(void** array_ptr) {
//   array64_length_set_unsafe(array_ptr, 0);
// }
void array64_alloc_or_clear(void*** array_ptr, size_t size) {
  if (*array_ptr == 0) {
    *array_ptr = array64_alloc(size);
    return;
  }
  array64_length_set_unsafe(*array_ptr, 0); // don't move data that we don't need
  *array_ptr = array64_length_set_realloc(*array_ptr, size); // the only function that makes proper grow to next size
  // set 1 more time bacause length set setted wrong size
  array64_length_set_unsafe(*array_ptr, 0);
}


// inline
// size_t array64_capacity_get(void** array_ptr) {
//   size_t* size_t_view = (size_t*)array_ptr;
//   return size_t_view[CAPACITY_OFFSET];
// }
// 
// inline
// size_t array64_length_get(void** array_ptr) {
//   size_t* size_t_view = (size_t*)array_ptr;
//   return size_t_view[LENGTH_OFFSET];
// }

size_t export_array64_capacity_get(void** array_ptr) {
  size_t* size_t_view = (size_t*)array_ptr;
  return size_t_view[CAPACITY_OFFSET];
}
size_t export_array64_length_get(void** array_ptr) {
  size_t* size_t_view = (size_t*)array_ptr;
  return size_t_view[LENGTH_OFFSET];
}

// DO not set to zero all data if length > old_length
// DO not free data            if length < old_length
// inline
// void array64_length_set_unsafe(void** array_ptr, size_t length) {
//   size_t* size_t_view = (size_t*)array_ptr;
//   size_t_view[LENGTH_OFFSET] = length;
// }

void** array64_length_set_realloc(void** array_ptr, size_t length) {
  size_t* size_t_view = (size_t*)array_ptr;
  size_t capacity   = size_t_view[CAPACITY_OFFSET];
  if (length > capacity) {
    size_t new_capacity = capacity * 2;
    while (new_capacity < length) {
      new_capacity *= 2;
    }
    
    array_ptr = array64_realloc(array_ptr, new_capacity);
    size_t_view = (size_t*)array_ptr;
  }
  size_t_view[LENGTH_OFFSET] = length;
  return array_ptr;
}

void** array64_realloc(void** array_ptr, size_t size) {
  // COPYPASTE array64_length_get
  size_t* size_t_view = (size_t*)array_ptr;
  size_t old_length = size_t_view[LENGTH_OFFSET];
  if (size < old_length) {
    putsi("new size", size);
    putsi("old size", old_length);
    user_throw("bad realloc");
  }
  
  void** res = array64_alloc(size);
  u64* res_u64_view = (u64*)res;
  u64* u64_view = (u64*)array_ptr;
  array64_length_set_unsafe(res, old_length);
  
  // TODO? memcpy
  for(size_t i=0;i<old_length;i++) {
    res_u64_view[i] = u64_view[i];
  }
  
  __free_array(array_ptr);
  return res;
}

void** array64_push(void** array_ptr, u64 item) {
  size_t capacity= array64_capacity_get(array_ptr);
  size_t length  = array64_length_get(array_ptr);
  if (length >= capacity) {
    array_ptr = array64_realloc(array_ptr, capacity*2);
  }
  u64* u64_view = (u64*)array_ptr;
  array64_length_set_unsafe(array_ptr, length+1);
  u64_view[length] = item;
  
  return array_ptr;
}

// inline
// void array64_push_unsafe(void** array_ptr, u64 item) {
//   size_t capacity = array64_capacity_get(array_ptr);
//   size_t length = array64_length_get(array_ptr);
//   u64* u64_view = (u64*)array_ptr;
//   array64_length_set_unsafe(array_ptr, length+1);
//   u64_view[length] = item;
// }

void array64_push_cap(void** array_ptr, u64 item) {
  size_t capacity = array64_capacity_get(array_ptr);
  size_t length = array64_length_get(array_ptr);
  if (length >= capacity) {
    user_throw("array64_push_cap assert fail");
    return;
  }
  u64* u64_view = (u64*)array_ptr;
  array64_length_set_unsafe(array_ptr, length+1);
  u64_view[length] = item;
}

// inline
// u64 array64_pop_unsafe(void** array_ptr) {
//   size_t* size_t_view = (size_t*)array_ptr;
//   u64* u64_view = (u64*)array_ptr;
//   size_t length = --size_t_view[LENGTH_OFFSET];
//   return u64_view[length];
// }
// // will return 0 if no elements (prevent corrupt of length to 2**32-1)
// inline
// u64 array64_pop(void** array_ptr) {
//   size_t* size_t_view = (size_t*)array_ptr;
//   size_t length = size_t_view[LENGTH_OFFSET];
//   if (length == 0) return 0;
//   size_t_view[LENGTH_OFFSET]--;
//   u64* u64_view = (u64*)array_ptr;
//   return u64_view[length];
// }
