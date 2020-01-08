#include "runtime.h"
#include "hash.h"
const u32 RTTI_HASH = 9;

// implements generic map for key=u32 and value=u32
// doesn't increase bucket count when grows.
// LATER TODO increase bucket_size

const u32 HASH32_BUCKET_COUNT         = 1024;
const u32 HASH32_BUCKET_DEFAULT_SIZE  = 64; // 32 pair
const u32 HASH32_EMPTY_VALUE          = 0;
// 64 Kb default/minimum size for upto 32k keys (optimistic)

void*** hash_size_t_alloc() {
  void*** hash_ptr = (void***)array_size_t_alloc(HASH32_BUCKET_COUNT);
  size_t* size_t_view = (size_t*)hash_ptr;
  size_t*** size_t_view3 = (size_t***)hash_ptr;
  size_t_view[RTTI_OFFSET]       = RTTI_HASH;
  size_t_view[REF_COUNT_OFFSET]  = 1;
  size_t_view[CAPACITY_OFFSET]   = HASH32_BUCKET_COUNT;
  size_t_view[LENGTH_OFFSET]     = HASH32_BUCKET_COUNT;
  
  for(size_t bucket_id=0;bucket_id<HASH32_BUCKET_COUNT;bucket_id++) {
    size_t_view3[bucket_id] = (size_t**)array_size_t_alloc(HASH32_BUCKET_DEFAULT_SIZE);
  }
  
  return hash_ptr;
}

void hash_size_t_free(void*** hash_ptr) {
  size_t* size_t_view = (size_t*)hash_ptr;
  if (size_t_view[RTTI_OFFSET] != RTTI_HASH) {
    putsi("pointer  = ", (size_t)hash_ptr);
    putsi("RTTI     = ", size_t_view[RTTI_OFFSET]);
    user_throw("hash_size_t_free bad RTTI");
    return;
  }
  
  for(size_t bucket_id=0;bucket_id<HASH32_BUCKET_COUNT;bucket_id++) {
    array_size_t_free((void**)size_t_view[bucket_id]);
  }
  
  ws_free(hash_ptr);
}

void hash_size_t_clear(void*** hash_ptr) {
  size_t*** size_t_view3 = (size_t***)hash_ptr;
  for(size_t bucket_id=0;bucket_id<HASH32_BUCKET_COUNT;bucket_id++) {
    size_t** bucket = size_t_view3[bucket_id];
    array_size_t_length_set_unsafe((void**)bucket, 0);
  }
}

void hash_size_t_alloc_or_clear(void**** hash_ptr) {
  if (*hash_ptr == 0) {
    *hash_ptr = hash_size_t_alloc();
    return;
  }
  hash_size_t_clear(*hash_ptr);
}

// inline
// size_t hash_size_t_key_hash(size_t key, size_t bucket_count) {
//   // TODO (key*a) % HASH32_BUCKET_COUNT 
//   return key % bucket_count;
// }
// 
// inline
// bool hash_size_t_has(void*** hash_ptr, size_t key) {
//   // TODO extract HASH32_BUCKET_COUNT from hash_ptr
//   size_t hash_key = hash_size_t_key_hash(key, HASH32_BUCKET_COUNT);
//   size_t*** size_t_view3 = (size_t***)hash_ptr;
//   size_t** bucket = size_t_view3[hash_key];
//   size_t bucket_length = array_size_t_length_get((void**)bucket);
//   size_t** bucket_end = bucket+bucket_length;
//   for(;bucket < bucket_end;bucket+=2) {
//     size_t* bucket1 = (size_t*)bucket;
//     if (*bucket1 != hash_key) continue;
//     return true;
//   }
//   return false;
// }

size_t hash_size_t_get(void*** hash_ptr, size_t key) {
  // TODO extract HASH32_BUCKET_COUNT from hash_ptr
  size_t hash_key = hash_size_t_key_hash(key, HASH32_BUCKET_COUNT);
  size_t*** size_t_view3 = (size_t***)hash_ptr;
  size_t** bucket = size_t_view3[hash_key];
  size_t bucket_length = array_size_t_length_get((void**)bucket);
  size_t** bucket_end = bucket+bucket_length;
  for(;bucket < bucket_end;bucket+=2) {
    size_t* bucket1 = (size_t*)bucket;
    if (*bucket1 != hash_key) continue;
    return *(bucket1+1);
  }
  return HASH32_EMPTY_VALUE;
}

// NOTE it returns new_hash_ptr if realloc (e.g. bucket_size increase)
// TODO realloc LATER
void*** hash_size_t_set(void*** hash_ptr, size_t key, size_t value) {
  // TODO extract HASH32_BUCKET_COUNT from hash_ptr
  size_t hash_key = hash_size_t_key_hash(key, HASH32_BUCKET_COUNT);
  size_t*** size_t_view3 = (size_t***)hash_ptr;
  size_t** bucket = size_t_view3[hash_key];
  size_t bucket_size = array_size_t_length_get((void**)bucket);
  // check key
  size_t** bucket_end = bucket + bucket_size;
  for(;bucket < bucket_end; bucket+=2) {
    size_t* bucket1 = (size_t*)bucket;
    if (*bucket1 != hash_key) continue;
    *(bucket1+1) = value;
    return hash_ptr;
  }
  // add key
  size_t_view3[hash_key] = bucket = (size_t**)array_size_t_length_set_realloc((void**)bucket, bucket_size+2);
  
  size_t* bucket1 = (size_t*)bucket;
  bucket1[bucket_size  ] = hash_key;
  bucket1[bucket_size+1] = value;
  
  return hash_ptr;
}