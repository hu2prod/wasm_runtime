#pragma once
#include "type.h"
#include "array32.h"
#include "str.h"

/*
  order rule
  1. queue_id
  2. kernel_id
  3. buffer_id
*/
i32 __fuck_gpu_buffer_alloc             (u32* fuck_wasmer_gpu_id_list, u32 gpu_id, char* buffer_id, u32 size, char* flags);
i32 __fuck_gpu_buffer_free              (u32* fuck_wasmer_gpu_id_list, u32 gpu_id, char* buffer_id);
i32 __fuck_gpu_buffer_h2d_frame         (u32* fuck_wasmer_gpu_id_list, u32 gpu_id, char* queue_id, u32 history_offset, char* buffer_id);
i32 __fuck_gpu_buffer_h2d               (u32* fuck_wasmer_gpu_id_list, u32 gpu_id, char* queue_id, u32 device_ptr, char* buffer_id);
i32 __fuck_gpu_buffer_d2h               (u32* fuck_wasmer_gpu_id_list, u32 gpu_id, char* queue_id, char* buffer_id, u32 device_ptr);
i32 __fuck_wasm_gpu_kernel_decl_fs_direct(u32* fuck_wasmer_gpu_id_list, u32 gpu_id, char* file_path, char* kernel_id);
// u32 __fuck_gpu_kernel_decl              (u32* fuck_wasmer_gpu_id_list, u32 gpu_id, char* s);
i32 __fuck_gpu_kernel_arg_set_const_i32 (u32* fuck_wasmer_gpu_id_list, u32 gpu_id, char* kernel_id, u32 arg_idx, u32 arg_value);
i32 __fuck_gpu_kernel_arg_set_const_f32 (u32* fuck_wasmer_gpu_id_list, u32 gpu_id, char* kernel_id, u32 arg_idx, u32 arg_value);
i32 __fuck_gpu_kernel_arg_set_buffer    (u32* fuck_wasmer_gpu_id_list, u32 gpu_id, char* kernel_id, u32 arg_idx, char* buffer_id);
i32 __fuck_gpu_kernel_set_global_work_size(u32* fuck_wasmer_gpu_id_list, u32 gpu_id, char* kernel_id, u32 arg_value);
i32 __fuck_gpu_kernel_set_local_work_size (u32* fuck_wasmer_gpu_id_list, u32 gpu_id, char* kernel_id, u32 arg_value);
i32 __fuck_gpu_kernel_call              (u32* fuck_wasmer_gpu_id_list, u32 gpu_id, char* queue_id, char* kernel_id);
i32 __fuck_gpu_sync_queue               (u32* fuck_wasmer_gpu_id_list, u32 gpu_id, char* queue_id);

// mapping relative gpu id -> real gpu id
extern char* __gpu_local_prefix;
extern char* __gpu_group_prefix;
extern u32* __gpu_id_list;
inline u32 gpu_count_get() {
  if (__gpu_id_list == 0) {
    // logsi("__gpu_id_list uninitialized", 1);
    return 0;
  }
  return array32_length_get((void**)__gpu_id_list);
}
void __gpu_id_init(u32 count);
void __gpu_id_set(u32 idx, u32 value);

// Пофиксить это говно, когда wasmer.io таки будет позволять вызов экспортов с env func
inline i32 gpu_buffer_alloc             (u32 gpu_id, char* buffer_id, u32 size, char* flags)              {return __fuck_gpu_buffer_alloc            (__gpu_id_list, gpu_id, buffer_id, size, flags             );}
inline i32 gpu_buffer_free              (u32 gpu_id, char* buffer_id)                                     {return __fuck_gpu_buffer_free             (__gpu_id_list, gpu_id, buffer_id                          );}
inline i32 gpu_buffer_h2d_frame         (u32 gpu_id, char* queue_id, u32 history_offset, char* buffer_id) {return __fuck_gpu_buffer_h2d_frame        (__gpu_id_list, gpu_id, queue_id, history_offset, buffer_id);}
inline i32 gpu_buffer_h2d               (u32 gpu_id, char* queue_id, u32 device_ptr, char* buffer_id)     {return __fuck_gpu_buffer_h2d              (__gpu_id_list, gpu_id, queue_id, device_ptr, buffer_id    );}
inline i32 gpu_buffer_d2h               (u32 gpu_id, char* queue_id, char* buffer_id, u32 device_ptr)     {return __fuck_gpu_buffer_d2h              (__gpu_id_list, gpu_id, queue_id, buffer_id, device_ptr    );}

