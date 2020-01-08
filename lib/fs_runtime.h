#pragma once

u32 fs_read(char* s);
i32 fs_read_to_buf(char* s, u8* dst, u32 len);
i32 fs_write_str(char* s, char* cont);
i32 fs_write_buf(char* s, u8* cont, u32 len);
// TODO change img type
u32 fs_img_read(char* s);
i32 fs_img_write(char* s, u32 img);
