#include "runtime.h"
#include "json.h"

char* str_to_json(char* t) {
  size_t out_len = 2; // ""
  size_t in_len = str_len(t);
  char* lookup = t;
  for(size_t in_len_tmp = in_len;in_len_tmp--;lookup++) {
    switch(*lookup) {
      case '\\':
      case '"':
      case '\b':
      case '\f':
      case '\n':
      case '\r':
      case '\t':
        out_len+=2;
        break;
      default:
        if ('\x00' <= *lookup && *lookup <= '\x1f') {
          // unicode '\x00??'
          out_len += 6;
        } else {
          out_len++;
        }
    }
  }
  char* res = str_alloc(out_len);
  char* dst = res;
  
  lookup = t;
  for(size_t in_len_tmp = in_len;in_len_tmp--;lookup++) {
    switch(*lookup) {
      case '\\':
      case '"':
      case '\b':
      case '\f':
      case '\n':
      case '\r':
      case '\t':
        *dst++ = '\\';
        *dst++ = *lookup;
        break;
      default:
        if ('\x00' <= *lookup && *lookup <= '\x1f') {
          // unicode '\x00??'
          *dst++ = '\\';
          *dst++ = 'x';
          *dst++ = '0';
          *dst++ = '0';
          char* tmp = u32_to_str_dec_zero((u32)*lookup, 2);
          *dst++ = tmp[0];
          *dst++ = tmp[1];
          str_free(tmp);
        } else {
          *dst++ = *lookup;
        }
    }
  }
  
  return res;
}
