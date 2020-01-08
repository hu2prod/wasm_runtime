#include "runtime.h"
#include "str.h"
// втихую используем этот аллокатор
#include "array8.h"
// NOTE need special ID for that
const u32 RTTI_STR = 0xdeadbeef;

char* str_alloc(size_t len) {
  size_t pad_len = len+1;
  size_t capacity = 1;
  while (capacity <= pad_len) {
    capacity *= 2;
  }
  void** ptr = array8_alloc(capacity);
  size_t* size_t_view = (size_t*)ptr;
  size_t_view[RTTI_OFFSET] = RTTI_STR;
  char* char_view = (char*)ptr;
  char_view[len] = 0;
  array8_length_set_unsafe(char_view, pad_len);
  return char_view;
}

// на самом деле, если будет плохое стечение обстоятельств и в памяти будет именно этот идентификатор... то оно сделает пиздец layout'у памяти
// оно вернет в пул указатель на зону которую мы не выделяли и будет дамажить всю память
void str_free(char* str) {
  if (str < __heap_start) {
    putsi("try free non-heap string", 1);
    putsi(str, str);
    return;
  }
  size_t* size_t_view = (size_t*)str;
  if (size_t_view[RTTI_OFFSET] != RTTI_STR) {
    putsi("try free non-array8 string on heap", 1);
    putsi(str, str);
    return;
  }
  size_t_view[RTTI_OFFSET] = RTTI_ARRAY8;
  array8_free(str);
}

size_t str_len(char *str) {
  if (is_heap_str(str)) {
    return array8_length_get(str) - 1;
  }
  // for conventional strings
  size_t ret = 0;
  char* lookup = str;
  for(;*lookup;lookup++) {}
  
  return lookup - str;
}

// NOTE it frees heap str
char* str_concat_z(char** str_list) {
  size_t total_size = 0;
  char** lookup;
  
  for(lookup = str_list;*lookup;lookup++) {
    // logsi("lookup", lookup);
    // logsi("*lookup", *lookup);
    // logsi(*lookup, 1);
    total_size += str_len(*lookup);
  }
  // logsi("total_size", total_size);
  char* res = str_alloc(total_size);
  char* dst = res;
  // logsi("here", 1);
  for(lookup = str_list;*lookup;lookup++) {
    size_t loc_size = str_len(*lookup);
    // logsi("loc_size2", loc_size);
    // logsi(*lookup, 1);
    char* loc_lookup = *lookup;
    for(;loc_size--;) {
      *dst++ = *loc_lookup++;
    }
    if (is_heap_str(*lookup)) {
      // logsi("is_heap_str free", 1);
      str_free(*lookup);
    }
  }
  
  // logsi("res", 1);
  // logsi(res, 1);
  return res;
}

size_t __last_u32_to_str_len = 0;
char* _u32_to_str_dec(u32 t, u32 min_len) {
  char rev_buf[32]; // точно хватит, сколько точно считать лень
  size_t len = 0;
  if (t == 0) {
    len = 1;
    rev_buf[0] = '0';
  } else {
    for(u32 t_lookup = t;t_lookup > 0;t_lookup /= 10) {
      u32 digit = t_lookup % 10;
      rev_buf[len++] = digit + '0';
    }
  }
  size_t real_len = max(len, min_len);
  __last_u32_to_str_len = len;
  char* res = str_alloc(real_len);
  size_t prefix = real_len - len;
  char* dst = res+prefix;
  for(;len--;) {
    *dst++ = rev_buf[len];
  }
  return res;
}
// NOTE max radix 26
char* _u32_to_str_hex(u32 t, u32 min_len) {
  const radix = 16;
  char rev_buf[32]; // точно хватит, сколько точно считать лень
  size_t len = 0;
  if (t == 0) {
    len = 1;
    rev_buf[0] = '0';
  } else {
    for(u32 t_lookup = t;t_lookup > 0;t_lookup /= radix) {
      u32 digit = t_lookup % radix;
      if (digit < 10) {
        rev_buf[len++] = digit + '0';
      } else {
        rev_buf[len++] = digit + 'a' - 10;
      }
    }
  }
  size_t real_len = max(len, min_len);
  __last_u32_to_str_len = len;
  char* res = str_alloc(real_len);
  size_t prefix = real_len - len;
  char* dst = res+prefix;
  for(;len--;) {
    *dst++ = rev_buf[len];
  }
  return res;
}
