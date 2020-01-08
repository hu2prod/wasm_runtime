#pragma once

inline
i32 abs(i32 a) {
  return a < 0 ? -a : a;
}

inline
u32 min(u32 a, u32 b) {
  return a < b ? a : b;
}

inline
u32 max(u32 a, u32 b) {
  return a > b ? a : b;
}

inline
u64 round_f2i64(f64 x)
{
  if (x < 0.0)
    return (u64)(x - 0.5);
  else
    return (u64)(x + 0.5);
}

inline
u32 round_f2i(f32 x)
{
  if (x < 0.0)
    return (u32)(x - 0.5);
  else
    return (u32)(x + 0.5);
}
