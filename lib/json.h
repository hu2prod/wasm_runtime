#pragma once
#include "type.h"
#include "str.h"

inline
char* u32_to_json(u32 t) {
  return u32_to_str_dec(t);
}

inline
char* i32_to_json(i32 t) {
  return i32_to_str_dec(t);
}

extern char* str_to_json(char* t);
