#include "runtime.h"
#include "array_size_t.h"
const u32 RTTI_ARRAY_SIZE_T = 4;

// implements generic array for u32 and LATER u8
// should be enough for most cases

__attribute__((visibility("default")))
void** array_size_t_alloc(size_t size) {
  void** array_ptr = __alloc_array(sizeof(size_t)*size, "array_size_t_alloc");
  size_t* size_t_view = (size_t*)array_ptr;
  size_t_view[RTTI_OFFSET] = RTTI_ARRAY_SIZE_T; // rtti
  size_t_view[REF_COUNT_OFFSET]  = 1;    // ref_count
  size_t_view[CAPACITY_OFFSET]   = size; // capacity
  size_t_view[LENGTH_OFFSET]     = 0;    // length
  
  return array_ptr;
}
__attribute__((visibility("default")))
void array_size_t_free(void** array_ptr) {
  size_t* size_t_view = (size_t*)array_ptr;
  if (size_t_view[RTTI_OFFSET] != RTTI_ARRAY_SIZE_T) {
    putsi("pointer  = ", (size_t)array_ptr);
    putsi("RTTI     = ", size_t_view[RTTI_OFFSET]);
    user_throw("array_size_t_free bad RTTI");
    return;
  }
  if (size_t_view[REF_COUNT_OFFSET] > 1) {
    size_t_view[REF_COUNT_OFFSET]--;
    return;
  }
  __free_array(array_ptr);
}

// inline
// void array_size_t_clear(void** array_ptr) {
//   array_size_t_length_set_unsafe(array_ptr, 0);
// }
void array_size_t_alloc_or_clear(void*** array_ptr, size_t size) {
  if (*array_ptr == 0) {
    *array_ptr = array_size_t_alloc(size);
    return;
  }
  array_size_t_length_set_unsafe(*array_ptr, 0); // don't move data that we don't need
  *array_ptr = array_size_t_length_set_realloc(*array_ptr, size); // the only function that makes proper grow to next size
  // set 1 more time bacause length set setted wrong size
  array_size_t_length_set_unsafe(*array_ptr, 0);
}


// inline
// size_t array_size_t_capacity_get(void** array_ptr) {
//   size_t* size_t_view = (size_t*)array_ptr;
//   return size_t_view[CAPACITY_OFFSET];
// }
// 
// inline
// size_t array_size_t_length_get(void** array_ptr) {
//   size_t* size_t_view = (size_t*)array_ptr;
//   return size_t_view[LENGTH_OFFSET];
// }

size_t export_array_size_t_capacity_get(void** array_ptr) {
  size_t* size_t_view = (size_t*)array_ptr;
  return size_t_view[CAPACITY_OFFSET];
}
size_t export_array_size_t_length_get(void** array_ptr) {
  size_t* size_t_view = (size_t*)array_ptr;
  return size_t_view[LENGTH_OFFSET];
}

// DO not set to zero all data if length > old_length
// DO not free data            if length < old_length
// inline
// void array_size_t_length_set_unsafe(void** array_ptr, size_t length) {
//   size_t* size_t_view = (size_t*)array_ptr;
//   size_t_view[LENGTH_OFFSET] = length;
// }

void** array_size_t_length_set_realloc(void** array_ptr, size_t length) {
  size_t* size_t_view = (size_t*)array_ptr;
  size_t capacity   = size_t_view[CAPACITY_OFFSET];
  if (length > capacity) {
    size_t new_capacity = capacity * 2;
    while (new_capacity < length) {
      new_capacity *= 2;
    }
    
    array_ptr = array_size_t_realloc(array_ptr, new_capacity);
    size_t_view = (size_t*)array_ptr;
  }
  size_t_view[LENGTH_OFFSET] = length;
  return array_ptr;
}

void** array_size_t_realloc(void** array_ptr, size_t size) {
  // COPYPASTE array_size_t_length_get
  size_t* size_t_view = (size_t*)array_ptr;
  size_t old_length = size_t_view[LENGTH_OFFSET];
  if (size < old_length) {
    putsi("new size", size);
    putsi("old size", old_length);
    user_throw("bad realloc");
  }
  
  void** res = array_size_t_alloc(size);
  size_t* res_size_t_view = (size_t*)res;
  // size_t* size_t_view = (size_t*)array_ptr;
  array_size_t_length_set_unsafe(res, old_length);
  
  // TODO? memcpy
  for(size_t i=0;i<old_length;i++) {
    res_size_t_view[i] = size_t_view[i];
  }
  
  __free_array(array_ptr);
  return res;
}

void** array_size_t_push(void** array_ptr, size_t item) {
  size_t capacity= array_size_t_capacity_get(array_ptr);
  size_t length  = array_size_t_length_get(array_ptr);
  if (length >= capacity) {
    array_ptr = array_size_t_realloc(array_ptr, capacity*2);
  }
  size_t* size_t_view = (size_t*)array_ptr;
  array_size_t_length_set_unsafe(array_ptr, length+1);
  size_t_view[length] = item;
  
  return array_ptr;
}

// inline
// void array_size_t_push_unsafe(void** array_ptr, size_t item) {
//   size_t capacity = array_size_t_capacity_get(array_ptr);
//   size_t length = array_size_t_length_get(array_ptr);
//   size_t* size_t_view = (size_t*)array_ptr;
//   array_size_t_length_set_unsafe(array_ptr, length+1);
//   size_t_view[length] = item;
// }

void array_size_t_push_cap(void** array_ptr, size_t item) {
  size_t capacity = array_size_t_capacity_get(array_ptr);
  size_t length = array_size_t_length_get(array_ptr);
  if (length >= capacity) {
    user_throw("array_size_t_push_cap assert fail");
    return;
  }
  size_t* size_t_view = (size_t*)array_ptr;
  array_size_t_length_set_unsafe(array_ptr, length+1);
  size_t_view[length] = item;
}

// inline
// size_t array_size_t_pop_unsafe(void** array_ptr) {
//   size_t* size_t_view = (size_t*)array_ptr;
//   // size_t* size_t_view = (size_t*)array_ptr;
//   size_t length = --size_t_view[LENGTH_OFFSET];
//   return size_t_view[length];
// }
// // will return 0 if no elements (prevent corrupt of length to 2**32-1)
// inline
// size_t array_size_t_pop(void** array_ptr) {
//   size_t* size_t_view = (size_t*)array_ptr;
//   size_t length = size_t_view[LENGTH_OFFSET];
//   if (length == 0) return 0;
//   size_t_view[LENGTH_OFFSET]--;
//   // size_t* size_t_view = (size_t*)array_ptr;
//   return size_t_view[length];
// }
