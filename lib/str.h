#pragma once
#include "type.h"
extern const u32 RTTI_STR;

extern char* str_alloc(size_t len);
// на самом деле, если будет плохое стечение обстоятельств и в памяти будет именно этот идентификатор... то оно сделает пиздец layout'у памяти
// оно вернет в пул указатель на зону которую мы не выделяли и будет дамажить всю память
extern void str_free(char* str);

inline
bool is_heap_str(char *str) {
  size_t* size_t_view = (size_t*)str;
  return size_t_view[RTTI_OFFSET] == RTTI_STR;
}
extern size_t str_len(char *str);

// NOTE it frees heap str
extern char* str_concat_z(char** str_list);

extern size_t __last_u32_to_str_len;
extern char* _u32_to_str_dec(u32 t, u32 min_len);
extern char* _u32_to_str_hex(u32 t, u32 min_len);

inline
char* u32_to_str_dec(u32 t) {
  return _u32_to_str_dec(t, 0);
}

inline
char* u32_to_str_dec_zero(u32 t, size_t min_len) {
  char* res = _u32_to_str_dec(t, min_len);
  // size_t real_len = str_len(res);
  size_t real_len = __last_u32_to_str_len;
  if (real_len >= min_len) return res;
  size_t diff_len = min_len - real_len;
  char* dst = res;
  for(;diff_len--;) {
    *dst++ = '0';
  }
  return res;
}

inline
char* u32_to_str_hex(u32 t) {
  return _u32_to_str_hex(t, 0);
}

inline
char* u32_to_str_hex_zero(u32 t, size_t min_len) {
  char* res = _u32_to_str_hex(t, min_len);
  // size_t real_len = str_len(res);
  size_t real_len = __last_u32_to_str_len;
  if (real_len >= min_len) return res;
  size_t diff_len = min_len - real_len;
  char* dst = res;
  for(;diff_len--;) {
    *dst++ = '0';
  }
  return res;
}

inline
char* i32_to_str_dec(i32 t) {
  if (t >= 0) {
    return _u32_to_str_dec(t, 0);
  }
  char* res = _u32_to_str_dec(t, 1);
  res[0] = '-';
  return res;
}
