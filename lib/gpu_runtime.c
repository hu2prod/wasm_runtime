#pragma once
#include "runtime.h"
#include "gpu_runtime.h"

__attribute__((visibility("default"))) char* __gpu_local_prefix;
__attribute__((visibility("default"))) char* __gpu_group_prefix; 
u32* __gpu_id_list;
__attribute__((visibility("default"))) 
void __gpu_id_init(u32 count) {
  // logsi("__gpu_id_init", count);
  __gpu_id_list = (u32*)array32_alloc((size_t)count);
  // logsi("__gpu_id_init done", 1);
}

__attribute__((visibility("default"))) 
void __gpu_id_set(u32 idx, u32 value) {
  // logsi("__gpu_id_set idx", idx);
  // logsi("__gpu_id_set value", value);
  array32_length_set_unsafe((void*)__gpu_id_list, idx+1);
  __gpu_id_list[idx] = value;
}