inline char* buffer_id2local(char *buffer_id) {
  char* list[] = {__gpu_local_prefix, buffer_id, 0};
  char *ret = str_concat_z(&list);
  return ret;
}
inline i32 gpu_buffer_local_alloc             (u32 gpu_id, char* buffer_id, u32 size, char* flags)              {buffer_id = buffer_id2local(buffer_id);i32 ret = __fuck_gpu_buffer_alloc            (__gpu_id_list, gpu_id, buffer_id, size, flags             );str_free(buffer_id);return ret;}
inline i32 gpu_buffer_local_free              (u32 gpu_id, char* buffer_id)                                     {buffer_id = buffer_id2local(buffer_id);i32 ret = __fuck_gpu_buffer_free             (__gpu_id_list, gpu_id, buffer_id                          );str_free(buffer_id);return ret;}
inline i32 gpu_buffer_local_h2d_frame         (u32 gpu_id, char* queue_id, u32 history_offset, char* buffer_id) {buffer_id = buffer_id2local(buffer_id);i32 ret = __fuck_gpu_buffer_h2d_frame        (__gpu_id_list, gpu_id, queue_id, history_offset, buffer_id);str_free(buffer_id);return ret;}
inline i32 gpu_buffer_local_h2d               (u32 gpu_id, char* queue_id, u32 device_ptr, char* buffer_id)     {buffer_id = buffer_id2local(buffer_id);i32 ret = __fuck_gpu_buffer_h2d              (__gpu_id_list, gpu_id, queue_id, device_ptr, buffer_id    );str_free(buffer_id);return ret;}
inline i32 gpu_buffer_local_d2h               (u32 gpu_id, char* queue_id, char* buffer_id, u32 device_ptr)     {buffer_id = buffer_id2local(buffer_id);i32 ret = __fuck_gpu_buffer_d2h              (__gpu_id_list, gpu_id, queue_id, buffer_id, device_ptr    );str_free(buffer_id);return ret;}

inline char* buffer_id2group(char *buffer_id) {
  char* list[] = {__gpu_group_prefix, buffer_id, 0};
  char *ret = str_concat_z(&list);
  return ret;
}
inline i32 gpu_buffer_group_alloc             (u32 gpu_id, char* buffer_id, u32 size, char* flags)              {buffer_id = buffer_id2group(buffer_id);i32 ret = __fuck_gpu_buffer_alloc            (__gpu_id_list, gpu_id, buffer_id, size, flags             );str_free(buffer_id);return ret;}
inline i32 gpu_buffer_group_free              (u32 gpu_id, char* buffer_id)                                     {buffer_id = buffer_id2group(buffer_id);i32 ret = __fuck_gpu_buffer_free             (__gpu_id_list, gpu_id, buffer_id                          );str_free(buffer_id);return ret;}
inline i32 gpu_buffer_group_h2d_frame         (u32 gpu_id, char* queue_id, u32 history_offset, char* buffer_id) {buffer_id = buffer_id2group(buffer_id);i32 ret = __fuck_gpu_buffer_h2d_frame        (__gpu_id_list, gpu_id, queue_id, history_offset, buffer_id);str_free(buffer_id);return ret;}
inline i32 gpu_buffer_group_h2d               (u32 gpu_id, char* queue_id, u32 device_ptr, char* buffer_id)     {buffer_id = buffer_id2group(buffer_id);i32 ret = __fuck_gpu_buffer_h2d              (__gpu_id_list, gpu_id, queue_id, device_ptr, buffer_id    );str_free(buffer_id);return ret;}
inline i32 gpu_buffer_group_d2h               (u32 gpu_id, char* queue_id, char* buffer_id, u32 device_ptr)     {buffer_id = buffer_id2group(buffer_id);i32 ret = __fuck_gpu_buffer_d2h              (__gpu_id_list, gpu_id, queue_id, buffer_id, device_ptr    );str_free(buffer_id);return ret;}

inline i32 gpu_kernel_decl_fs_direct    (u32 gpu_id, char* file_path, char* kernel_id)                    {return __fuck_wasm_gpu_kernel_decl_fs_direct(__gpu_id_list, gpu_id, file_path, kernel_id             );}
// inline u32 gpu_kernel_decl              (u32 gpu_id, char* s)                                            {return __fuck_gpu_kernel_decl             (__gpu_id_list, gpu_id, s                                  );}
inline i32 gpu_kernel_arg_set_const_i32 (u32 gpu_id, char* kernel_id, u32 arg_idx, i32 arg_value)         {return __fuck_gpu_kernel_arg_set_const_i32(__gpu_id_list, gpu_id, kernel_id, arg_idx, arg_value      );}
inline i32 gpu_kernel_arg_set_const_f32 (u32 gpu_id, char* kernel_id, u32 arg_idx, f32 arg_value)         {return __fuck_gpu_kernel_arg_set_const_f32(__gpu_id_list, gpu_id, kernel_id, arg_idx, arg_value      );}
inline i32 gpu_kernel_arg_set_buffer    (u32 gpu_id, char* kernel_id, u32 arg_idx, char* buffer_id)       {return __fuck_gpu_kernel_arg_set_buffer   (__gpu_id_list, gpu_id, kernel_id, arg_idx, buffer_id      );}
inline i32 gpu_kernel_set_global_work_size(u32 gpu_id, char* kernel_id, u32 arg_value)                    {return __fuck_gpu_kernel_set_global_work_size(__gpu_id_list, gpu_id, kernel_id, arg_value        );}
inline i32 gpu_kernel_set_local_work_size (u32 gpu_id, char* kernel_id, u32 arg_value)                    {return __fuck_gpu_kernel_set_local_work_size (__gpu_id_list, gpu_id, kernel_id, arg_value        );}
inline i32 gpu_kernel_call              (u32 gpu_id, char* queue_id, char* kernel_id)                     {return __fuck_gpu_kernel_call             (__gpu_id_list, gpu_id, queue_id, kernel_id                );}
inline i32 gpu_sync_queue               (u32 gpu_id, char* queue_id)                                      {return __fuck_gpu_sync_queue              (__gpu_id_list, gpu_id, queue_id                           );}