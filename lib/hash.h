#include "type.h"
extern const u32 RTTI_HASH;

// implements generic map for key=u32 and value=u32
// doesn't increase bucket count when grows.
// LATER TODO increase bucket_size

extern const u32 HASH32_BUCKET_COUNT        ;
extern const u32 HASH32_BUCKET_DEFAULT_SIZE ;
extern const u32 HASH32_EMPTY_VALUE         ;

extern void*** hash_size_t_alloc();
extern void hash_size_t_free(void*** hash_ptr);
extern void hash_size_t_clear(void*** hash_ptr);
extern void hash_size_t_alloc_or_clear(void**** hash_ptr);

inline
size_t hash_size_t_key_hash(size_t key, size_t bucket_count) {
  // TODO (key*a) % HASH32_BUCKET_COUNT 
  return key % bucket_count;
}

inline
bool hash_size_t_has(void*** hash_ptr, size_t key) {
  // TODO extract HASH32_BUCKET_COUNT from hash_ptr
  size_t hash_key = hash_size_t_key_hash(key, HASH32_BUCKET_COUNT);
  size_t*** size_t_view3 = (size_t***)hash_ptr;
  size_t** bucket = size_t_view3[hash_key];
  size_t bucket_length = array_size_t_length_get((void**)bucket);
  size_t** bucket_end = bucket+bucket_length;
  for(;bucket < bucket_end;bucket+=2) {
    size_t* bucket1 = (size_t*)bucket;
    if (*bucket1 != hash_key) continue;
    return true;
  }
  return false;
}

extern size_t hash_size_t_get(void*** hash_ptr, size_t key);

// NOTE it returns new_hash_ptr if realloc (e.g. bucket_size increase)
// TODO realloc LATER
extern void*** hash_size_t_set(void*** hash_ptr, size_t key, size_t value);
