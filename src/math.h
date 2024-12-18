#ifndef MISSION_MATH_H
#define MISSION_MATH_H

#include <xmmintrin.h>
#include <float.h>
#include <math.h>

//

#include "api_types.h"

//
//
//

typedef __m128 v4sf;

#define MAT4_IDENTITY_INIT                            \
  {                                                   \
      1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, \
  }
static const mat4 mat4_identity = MAT4_IDENTITY_INIT;

#define make_mat4(e00, e01, e02, e03, e10, e11, e12, e13, e20, e21, e22, e23,  \
                  e30, e31, e32, e33)                                          \
  (mat4) {                                                                     \
    e00, e10, e20, e30, e01, e11, e21, e31, e02, e12, e22, e32, e03, e13, e23, \
        e33,                                                                   \
  }

#define PI32 3.1415926536f

MISSION_INLINE f32 to_radians(f32 deg) {
  return deg * (PI32 / 180.0f);
}

MISSION_INLINE f32 to_degrees(f32 rad) {
  return rad * (180.0f / PI32);
}

MISSION_INLINE f32 sqrt_ss(f32 x) {
  return _mm_cvtss_f32(_mm_sqrt_ss(_mm_set1_ps(x)));
}

MISSION_INLINE f32 rsqrt_ss(f32 x) {
  return _mm_cvtss_f32(_mm_rsqrt_ss(_mm_set1_ps(x)));
}

MISSION_INLINE f32 abs_ss(f32 x) {
  return (x < 0 ? -x : x);
}

MISSION_INLINE f32 sign_of(f32 x) {
  if (x >= 0.0f) {
    return 1.0f;
  } else {
    return -1.0f;
  }
}

MISSION_INLINE v4sf log_ps(v4sf x);
MISSION_INLINE v4sf exp_ps(v4sf x);
MISSION_INLINE v4sf sin_ps(v4sf x);
MISSION_INLINE v4sf cos_ps(v4sf x);
MISSION_INLINE void sincos_ps(v4sf x, v4sf* s, v4sf* c);

MISSION_INLINE f32 log_ss(f32 x) {
  return _mm_cvtss_f32(log_ps(_mm_set1_ps(x)));
}

MISSION_INLINE f32 exp_ss(f32 x) {
  return _mm_cvtss_f32(exp_ps(_mm_set1_ps(x)));
}

MISSION_INLINE f32 sin_ss(f32 x) {
  return _mm_cvtss_f32(sin_ps(_mm_set1_ps(x)));
}

MISSION_INLINE f32 cos_ss(f32 x) {
  return _mm_cvtss_f32(cos_ps(_mm_set1_ps(x)));
}

MISSION_INLINE void sincos_ss(f32 x, f32* s, f32* c) {
  v4sf s_ps, c_ps;
  sincos_ps(_mm_set1_ps(x), &s_ps, &c_ps);

  *s = _mm_cvtss_f32(s_ps);
  *c = _mm_cvtss_f32(c_ps);
}

MISSION_INLINE f32 tan_ss(f32 x) {
  f32 s, c;
  sincos_ss(x, &s, &c);

  return s / c;
}

MISSION_INLINE f32 fmod_ss(f32 x, f32 y) {
  int n = (int)(x / y);
  return x - n * y;
}

#ifdef NO_STD_LIB
  #pragma function(logf)
inline f32 logf(f32 x) {
  return log_ss(x);
}
  #pragma function(expf)
inline f32 expf(f32 x) {
  return exp_ss(x);
}
  #pragma function(sinf)
inline f32 sinf(f32 x) {
  return sin_ss(x);
}
  #pragma function(cosf)
inline f32 cosf(f32 x) {
  return cos_ss(x);
}
  #pragma function(tanf)
inline f32 tanf(f32 x) {
  return tan_ss(x);
}
  #pragma function(sqrtf)
inline f32 sqrtf(f32 x) {
  return sqrt_ss(x);
}
  #pragma function(fmodf)
inline f32 fmodf(f32 x, f32 y) {
  return fmod_ss(x, y);
}
  #pragma function(fabsf)
inline f32 fabsf(f32 x) {
  return abs_ss(x);
}
#endif

// float

MISSION_INLINE f32 f32_add(f32 l, f32 r) {
  return l + r;
}
MISSION_INLINE f32 f32_sub(f32 l, f32 r) {
  return l - r;
}
MISSION_INLINE f32 f32_mul(f32 l, f32 r) {
  return l * r;
}
MISSION_INLINE f32 f32_div(f32 l, f32 r) {
  return l / r;
}
MISSION_INLINE f32 f32_min(f32 a, f32 b) {
  return a < b ? a : b;
}
MISSION_INLINE f32 f32_max(f32 a, f32 b) {
  return a > b ? a : b;
}
MISSION_INLINE f32 f32_clamp(f32 x, f32 l, f32 h) {
  return x < l ? l : x > h ? h : x;
}
MISSION_INLINE f32 saturate(f32 x) {
  return x < 0.0f ? 0.0f : x > 1.0f ? 1.0f : x;
}

MISSION_INLINE bool f32_equal(f32 a, f32 b, f32 margin) {
  f32 diff = abs_ss(a - b);
  return diff <= margin;
}

MISSION_INLINE f32 lerp(f32 l, f32 r, f32 t) {
  return l * (1.0f - t) + r * t;
}

MISSION_INLINE f32 smoothstep(f32 x) {
  f32 result = x * x * (3.0f - 2.0f * x);
  return result;
}

MISSION_INLINE f32 smootherstep(f32 x) {
  f32 result = x * x * x * (x * (x * 6.0f - 15.0f) + 10.0f);
  return result;
}

// Color stuff

#define COLORF_WHITE make_v4(1, 1, 1, 1)
#define COLORF_BLACK make_v4(0, 0, 0, 1)
#define COLORF_RED make_v4(1, 0, 0, 1)
#define COLORF_GREEN make_v4(0, 1, 0, 1)
#define COLORF_BLUE make_v4(0, 0, 1, 1)
#define COLORF_MAGENTA make_v4(1, 0, 1, 1)
#define COLORF_CYAN make_v4(0, 1, 1, 1)
#define COLORF_YELLOW make_v4(1, 1, 0, 1)

#define COLOR32_WHITE pack_rgba(1, 1, 1, 1)
#define COLOR32_BLACK pack_rgba(0, 0, 0, 1)
#define COLOR32_RED pack_rgba(1, 0, 0, 1)
#define COLOR32_GREEN pack_rgba(0, 1, 0, 1)
#define COLOR32_BLUE pack_rgba(0, 0, 1, 1)
#define COLOR32_MAGENTA pack_rgba(1, 0, 1, 1)
#define COLOR32_CYAN pack_rgba(0, 1, 1, 1)
#define COLOR32_YELLOW pack_rgba(1, 1, 0, 1)

MISSION_INLINE v4 linear_to_srgb(v4 color) {
  color.xyz = (v3){
      .x = sqrt_ss(color.x),
      .y = sqrt_ss(color.y),
      .z = sqrt_ss(color.z),
  };
  return color;
}

MISSION_INLINE v4 srgb_to_linear(v4 color) {
  color.xyz = (v3){
      .x = color.x * color.x,
      .y = color.y * color.y,
      .z = color.z * color.z,
  };
  return color;
}

MISSION_INLINE u32 pack_color(v4 color) {
  color.x = CLAMP(color.x, 0.0f, 1.0f);
  color.y = CLAMP(color.y, 0.0f, 1.0f);
  color.z = CLAMP(color.z, 0.0f, 1.0f);
  color.w = CLAMP(color.w, 0.0f, 1.0f);

  color = linear_to_srgb(color);

  color.x *= color.w;
  color.y *= color.w;
  color.z *= color.w;

  u32 result =
      (((u32)(255.0f * color.x) << 0) | ((u32)(255.0f * color.y) << 8) |
       ((u32)(255.0f * color.z) << 16) | ((u32)(255.0f * color.w) << 24));
  return result;
}

MISSION_INLINE u32 pack_rgba(f32 r, f32 g, f32 b, f32 a) {
  r *= a;
  g *= a;
  b *= a;
  return pack_color((v4){r, g, b, a});
}

MISSION_INLINE u32 pack_rgb(f32 r, f32 g, f32 b) {
  return pack_color((v4){r, g, b, 1.0f});
}

MISSION_INLINE v4 unpack_color(u32 color) {
  f32 rcp_255 = 1.0f / 255.0f;

  v4 result;
  result.x = rcp_255 * (f32)((color >> 0) & 0xFF);
  result.y = rcp_255 * (f32)((color >> 8) & 0xFF);
  result.z = rcp_255 * (f32)((color >> 16) & 0xFF);
  result.w = rcp_255 * (f32)((color >> 24) & 0xFF);
  return result;
}

MISSION_INLINE unsigned pack_float11(f32 value) {
  if (value < 0.0f) {
    value = 0.0f;
  }
  if (value > 65024.0f) {
    value = 65024.0f;
  }

  union {
    f32 f;
    unsigned i;
  } u;

  u.f = value;

  int exponent = ((u.i >> 23) & 0xFF) - 127;
  int mantissa = u.i & MASK_BITS(22);

  return (((exponent + 15) << 6) | (mantissa >> (23 - 6))) & MASK_BITS(11);
}

MISSION_INLINE unsigned pack_float10(f32 value) {
  if (value < 0.0f) {
    value = 0.0f;
  }
  if (value > 64512.0f) {
    value = 64512.0f;
  }

  union {
    f32 f;
    unsigned i;
  } u;

  u.f = value;

  int exponent = ((u.i >> 23) & 0xFF) - 127;
  int mantissa = u.i & MASK_BITS(22);

  return (((exponent + 15) << 5) | (mantissa >> (23 - 5))) & MASK_BITS(10);
}

MISSION_INLINE u32 pack_r11g11b10f(v3 color) {
  unsigned r = pack_float11(color.x);
  unsigned g = pack_float11(color.y);
  unsigned b = pack_float10(color.z);

  unsigned result = (b << 22) | (g << 11) | r;

  return result;
}

// v2i

MISSION_INLINE bool v2i_are_equal(v2i a, v2i b) {
  return a.x == b.x && a.y == b.y;
}

// v2

#define DECLARE_VECTOR2_OP(name, op)            \
  MISSION_INLINE v2 v2_##name(v2 l, v2 r) {     \
    return (v2){l.x op r.x, l.y op r.y};        \
  }                                             \
  MISSION_INLINE v2 v2_s##name(f32 l, v2 r) {   \
    return (v2){l op r.x, l op r.y};            \
  }                                             \
  MISSION_INLINE v2 v2_##name##s(v2 l, f32 r) { \
    return (v2){l.x op r, l.y op r};            \
  }

DECLARE_VECTOR2_OP(add, +)
DECLARE_VECTOR2_OP(sub, -)
DECLARE_VECTOR2_OP(mul, *)
DECLARE_VECTOR2_OP(div, /)

MISSION_INLINE v2 make_v2(f32 x, f32 y) {
  return (v2){x, y};
}

MISSION_INLINE v2 v2_add3(v2 a, v2 b, v2 c) {
  v2 result = {
      a.x + b.x + c.x,
      a.y + b.y + c.y,
  };
  return result;
}

MISSION_INLINE f32 v2_dot(v2 l, v2 r) {
  return l.x * r.x + l.y * r.y;
}

MISSION_INLINE f32 v2_lensq(v2 x) {
  return x.x * x.x + x.y * x.y;
}

MISSION_INLINE f32 v2_len(v2 x) {
  return sqrt_ss(x.x * x.x + x.y * x.y);
}

MISSION_INLINE v2 v2_normalize(v2 x) {
  f32 rcp_len = 1.0f / sqrt_ss(x.x * x.x + x.y * x.y);
  return (v2){rcp_len * x.x, rcp_len * x.y};
}

MISSION_INLINE v2 v2_normalize_or_zero(v2 x) {
  f32 len = sqrt_ss(x.x * x.x + x.y * x.y);

  f32 rcp_len = 0.0f;
  if (len > 0.001f) {
    rcp_len = 1.0f / len;
  }
  return (v2){rcp_len * x.x, rcp_len * x.y};
}

MISSION_INLINE v2 v2_negate(v2 x) {
  return (v2){-x.x, -x.y};
}

MISSION_INLINE v2 v2_lerp(v2 l, v2 r, v2 t) {
  v2 inv_t = {1.0f - t.x, 1.0f - t.y};
  return (v2){l.x * inv_t.x + r.x * t.x, l.y * inv_t.y + r.y * t.y};
}

MISSION_INLINE v2 v2_lerps(v2 l, v2 r, f32 t) {
  f32 inv_t = 1.0f - t;
  return (v2){l.x * inv_t + r.x * t, l.y * inv_t + r.y * t};
}

MISSION_INLINE v2 v2_min(v2 l, v2 r) {
  return (v2){
      l.x < r.x ? l.x : r.x,
      l.y < r.y ? l.y : r.y,
  };
}

MISSION_INLINE v2 v2_smin(f32 l, v2 r) {
  return (v2){
      l < r.x ? l : r.x,
      l < r.y ? l : r.y,
  };
}

MISSION_INLINE v2 v2_mins(v2 l, f32 r) {
  return (v2){
      l.x < r ? l.x : r,
      l.y < r ? l.y : r,
  };
}

MISSION_INLINE v2 v2_max(v2 l, v2 r) {
  return (v2){
      l.x > r.x ? l.x : r.x,
      l.y > r.y ? l.y : r.y,
  };
}

MISSION_INLINE v2 v2_smax(f32 l, v2 r) {
  return (v2){
      l > r.x ? l : r.x,
      l > r.y ? l : r.y,
  };
}

MISSION_INLINE v2 v2_maxs(v2 l, f32 r) {
  return (v2){
      l.x > r ? l.x : r,
      l.y > r ? l.y : r,
  };
}

MISSION_INLINE f32 v2_minval(v2 v) {
  f32 m;
  m = v.x;
  m = v.y < m ? v.y : m;
  return m;
}

MISSION_INLINE f32 v2_maxval(v2 v) {
  f32 m;
  m = v.x;
  m = v.y > m ? v.y : m;
  return m;
}

// v3

#define DECLARE_VECTOR3_OP(name, op)                 \
  MISSION_INLINE v3 v3_##name(v3 l, v3 r) {          \
    return (v3){l.x op r.x, l.y op r.y, l.z op r.z}; \
  }                                                  \
  MISSION_INLINE v3 v3_s##name(f32 l, v3 r) {        \
    return (v3){l op r.x, l op r.y, l op r.z};       \
  }                                                  \
  MISSION_INLINE v3 v3_##name##s(v3 l, f32 r) {      \
    return (v3){l.x op r, l.y op r, l.z op r};       \
  }

DECLARE_VECTOR3_OP(add, +)
DECLARE_VECTOR3_OP(sub, -)
DECLARE_VECTOR3_OP(mul, *)
DECLARE_VECTOR3_OP(div, /)

MISSION_INLINE v3 make_v3(f32 x, f32 y, f32 z) {
  return (v3){x, y, z};
}

MISSION_INLINE v3 v3_add3(v3 a, v3 b, v3 c) {
  v3 result = {
      a.x + b.x + c.x,
      a.y + b.y + c.y,
      a.z + b.z + c.z,
  };
  return result;
}

MISSION_INLINE f32 v3_dot(v3 l, v3 r) {
  return l.x * r.x + l.y * r.y + l.z * r.z;
}

MISSION_INLINE f32 v3_lensq(v3 x) {
  return x.x * x.x + x.y * x.y + x.z * x.z;
}

MISSION_INLINE f32 v3_len(v3 x) {
  return sqrt_ss(x.x * x.x + x.y * x.y + x.z * x.z);
}

MISSION_INLINE v3 v3_normalize(v3 x) {
  f32 rcp_len = 1.0f / sqrt_ss(x.x * x.x + x.y * x.y + x.z * x.z);
  return (v3){rcp_len * x.x, rcp_len * x.y, rcp_len * x.z};
}

MISSION_INLINE v3 v3_normalize_or_zero(v3 x) {
  f32 len = sqrt_ss(x.x * x.x + x.y * x.y + x.z * x.z);

  f32 rcp_len = 0.0f;
  if (len > 0.001f) {
    rcp_len = 1.0f / len;
  }
  return (v3){rcp_len * x.x, rcp_len * x.y, rcp_len * x.z};
}

MISSION_INLINE v3 v3_negate(v3 x) {
  return (v3){-x.x, -x.y, -x.z};
}

MISSION_INLINE v3 v3_lerp(v3 l, v3 r, v3 t) {
  v3 inv_t = {1.0f - t.x, 1.0f - t.y, 1.0f - t.z};
  return (v3){l.x * inv_t.x + r.x * t.x, l.y * inv_t.y + r.y * t.y,
              l.z * inv_t.z + r.z * t.z};
}

MISSION_INLINE v3 v3_lerps(v3 l, v3 r, f32 t) {
  f32 inv_t = 1.0f - t;
  return (v3){l.x * inv_t + r.x * t, l.y * inv_t + r.y * t,
              l.z * inv_t + r.z * t};
}

MISSION_INLINE v3 v3_min(v3 l, v3 r) {
  return (v3){
      l.x < r.x ? l.x : r.x,
      l.y < r.y ? l.y : r.y,
      l.z < r.z ? l.z : r.z,
  };
}

MISSION_INLINE v3 v3_smin(f32 l, v3 r) {
  return (v3){
      l < r.x ? l : r.x,
      l < r.y ? l : r.y,
      l < r.z ? l : r.z,
  };
}

MISSION_INLINE v3 v3_mins(v3 l, f32 r) {
  return (v3){
      l.x < r ? l.x : r,
      l.y < r ? l.y : r,
      l.z < r ? l.z : r,
  };
}

MISSION_INLINE f32 v3_max3(v3 v) {
  f32 result = (v.x > v.y ? v.x : v.y);
  result = (v.z > result ? v.z : result);
  return result;
}

MISSION_INLINE v3 v3_max(v3 l, v3 r) {
  return (v3){
      l.x > r.x ? l.x : r.x,
      l.y > r.y ? l.y : r.y,
      l.z > r.z ? l.z : r.z,
  };
}

MISSION_INLINE v3 v3_smax(f32 l, v3 r) {
  return (v3){
      l > r.x ? l : r.x,
      l > r.y ? l : r.y,
      l > r.z ? l : r.z,
  };
}

MISSION_INLINE v3 v3_maxs(v3 l, f32 r) {
  return (v3){
      l.x > r ? l.x : r,
      l.y > r ? l.y : r,
      l.z > r ? l.z : r,
  };
}

MISSION_INLINE f32 v3_minval(v3 v) {
  f32 m;
  m = v.x;
  m = v.y < m ? v.y : m;
  m = v.z < m ? v.z : m;
  return m;
}

MISSION_INLINE f32 v3_maxval(v3 v) {
  f32 m;
  m = v.x;
  m = v.y > m ? v.y : m;
  m = v.z > m ? v.z : m;
  return m;
}

MISSION_INLINE v3 cross(v3 a, v3 b) {
  v3 result;
  result.x = a.y * b.z - a.z * b.y;
  result.y = a.z * b.x - a.x * b.z;
  result.z = a.x * b.y - a.y * b.x;
  return result;
}

static bool v3_equal(v3 a, v3 b, f32 margin) {
  return (f32_equal(a.x, b.x, margin) && f32_equal(a.y, b.y, margin) &&
          f32_equal(a.z, b.z, margin));
}

// v4

#define DECLARE_VECTOR4_OP(name, op)                             \
  MISSION_INLINE v4 v4_##name(v4 l, v4 r) {                      \
    return (v4){l.x op r.x, l.y op r.y, l.z op r.z, l.w op r.w}; \
  }                                                              \
  MISSION_INLINE v4 v4_s##name(f32 l, v4 r) {                    \
    return (v4){l op r.x, l op r.y, l op r.z, l op r.w};         \
  }                                                              \
  MISSION_INLINE v4 v4_##name##s(v4 l, f32 r) {                  \
    return (v4){l.x op r, l.y op r, l.z op r, l.w op r};         \
  }

DECLARE_VECTOR4_OP(add, +)
DECLARE_VECTOR4_OP(sub, -)
DECLARE_VECTOR4_OP(mul, *)
DECLARE_VECTOR4_OP(div, /)

MISSION_INLINE v4 make_v4(f32 x, f32 y, f32 z, f32 w) {
  return (v4){x, y, z, w};
}

MISSION_INLINE f32 v4_dot(v4 l, v4 r) {
  return l.x * r.x + l.y * r.y + l.z * r.z + l.w * r.w;
}

MISSION_INLINE f32 v4_lensq(v4 x) {
  return x.x * x.x + x.y * x.y + x.z * x.z + x.w * x.w;
}

MISSION_INLINE f32 v4_len(v4 x) {
  return sqrt_ss(x.x * x.x + x.y * x.y + x.z * x.z + x.w * x.w);
}

MISSION_INLINE v4 v4_normalize(v4 x) {
  f32 rcp_len = 1.0f / sqrt_ss(x.x * x.x + x.y * x.y + x.z * x.z + x.w * x.w);
  return (v4){rcp_len * x.x, rcp_len * x.y, rcp_len * x.z, rcp_len * x.w};
}

MISSION_INLINE v4 v4_normalize_or_zero(v4 x) {
  f32 len = sqrt_ss(x.x * x.x + x.y * x.y + x.z * x.z + x.w * x.w);

  f32 rcp_len = 0.0f;
  if (len > 0.001f) {
    rcp_len = 1.0f / len;
  }
  return (v4){rcp_len * x.x, rcp_len * x.y, rcp_len * x.z, rcp_len * x.w};
}

MISSION_INLINE v4 v4_negate(v4 x) {
  return (v4){-x.x, -x.y, -x.z, -x.w};
}

MISSION_INLINE v4 v4_lerp(v4 l, v4 r, v4 t) {
  v4 inv_t = {1.0f - t.x, 1.0f - t.y, 1.0f - t.z, 1.0f - t.w};
  return (v4){l.x * inv_t.x + r.x * t.x, l.y * inv_t.y + r.y * t.y,
              l.z * inv_t.z + r.z * t.z, l.w * inv_t.w + r.w * t.w};
}

MISSION_INLINE v4 v4_lerps(v4 l, v4 r, f32 t) {
  f32 inv_t = 1.0f - t;
  return (v4){l.x * inv_t + r.x * t, l.y * inv_t + r.y * t,
              l.z * inv_t + r.z * t, l.w * inv_t + r.w * t};
}

MISSION_INLINE v4 v4_min(v4 l, v4 r) {
  return (v4){
      l.x < r.x ? l.x : r.x,
      l.y < r.y ? l.y : r.y,
      l.z < r.z ? l.z : r.z,
      l.w < r.w ? l.w : r.w,
  };
}

MISSION_INLINE v4 v4_smin(f32 l, v4 r) {
  return (v4){
      l < r.x ? l : r.x,
      l < r.y ? l : r.y,
      l < r.z ? l : r.z,
      l < r.w ? l : r.w,
  };
}

MISSION_INLINE v4 v4_mins(v4 l, f32 r) {
  return (v4){
      l.x < r ? l.x : r,
      l.y < r ? l.y : r,
      l.z < r ? l.z : r,
      l.w < r ? l.w : r,
  };
}

MISSION_INLINE v4 v4_max(v4 l, v4 r) {
  return (v4){
      l.x > r.x ? l.x : r.x,
      l.y > r.y ? l.y : r.y,
      l.z > r.z ? l.z : r.z,
      l.w > r.w ? l.w : r.w,
  };
}

MISSION_INLINE v4 v4_smax(f32 l, v4 r) {
  return (v4){
      l > r.x ? l : r.x,
      l > r.y ? l : r.y,
      l > r.z ? l : r.z,
      l > r.w ? l : r.w,
  };
}

MISSION_INLINE v4 v4_maxs(v4 l, f32 r) {
  return (v4){
      l.x > r ? l.x : r,
      l.y > r ? l.y : r,
      l.z > r ? l.z : r,
      l.w > r ? l.w : r,
  };
}

MISSION_INLINE f32 v4_minval(v4 v) {
  f32 m;
  m = v.x;
  m = v.y < m ? v.y : m;
  m = v.z < m ? v.z : m;
  m = v.w < m ? v.w : m;
  return m;
}

MISSION_INLINE f32 v4_maxval(v4 v) {
  f32 m;
  m = v.x;
  m = v.y > m ? v.y : m;
  m = v.z > m ? v.z : m;
  m = v.w > m ? v.w : m;
  return m;
}

MISSION_INLINE v4 v4_add3(v4 a, v4 b, v4 c) {
  v4 result = {
      a.x + b.x + c.x,
      a.y + b.y + c.y,
      a.z + b.z + c.z,
      a.w + b.w + c.w,
  };
  return result;
}

MISSION_INLINE v4 v4_add4(v4 a, v4 b, v4 c, v4 d) {
  v4 result = {
      a.x + b.x + c.x + d.x,
      a.y + b.y + c.y + d.y,
      a.z + b.z + c.z + d.z,
      a.w + b.w + c.w + d.w,
  };
  return result;
}

// mat4

MISSION_INLINE mat4 mat4_mul(mat4 b, mat4 a) {
  mat4 result;
  result.e[0][0] = a.e[0][0] * b.e[0][0] + a.e[0][1] * b.e[1][0] +
                   a.e[0][2] * b.e[2][0] + a.e[0][3] * b.e[3][0];
  result.e[0][1] = a.e[0][0] * b.e[0][1] + a.e[0][1] * b.e[1][1] +
                   a.e[0][2] * b.e[2][1] + a.e[0][3] * b.e[3][1];
  result.e[0][2] = a.e[0][0] * b.e[0][2] + a.e[0][1] * b.e[1][2] +
                   a.e[0][2] * b.e[2][2] + a.e[0][3] * b.e[3][2];
  result.e[0][3] = a.e[0][0] * b.e[0][3] + a.e[0][1] * b.e[1][3] +
                   a.e[0][2] * b.e[2][3] + a.e[0][3] * b.e[3][3];
  result.e[1][0] = a.e[1][0] * b.e[0][0] + a.e[1][1] * b.e[1][0] +
                   a.e[1][2] * b.e[2][0] + a.e[1][3] * b.e[3][0];
  result.e[1][1] = a.e[1][0] * b.e[0][1] + a.e[1][1] * b.e[1][1] +
                   a.e[1][2] * b.e[2][1] + a.e[1][3] * b.e[3][1];
  result.e[1][2] = a.e[1][0] * b.e[0][2] + a.e[1][1] * b.e[1][2] +
                   a.e[1][2] * b.e[2][2] + a.e[1][3] * b.e[3][2];
  result.e[1][3] = a.e[1][0] * b.e[0][3] + a.e[1][1] * b.e[1][3] +
                   a.e[1][2] * b.e[2][3] + a.e[1][3] * b.e[3][3];
  result.e[2][0] = a.e[2][0] * b.e[0][0] + a.e[2][1] * b.e[1][0] +
                   a.e[2][2] * b.e[2][0] + a.e[2][3] * b.e[3][0];
  result.e[2][1] = a.e[2][0] * b.e[0][1] + a.e[2][1] * b.e[1][1] +
                   a.e[2][2] * b.e[2][1] + a.e[2][3] * b.e[3][1];
  result.e[2][2] = a.e[2][0] * b.e[0][2] + a.e[2][1] * b.e[1][2] +
                   a.e[2][2] * b.e[2][2] + a.e[2][3] * b.e[3][2];
  result.e[2][3] = a.e[2][0] * b.e[0][3] + a.e[2][1] * b.e[1][3] +
                   a.e[2][2] * b.e[2][3] + a.e[2][3] * b.e[3][3];
  result.e[3][0] = a.e[3][0] * b.e[0][0] + a.e[3][1] * b.e[1][0] +
                   a.e[3][2] * b.e[2][0] + a.e[3][3] * b.e[3][0];
  result.e[3][1] = a.e[3][0] * b.e[0][1] + a.e[3][1] * b.e[1][1] +
                   a.e[3][2] * b.e[2][1] + a.e[3][3] * b.e[3][1];
  result.e[3][2] = a.e[3][0] * b.e[0][2] + a.e[3][1] * b.e[1][2] +
                   a.e[3][2] * b.e[2][2] + a.e[3][3] * b.e[3][2];
  result.e[3][3] = a.e[3][0] * b.e[0][3] + a.e[3][1] * b.e[1][3] +
                   a.e[3][2] * b.e[2][3] + a.e[3][3] * b.e[3][3];
  return result;
}

MISSION_INLINE v4 mat4_mulv(mat4 a, v4 b) {
  v4 result;
  result.x =
      a.e[0][0] * b.x + a.e[1][0] * b.y + a.e[2][0] * b.z + a.e[3][0] * b.w;
  result.y =
      a.e[0][1] * b.x + a.e[1][1] * b.y + a.e[2][1] * b.z + a.e[3][1] * b.w;
  result.z =
      a.e[0][2] * b.x + a.e[1][2] * b.y + a.e[2][2] * b.z + a.e[3][2] * b.w;
  result.w =
      a.e[0][3] * b.x + a.e[1][3] * b.y + a.e[2][3] * b.z + a.e[3][3] * b.w;
  return result;
}

// generic matrix stuff

bool solve_system_of_equations(mat* m, f32* x);

#define GENERIC_VECTOR_OP(name, v) \
  _Generic(v, v2: v2_##name, v3: v3_##name, v4: v4_##name)(v)

#define vlensq(v) GENERIC_VECTOR_OP(lensq, v)
#define vlen(v) GENERIC_VECTOR_OP(len, v)
// #define vlen_sq(v) GENERIC_VECTOR_OP(lensq, v)
#define vminval(v) GENERIC_VECTOR_OP(minval, v)
#define vmaxval(v) GENERIC_VECTOR_OP(maxval, v)
#define normalize(v) GENERIC_VECTOR_OP(normalize, v)
#define normalize_or_zero(v) GENERIC_VECTOR_OP(normalize_or_zero, v)
#define negate(v) GENERIC_VECTOR_OP(negate, v)

#define GENERIC_VECTOR_BINARY_OP(name, l, r) \
  _Generic((l),                              \
      f32: _Generic((r),                     \
      f32: f32_##name,                       \
      v2: v2_s##name,                        \
      v3: v3_s##name,                        \
      v4: v4_s##name,                        \
      mat4: NULL),                           \
      v2: _Generic((r),                      \
      f32: v2_##name##s,                     \
      v2: v2_##name,                         \
      v3: NULL,                              \
      v4: NULL,                              \
      mat4: NULL),                           \
      v3: _Generic((r),                      \
      f32: v3_##name##s,                     \
      v2: NULL,                              \
      v3: v3_##name,                         \
      v4: NULL,                              \
      mat4: NULL),                           \
      v4: _Generic((r),                      \
      f32: v4_##name##s,                     \
      v2: NULL,                              \
      v3: NULL,                              \
      v4: v4_##name,                         \
      mat4: NULL),                           \
      mat4: _Generic((r),                    \
      f32: NULL,                             \
      v2: NULL,                              \
      v3: NULL,                              \
      v4: mat4_mulv,                         \
      mat4: mat4_mul))(l, r)

#undef min
#undef max

#define add(l, r) GENERIC_VECTOR_BINARY_OP(add, l, r)
#define sub(l, r) GENERIC_VECTOR_BINARY_OP(sub, l, r)
#define mul(l, r) GENERIC_VECTOR_BINARY_OP(mul, l, r)
#define div(l, r) GENERIC_VECTOR_BINARY_OP(div, l, r)
#define min(l, r) GENERIC_VECTOR_BINARY_OP(min, l, r)
#define max(l, r) GENERIC_VECTOR_BINARY_OP(max, l, r)

#define dot(l, r) _Generic(l, v2: v2_dot, v3: v3_dot, v4: v4_dot)(l, r)

MISSION_INLINE void basis_vectors(v3 d, v3 up, v3* x, v3* y, v3* z) {
  *z = negate(d);
  *x = normalize(cross(up, *z));
  *y = normalize(cross(*z, *x));
}

// quat

MISSION_INLINE quat make_quat(v3 imag, f32 real) {
  return (quat){imag.x, imag.y, imag.z, real};
}

MISSION_INLINE quat make_quat_axis_angle(v3 axis, f32 angle) {
  axis = normalize(axis);

  f32 s, c;
  sincos_ss(0.5f * angle, &s, &c);

  quat result = make_quat(mul(s, axis), c);
  return result;
}

MISSION_INLINE quat quat_mul(quat a, quat b) {
  quat result;
  result.x =
      a.e[3] * b.e[0] + a.e[0] * b.e[3] + a.e[1] * b.e[2] - a.e[2] * b.e[1];
  result.y =
      a.e[3] * b.e[1] - a.e[0] * b.e[2] + a.e[1] * b.e[3] + a.e[2] * b.e[0];
  result.z =
      a.e[3] * b.e[2] + a.e[0] * b.e[1] - a.e[1] * b.e[0] + a.e[2] * b.e[3];
  result.w =
      a.e[3] * b.e[3] - a.e[0] * b.e[0] - a.e[1] * b.e[1] - a.e[2] * b.e[2];
  return result;
}

MISSION_INLINE quat quat_conjugate(quat q) {
  return (quat){-q.x, -q.y, -q.z, q.w};
}

MISSION_INLINE v3 quat_imag(quat q) {
  return (v3){q.x, q.y, q.z};
}

MISSION_INLINE f32 quat_real(quat q) {
  return q.w;
}

MISSION_INLINE v3 quat_rotatev(quat q, v3 v) {
  quat p = make_quat(v, 0.0f);

  quat r;
  r = quat_mul(q, p);
  r = quat_mul(r, quat_conjugate(q));

  return quat_imag(r);
}

MISSION_INLINE mat4 mat4_from_quat(quat q) {
  mat4 result;

  result.e[3][0] = 0.0f;
  result.e[3][1] = 0.0f;
  result.e[3][2] = 0.0f;

  result.e[0][3] = 0.0f;
  result.e[1][3] = 0.0f;
  result.e[2][3] = 0.0f;
  result.e[3][3] = 1.0f;

  f32 qx = 2.0f * q.x * q.x;
  f32 qy = 2.0f * q.y * q.y;
  f32 qz = 2.0f * q.z * q.z;
  f32 qxqy = 2.0f * q.x * q.y;
  f32 qxqz = 2.0f * q.x * q.z;
  f32 qxqw = 2.0f * q.x * q.w;
  f32 qyqz = 2.0f * q.y * q.z;
  f32 qyqw = 2.0f * q.y * q.w;
  f32 qzqw = 2.0f * q.z * q.w;

  result.e[0][0] = 1.0f - qy - qz;
  result.e[1][1] = 1.0f - qx - qz;
  result.e[2][2] = 1.0f - qx - qy;

  result.e[1][0] = qxqy + qzqw;
  result.e[2][0] = qxqz - qyqw;

  result.e[0][1] = qxqy - qzqw;
  result.e[2][1] = qyqz + qxqw;

  result.e[0][2] = qxqz + qyqw;
  result.e[1][2] = qyqz - qxqw;

  return result;
}

// mat4

MISSION_INLINE mat4 transpose(mat4 mat) {
  mat4 result;
  for (size_t i = 0; i < 4; i++) {
    for (size_t j = 0; j < 4; j++) {
      result.e[j][i] = mat.e[i][j];
    }
  }
  return result;
}

MISSION_INLINE mat4 translate(mat4 mat, v3 p) {
  mat.col[3] = add(mat.col[3], mul(mat.col[0], p.x));
  mat.col[3] = add(mat.col[3], mul(mat.col[1], p.y));
  mat.col[3] = add(mat.col[3], mul(mat.col[2], p.z));
  return mat;
}

MISSION_INLINE mat4 make_view_matrix(v3 p, v3 d, v3 up) {
  v3 x, y, z;
  basis_vectors(d, up, &x, &y, &z);

  mat4 mat = mat4_identity;

  mat.e[0][0] = x.x;
  mat.e[1][0] = x.y;
  mat.e[2][0] = x.z;

  mat.e[0][1] = y.x;
  mat.e[1][1] = y.y;
  mat.e[2][1] = y.z;

  mat.e[0][2] = z.x;
  mat.e[1][2] = z.y;
  mat.e[2][2] = z.z;

  mat = translate(mat, negate(p));
  return mat;
}

MISSION_INLINE mat4 make_orthographic_matrix(f32 w, f32 h, f32 d) {
  mat4 result =
      make_mat4(2.0f / w, 0.0f, 0.0f, 0.0f, 0.0f, 2.0f / h, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f / d, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
  return result;
}

MISSION_INLINE mat4 make_perspective_matrix(f32 vfov,
                                            f32 w_over_h,
                                            f32 near_plane) {
  f32 g = 1.0f / tan_ss(to_radians(0.5f * vfov));
  f32 s = w_over_h;
  f32 n = near_plane;

  f32 e = 0.000001f;
  mat4 result = make_mat4(g / s, 0, 0, 0, 0, g, 0, 0, 0, 0, e, n * (1.0f - e),
                          0, 0, -1, 0);

  return result;
}

//
// rect2i
//

MISSION_INLINE bool rect2i_contains_inclusive(rect2i rect, v2i p) {
  return (p.x >= rect.min.x && p.x <= rect.max.x && p.y >= rect.min.y &&
          p.y <= rect.max.y);
}

MISSION_INLINE bool rect2i_contains_exclusive(rect2i rect, v2i p) {
  return (p.x >= rect.min.x && p.x < rect.max.x && p.y >= rect.min.y &&
          p.y < rect.max.y);
}

MISSION_INLINE v2i rect2i_get_dim(rect2i rect) {
  v2i result = {
      rect.max.x - rect.min.x,
      rect.max.y - rect.min.y,
  };
  return result;
}

//
// rect2
//

MISSION_INLINE rect2 rect2_min_dim(v2 min, v2 dim) {
  rect2 result = {
      .min = min,
      .max = add(min, dim),
  };
  return result;
}

MISSION_INLINE rect2 rect2_center_dim(v2 center, v2 dim) {
  rect2 result = {
      .min = sub(center, mul(0.5f, dim)),
      .max = add(center, mul(0.5f, dim)),
  };
  return result;
}

MISSION_INLINE rect2 rect2_add_radius(rect2 rect, v2 radius) {
  rect2 result = {
      .min = sub(rect.min, radius),
      .max = add(rect.max, radius),
  };
  return result;
}

MISSION_INLINE rect2 rect2_sub_radius(rect2 rect, v2 radius) {
  rect2 result = {
      .min = add(rect.min, radius),
      .max = sub(rect.max, radius),
  };
  return result;
}

MISSION_INLINE bool rect2_contains_point(rect2 rect, v2 point) {
  return (point.x >= rect.min.x && point.x < rect.max.x &&
          point.y >= rect.min.y && point.y < rect.max.y);
}

MISSION_INLINE rect2 rect2_intersect(rect2 a, rect2 b) {
  rect2 result = {
      .min = max(a.min, b.min),
      .max = min(a.max, b.max),
  };
  return result;
}

MISSION_INLINE rect2 rect2_uninvert(rect2 rect) {
  rect2 result = {
      .min = min(rect.min, rect.max),
      .max = max(rect.min, rect.max),
  };

  return result;
}

MISSION_INLINE rect2 rect2_infinity(void) {
  rect2 result = {
      .min = {-FLT_MAX, -FLT_MAX},
      .max = {FLT_MAX, FLT_MAX},
  };
  return result;
}

MISSION_INLINE rect2 rect2_add_offset(rect2 rect, v2 offset) {
  rect2 result = {
      .min = add(rect.min, offset),
      .max = add(rect.max, offset),
  };
  return result;
}

MISSION_INLINE v2 rect2_center(rect2 rect) {
  return mul(0.5f, add(rect.min, rect.max));
}

MISSION_INLINE v2 rect2_get_dim(rect2 rect) {
  v2 result = {
      rect.max.x - rect.min.x,
      rect.max.y - rect.min.y,
  };
  return result;
}

MISSION_INLINE f32 rect2_width(rect2 rect) {
  return rect.max.x - rect.min.x;
}

MISSION_INLINE f32 rect2_height(rect2 rect) {
  return rect.max.y - rect.min.y;
}

MISSION_INLINE f32 rect2_area(rect2 rect) {
  f32 w = rect2_width(rect);
  f32 h = rect2_height(rect);
  return w * h;
}

//
// rect3
//

MISSION_INLINE rect3 rect3_inverted_infinity(void) {
  rect3 result = {
      .min = {FLT_MAX, FLT_MAX, FLT_MAX},
      .max = {-FLT_MAX, -FLT_MAX, -FLT_MAX},
  };
  return result;
}

MISSION_INLINE rect3 rect3_grow_to_contain(rect3 rect, v3 p) {
  rect3 result = {
      .min = min(rect.min, p),
      .max = max(rect.max, p),
  };
  return result;
}

MISSION_INLINE rect3 rect3_union(rect3 a, rect3 b) {
  rect3 result = {
      .min = min(a.min, b.min),
      .max = max(a.max, b.max),
  };
  return result;
}

MISSION_INLINE rect3 rect3_add(rect3 a, rect3 b) {
  rect3 result = {
      .min = add(a.min, b.min),
      .max = add(a.max, b.max),
  };
  return result;
}

MISSION_INLINE rect3 rect3_grow_radius(rect3 rect, v3 radius) {
  rect3 result = {
      .min = sub(rect.min, radius),
      .max = add(rect.max, radius),
  };
  return result;
}

MISSION_INLINE v3 rect3_center(rect3 rect) {
  return mul(0.5f, add(rect.min, rect.max));
}

MISSION_INLINE v3 rect3_dim(rect3 rect) {
  return sub(rect.max, rect.min);
}

MISSION_INLINE rect3 rect3_center_radius(v3 center, v3 radius) {
  rect3 result = {
      .min = sub(center, radius),
      .max = add(center, radius),
  };
  return result;
}

MISSION_INLINE u8 rect3_largest_axis(rect3 rect) {
  v3 dim = {rect.max.x - rect.min.x, rect.max.y - rect.min.y,
            rect.max.z - rect.min.z};

  u8 result = 0;

  f32 largest = dim.x;
  if (dim.y > largest) {
    result = 1;
    largest = dim.y;
  }
  if (dim.z > largest) {
    result = 2;
  }
  return result;
}

MISSION_INLINE void get_tangent_vectors(v3 n, v3* t, v3* b) {
  v3 up = {0, 0, 1};
  *t = cross(up, n);

  if (vlen(*t) <= 0.01f) {
    up = (v3){0, 1, 0};
    *t = cross(up, n);
  }

  *t = normalize(*t);
  *b = normalize(cross(n, *t));
}

MISSION_INLINE void plane_from_points(v3 a, v3 b, v3 c, plane_t* p) {
  p->n = normalize(cross(sub(c, a), sub(b, a)));
  p->d = dot(p->n, a);
}

// Steam hardware survey says SSE2 availability is 100%
// That seems good enough to me.
#define USE_SSE2

// below is Julien Pommier's sse_mathfun.h
// (http://gruntthepeon.free.fr/ssemath/) with minor alterations:

/* SIMD (SSE1+MMX or SSE2) implementation of sin, cos, exp and log

   Inspired by Intel Approximate Math library, and based on the
   corresponding algorithms of the cephes math library

   The default is to use the SSE1 version. If you define USE_SSE2 the
   the SSE2 intrinsics will be used in place of the MMX intrinsics. Do
   not expect any significant performance improvement with SSE2.
*/

/* Copyright (C) 2007  Julien Pommier

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  (this is the zlib license)
*/

/* yes I know, the top of this file is quite ugly */

#ifdef _MSC_VER /* visual c++ */
  #define ALIGN16_BEG __declspec(align(16))
  #define ALIGN16_END
#else /* gcc or icc */
  #define ALIGN16_BEG
  #define ALIGN16_END __attribute__((aligned(16)))
#endif

#ifdef USE_SSE2
  #include <emmintrin.h>
typedef __m128i v4si;  // vector of 4 int (sse2)
#else
typedef __m64 v2si;  // vector of 2 int (mmx)
#endif

/* declare some SSE constants -- why can't I figure a better way to do that? */
#define _PS_CONST(Name, Val)                                   \
  static const ALIGN16_BEG float _ps_##Name[4] ALIGN16_END = { \
      (float)Val, (float)Val, (float)Val, (float)Val}
#define _PI32_CONST(Name, Val)                                               \
  static const ALIGN16_BEG int _pi32_##Name[4] ALIGN16_END = {Val, Val, Val, \
                                                              Val}
#define _PS_CONST_TYPE(Name, Type, Val) \
  static const ALIGN16_BEG Type _ps_##Name[4] ALIGN16_END = {Val, Val, Val, Val}

_PS_CONST(1, 1.0f);
_PS_CONST(0p5, 0.5f);
/* the smallest non denormalized float number */
_PS_CONST_TYPE(min_norm_pos, int, 0x00800000);
_PS_CONST_TYPE(mant_mask, int, 0x7f800000);
_PS_CONST_TYPE(inv_mant_mask, int, ~0x7f800000);

_PS_CONST_TYPE(sign_mask, int, (int)0x80000000);
_PS_CONST_TYPE(inv_sign_mask, int, ~0x80000000);

_PI32_CONST(1, 1);
_PI32_CONST(inv1, ~1);
_PI32_CONST(2, 2);
_PI32_CONST(4, 4);
_PI32_CONST(0x7f, 0x7f);

_PS_CONST(cephes_SQRTHF, 0.707106781186547524);
_PS_CONST(cephes_log_p0, 7.0376836292E-2);
_PS_CONST(cephes_log_p1, -1.1514610310E-1);
_PS_CONST(cephes_log_p2, 1.1676998740E-1);
_PS_CONST(cephes_log_p3, -1.2420140846E-1);
_PS_CONST(cephes_log_p4, +1.4249322787E-1);
_PS_CONST(cephes_log_p5, -1.6668057665E-1);
_PS_CONST(cephes_log_p6, +2.0000714765E-1);
_PS_CONST(cephes_log_p7, -2.4999993993E-1);
_PS_CONST(cephes_log_p8, +3.3333331174E-1);
_PS_CONST(cephes_log_q1, -2.12194440e-4);
_PS_CONST(cephes_log_q2, 0.693359375);

#ifndef USE_SSE2
typedef union xmm_mm_union {
  __m128 xmm;
  __m64 mm[2];
} xmm_mm_union;

  #define COPY_XMM_TO_MM(xmm_, mm0_, mm1_) \
    {                                      \
      xmm_mm_union u;                      \
      u.xmm = xmm_;                        \
      mm0_ = u.mm[0];                      \
      mm1_ = u.mm[1];                      \
    }

  #define COPY_MM_TO_XMM(mm0_, mm1_, xmm_) \
    {                                      \
      xmm_mm_union u;                      \
      u.mm[0] = mm0_;                      \
      u.mm[1] = mm1_;                      \
      xmm_ = u.xmm;                        \
    }

#endif  // USE_SSE2

/* natural logarithm computed for 4 simultaneous float
   return NaN for x <= 0
*/
v4sf log_ps(v4sf x) {
#ifdef USE_SSE2
  v4si emm0;
#else
  v2si mm0, mm1;
#endif
  v4sf one = *(v4sf*)_ps_1;

  v4sf invalid_mask = _mm_cmple_ps(x, _mm_setzero_ps());

  x = _mm_max_ps(x, *(v4sf*)_ps_min_norm_pos); /* cut off denormalized stuff */

#ifndef USE_SSE2
  /* part 1: x = frexpf(x, &e); */
  COPY_XMM_TO_MM(x, mm0, mm1);
  mm0 = _mm_srli_pi32(mm0, 23);
  mm1 = _mm_srli_pi32(mm1, 23);
#else
  emm0 = _mm_srli_epi32(_mm_castps_si128(x), 23);
#endif
  /* keep only the fractional part */
  x = _mm_and_ps(x, *(v4sf*)_ps_inv_mant_mask);
  x = _mm_or_ps(x, *(v4sf*)_ps_0p5);

#ifndef USE_SSE2
  /* now e=mm0:mm1 contain the really base-2 exponent */
  mm0 = _mm_sub_pi32(mm0, *(v2si*)_pi32_0x7f);
  mm1 = _mm_sub_pi32(mm1, *(v2si*)_pi32_0x7f);
  v4sf e = _mm_cvtpi32x2_ps(mm0, mm1);
  _mm_empty(); /* bye bye mmx */
#else
  emm0 = _mm_sub_epi32(emm0, *(v4si*)_pi32_0x7f);
  v4sf e = _mm_cvtepi32_ps(emm0);
#endif

  e = _mm_add_ps(e, one);

  /* part2:
     if( x < SQRTHF ) {
       e -= 1;
       x = x + x - 1.0;
     } else { x = x - 1.0; }
  */
  v4sf mask = _mm_cmplt_ps(x, *(v4sf*)_ps_cephes_SQRTHF);
  v4sf tmp = _mm_and_ps(x, mask);
  x = _mm_sub_ps(x, one);
  e = _mm_sub_ps(e, _mm_and_ps(one, mask));
  x = _mm_add_ps(x, tmp);

  v4sf z = _mm_mul_ps(x, x);

  v4sf y = *(v4sf*)_ps_cephes_log_p0;
  y = _mm_mul_ps(y, x);
  y = _mm_add_ps(y, *(v4sf*)_ps_cephes_log_p1);
  y = _mm_mul_ps(y, x);
  y = _mm_add_ps(y, *(v4sf*)_ps_cephes_log_p2);
  y = _mm_mul_ps(y, x);
  y = _mm_add_ps(y, *(v4sf*)_ps_cephes_log_p3);
  y = _mm_mul_ps(y, x);
  y = _mm_add_ps(y, *(v4sf*)_ps_cephes_log_p4);
  y = _mm_mul_ps(y, x);
  y = _mm_add_ps(y, *(v4sf*)_ps_cephes_log_p5);
  y = _mm_mul_ps(y, x);
  y = _mm_add_ps(y, *(v4sf*)_ps_cephes_log_p6);
  y = _mm_mul_ps(y, x);
  y = _mm_add_ps(y, *(v4sf*)_ps_cephes_log_p7);
  y = _mm_mul_ps(y, x);
  y = _mm_add_ps(y, *(v4sf*)_ps_cephes_log_p8);
  y = _mm_mul_ps(y, x);

  y = _mm_mul_ps(y, z);

  tmp = _mm_mul_ps(e, *(v4sf*)_ps_cephes_log_q1);
  y = _mm_add_ps(y, tmp);

  tmp = _mm_mul_ps(z, *(v4sf*)_ps_0p5);
  y = _mm_sub_ps(y, tmp);

  tmp = _mm_mul_ps(e, *(v4sf*)_ps_cephes_log_q2);
  x = _mm_add_ps(x, y);
  x = _mm_add_ps(x, tmp);
  x = _mm_or_ps(x, invalid_mask);  // negative arg will be NAN
  return x;
}

_PS_CONST(exp_hi, 88.3762626647949f);
_PS_CONST(exp_lo, -88.3762626647949f);

_PS_CONST(cephes_LOG2EF, 1.44269504088896341);
_PS_CONST(cephes_exp_C1, 0.693359375);
_PS_CONST(cephes_exp_C2, -2.12194440e-4);

_PS_CONST(cephes_exp_p0, 1.9875691500E-4);
_PS_CONST(cephes_exp_p1, 1.3981999507E-3);
_PS_CONST(cephes_exp_p2, 8.3334519073E-3);
_PS_CONST(cephes_exp_p3, 4.1665795894E-2);
_PS_CONST(cephes_exp_p4, 1.6666665459E-1);
_PS_CONST(cephes_exp_p5, 5.0000001201E-1);

v4sf exp_ps(v4sf x) {
  v4sf tmp = _mm_setzero_ps(), fx;
#ifdef USE_SSE2
  v4si emm0;
#else
  v2si mm0, mm1;
#endif
  v4sf one = *(v4sf*)_ps_1;

  x = _mm_min_ps(x, *(v4sf*)_ps_exp_hi);
  x = _mm_max_ps(x, *(v4sf*)_ps_exp_lo);

  /* express exp(x) as exp(g + n*log(2)) */
  fx = _mm_mul_ps(x, *(v4sf*)_ps_cephes_LOG2EF);
  fx = _mm_add_ps(fx, *(v4sf*)_ps_0p5);

  /* how to perform a floorf with SSE: just below */
#ifndef USE_SSE2
  /* step 1 : cast to int */
  tmp = _mm_movehl_ps(tmp, fx);
  mm0 = _mm_cvttps_pi32(fx);
  mm1 = _mm_cvttps_pi32(tmp);
  /* step 2 : cast back to float */
  tmp = _mm_cvtpi32x2_ps(mm0, mm1);
#else
  emm0 = _mm_cvttps_epi32(fx);
  tmp = _mm_cvtepi32_ps(emm0);
#endif
  /* if greater, substract 1 */
  v4sf mask = _mm_cmpgt_ps(tmp, fx);
  mask = _mm_and_ps(mask, one);
  fx = _mm_sub_ps(tmp, mask);

  tmp = _mm_mul_ps(fx, *(v4sf*)_ps_cephes_exp_C1);
  v4sf z = _mm_mul_ps(fx, *(v4sf*)_ps_cephes_exp_C2);
  x = _mm_sub_ps(x, tmp);
  x = _mm_sub_ps(x, z);

  z = _mm_mul_ps(x, x);

  v4sf y = *(v4sf*)_ps_cephes_exp_p0;
  y = _mm_mul_ps(y, x);
  y = _mm_add_ps(y, *(v4sf*)_ps_cephes_exp_p1);
  y = _mm_mul_ps(y, x);
  y = _mm_add_ps(y, *(v4sf*)_ps_cephes_exp_p2);
  y = _mm_mul_ps(y, x);
  y = _mm_add_ps(y, *(v4sf*)_ps_cephes_exp_p3);
  y = _mm_mul_ps(y, x);
  y = _mm_add_ps(y, *(v4sf*)_ps_cephes_exp_p4);
  y = _mm_mul_ps(y, x);
  y = _mm_add_ps(y, *(v4sf*)_ps_cephes_exp_p5);
  y = _mm_mul_ps(y, z);
  y = _mm_add_ps(y, x);
  y = _mm_add_ps(y, one);

  /* build 2^n */
#ifndef USE_SSE2
  z = _mm_movehl_ps(z, fx);
  mm0 = _mm_cvttps_pi32(fx);
  mm1 = _mm_cvttps_pi32(z);
  mm0 = _mm_add_pi32(mm0, *(v2si*)_pi32_0x7f);
  mm1 = _mm_add_pi32(mm1, *(v2si*)_pi32_0x7f);
  mm0 = _mm_slli_pi32(mm0, 23);
  mm1 = _mm_slli_pi32(mm1, 23);

  v4sf pow2n;
  COPY_MM_TO_XMM(mm0, mm1, pow2n);
  _mm_empty();
#else
  emm0 = _mm_cvttps_epi32(fx);
  emm0 = _mm_add_epi32(emm0, *(v4si*)_pi32_0x7f);
  emm0 = _mm_slli_epi32(emm0, 23);
  v4sf pow2n = _mm_castsi128_ps(emm0);
#endif
  y = _mm_mul_ps(y, pow2n);
  return y;
}

_PS_CONST(minus_cephes_DP1, -0.78515625);
_PS_CONST(minus_cephes_DP2, -2.4187564849853515625e-4);
_PS_CONST(minus_cephes_DP3, -3.77489497744594108e-8);
_PS_CONST(sincof_p0, -1.9515295891E-4);
_PS_CONST(sincof_p1, 8.3321608736E-3);
_PS_CONST(sincof_p2, -1.6666654611E-1);
_PS_CONST(coscof_p0, 2.443315711809948E-005);
_PS_CONST(coscof_p1, -1.388731625493765E-003);
_PS_CONST(coscof_p2, 4.166664568298827E-002);
_PS_CONST(cephes_FOPI, 1.27323954473516);  // 4 / M_PI

/* evaluation of 4 sines at onces, using only SSE1+MMX intrinsics so
   it runs also on old athlons XPs and the pentium III of your grand
   mother.

   The code is the exact rewriting of the cephes sinf function.
   Precision is excellent as long as x < 8192 (I did not bother to
   take into account the special handling they have for greater values
   -- it does not return garbage for arguments over 8192, though, but
   the extra precision is missing).

   Note that it is such that sinf((float)M_PI) = 8.74e-8, which is the
   surprising but correct result.

   Performance is also surprisingly good, 1.33 times faster than the
   macos vsinf SSE2 function, and 1.5 times faster than the
   __vrs4_sinf of amd's ACML (which is only available in 64 bits). Not
   too bad for an SSE1 function (with no special tuning) !
   However the latter libraries probably have a much better handling of NaN,
   Inf, denormalized and other special arguments..

   On my core 1 duo, the execution of this function takes approximately 95
   cycles.

   From what I have observed on the experiments with Intel AMath lib, switching
   to an SSE2 version would improve the perf by only 10%.

   Since it is based on SSE intrinsics, it has to be compiled at -O2 to
   deliver full speed.
*/
v4sf sin_ps(v4sf x) {  // any x
  v4sf xmm1, xmm2 = _mm_setzero_ps(), xmm3, sign_bit, y;

#ifdef USE_SSE2
  v4si emm0, emm2;
#else
  v2si mm0, mm1, mm2, mm3;
#endif
  sign_bit = x;
  /* take the absolute value */
  x = _mm_and_ps(x, *(v4sf*)_ps_inv_sign_mask);
  /* extract the sign bit (upper one) */
  sign_bit = _mm_and_ps(sign_bit, *(v4sf*)_ps_sign_mask);

  /* scale by 4/Pi */
  y = _mm_mul_ps(x, *(v4sf*)_ps_cephes_FOPI);

#ifdef USE_SSE2
  /* store the integer part of y in mm0 */
  emm2 = _mm_cvttps_epi32(y);
  /* j=(j+1) & (~1) (see the cephes sources) */
  emm2 = _mm_add_epi32(emm2, *(v4si*)_pi32_1);
  emm2 = _mm_and_si128(emm2, *(v4si*)_pi32_inv1);
  y = _mm_cvtepi32_ps(emm2);

  /* get the swap sign flag */
  emm0 = _mm_and_si128(emm2, *(v4si*)_pi32_4);
  emm0 = _mm_slli_epi32(emm0, 29);
  /* get the polynom selection mask
     there is one polynom for 0 <= x <= Pi/4
     and another one for Pi/4<x<=Pi/2

     Both branches will be computed.
  */
  emm2 = _mm_and_si128(emm2, *(v4si*)_pi32_2);
  emm2 = _mm_cmpeq_epi32(emm2, _mm_setzero_si128());

  v4sf swap_sign_bit = _mm_castsi128_ps(emm0);
  v4sf poly_mask = _mm_castsi128_ps(emm2);
  sign_bit = _mm_xor_ps(sign_bit, swap_sign_bit);

#else
  /* store the integer part of y in mm0:mm1 */
  xmm2 = _mm_movehl_ps(xmm2, y);
  mm2 = _mm_cvttps_pi32(y);
  mm3 = _mm_cvttps_pi32(xmm2);
  /* j=(j+1) & (~1) (see the cephes sources) */
  mm2 = _mm_add_pi32(mm2, *(v2si*)_pi32_1);
  mm3 = _mm_add_pi32(mm3, *(v2si*)_pi32_1);
  mm2 = _mm_and_si64(mm2, *(v2si*)_pi32_inv1);
  mm3 = _mm_and_si64(mm3, *(v2si*)_pi32_inv1);
  y = _mm_cvtpi32x2_ps(mm2, mm3);
  /* get the swap sign flag */
  mm0 = _mm_and_si64(mm2, *(v2si*)_pi32_4);
  mm1 = _mm_and_si64(mm3, *(v2si*)_pi32_4);
  mm0 = _mm_slli_pi32(mm0, 29);
  mm1 = _mm_slli_pi32(mm1, 29);
  /* get the polynom selection mask */
  mm2 = _mm_and_si64(mm2, *(v2si*)_pi32_2);
  mm3 = _mm_and_si64(mm3, *(v2si*)_pi32_2);
  mm2 = _mm_cmpeq_pi32(mm2, _mm_setzero_si64());
  mm3 = _mm_cmpeq_pi32(mm3, _mm_setzero_si64());
  v4sf swap_sign_bit, poly_mask;
  COPY_MM_TO_XMM(mm0, mm1, swap_sign_bit);
  COPY_MM_TO_XMM(mm2, mm3, poly_mask);
  sign_bit = _mm_xor_ps(sign_bit, swap_sign_bit);
  _mm_empty(); /* good-bye mmx */
#endif

  /* The magic pass: "Extended precision modular arithmetic"
     x = ((x - y * DP1) - y * DP2) - y * DP3; */
  xmm1 = *(v4sf*)_ps_minus_cephes_DP1;
  xmm2 = *(v4sf*)_ps_minus_cephes_DP2;
  xmm3 = *(v4sf*)_ps_minus_cephes_DP3;
  xmm1 = _mm_mul_ps(y, xmm1);
  xmm2 = _mm_mul_ps(y, xmm2);
  xmm3 = _mm_mul_ps(y, xmm3);
  x = _mm_add_ps(x, xmm1);
  x = _mm_add_ps(x, xmm2);
  x = _mm_add_ps(x, xmm3);

  /* Evaluate the first polynom  (0 <= x <= Pi/4) */
  y = *(v4sf*)_ps_coscof_p0;
  v4sf z = _mm_mul_ps(x, x);

  y = _mm_mul_ps(y, z);
  y = _mm_add_ps(y, *(v4sf*)_ps_coscof_p1);
  y = _mm_mul_ps(y, z);
  y = _mm_add_ps(y, *(v4sf*)_ps_coscof_p2);
  y = _mm_mul_ps(y, z);
  y = _mm_mul_ps(y, z);
  v4sf tmp = _mm_mul_ps(z, *(v4sf*)_ps_0p5);
  y = _mm_sub_ps(y, tmp);
  y = _mm_add_ps(y, *(v4sf*)_ps_1);

  /* Evaluate the second polynom  (Pi/4 <= x <= 0) */

  v4sf y2 = *(v4sf*)_ps_sincof_p0;
  y2 = _mm_mul_ps(y2, z);
  y2 = _mm_add_ps(y2, *(v4sf*)_ps_sincof_p1);
  y2 = _mm_mul_ps(y2, z);
  y2 = _mm_add_ps(y2, *(v4sf*)_ps_sincof_p2);
  y2 = _mm_mul_ps(y2, z);
  y2 = _mm_mul_ps(y2, x);
  y2 = _mm_add_ps(y2, x);

  /* select the correct result from the two polynoms */
  xmm3 = poly_mask;
  y2 = _mm_and_ps(xmm3, y2);  //, xmm3);
  y = _mm_andnot_ps(xmm3, y);
  y = _mm_add_ps(y, y2);
  /* update the sign */
  y = _mm_xor_ps(y, sign_bit);
  return y;
}

/* almost the same as sin_ps */
v4sf cos_ps(v4sf x) {  // any x
  v4sf xmm1, xmm2 = _mm_setzero_ps(), xmm3, y;
#ifdef USE_SSE2
  v4si emm0, emm2;
#else
  v2si mm0, mm1, mm2, mm3;
#endif
  /* take the absolute value */
  x = _mm_and_ps(x, *(v4sf*)_ps_inv_sign_mask);

  /* scale by 4/Pi */
  y = _mm_mul_ps(x, *(v4sf*)_ps_cephes_FOPI);

#ifdef USE_SSE2
  /* store the integer part of y in mm0 */
  emm2 = _mm_cvttps_epi32(y);
  /* j=(j+1) & (~1) (see the cephes sources) */
  emm2 = _mm_add_epi32(emm2, *(v4si*)_pi32_1);
  emm2 = _mm_and_si128(emm2, *(v4si*)_pi32_inv1);
  y = _mm_cvtepi32_ps(emm2);

  emm2 = _mm_sub_epi32(emm2, *(v4si*)_pi32_2);

  /* get the swap sign flag */
  emm0 = _mm_andnot_si128(emm2, *(v4si*)_pi32_4);
  emm0 = _mm_slli_epi32(emm0, 29);
  /* get the polynom selection mask */
  emm2 = _mm_and_si128(emm2, *(v4si*)_pi32_2);
  emm2 = _mm_cmpeq_epi32(emm2, _mm_setzero_si128());

  v4sf sign_bit = _mm_castsi128_ps(emm0);
  v4sf poly_mask = _mm_castsi128_ps(emm2);
#else
  /* store the integer part of y in mm0:mm1 */
  xmm2 = _mm_movehl_ps(xmm2, y);
  mm2 = _mm_cvttps_pi32(y);
  mm3 = _mm_cvttps_pi32(xmm2);

  /* j=(j+1) & (~1) (see the cephes sources) */
  mm2 = _mm_add_pi32(mm2, *(v2si*)_pi32_1);
  mm3 = _mm_add_pi32(mm3, *(v2si*)_pi32_1);
  mm2 = _mm_and_si64(mm2, *(v2si*)_pi32_inv1);
  mm3 = _mm_and_si64(mm3, *(v2si*)_pi32_inv1);

  y = _mm_cvtpi32x2_ps(mm2, mm3);

  mm2 = _mm_sub_pi32(mm2, *(v2si*)_pi32_2);
  mm3 = _mm_sub_pi32(mm3, *(v2si*)_pi32_2);

  /* get the swap sign flag in mm0:mm1 and the
     polynom selection mask in mm2:mm3 */

  mm0 = _mm_andnot_si64(mm2, *(v2si*)_pi32_4);
  mm1 = _mm_andnot_si64(mm3, *(v2si*)_pi32_4);
  mm0 = _mm_slli_pi32(mm0, 29);
  mm1 = _mm_slli_pi32(mm1, 29);

  mm2 = _mm_and_si64(mm2, *(v2si*)_pi32_2);
  mm3 = _mm_and_si64(mm3, *(v2si*)_pi32_2);

  mm2 = _mm_cmpeq_pi32(mm2, _mm_setzero_si64());
  mm3 = _mm_cmpeq_pi32(mm3, _mm_setzero_si64());

  v4sf sign_bit, poly_mask;
  COPY_MM_TO_XMM(mm0, mm1, sign_bit);
  COPY_MM_TO_XMM(mm2, mm3, poly_mask);
  _mm_empty(); /* good-bye mmx */
#endif
  /* The magic pass: "Extended precision modular arithmetic"
     x = ((x - y * DP1) - y * DP2) - y * DP3; */
  xmm1 = *(v4sf*)_ps_minus_cephes_DP1;
  xmm2 = *(v4sf*)_ps_minus_cephes_DP2;
  xmm3 = *(v4sf*)_ps_minus_cephes_DP3;
  xmm1 = _mm_mul_ps(y, xmm1);
  xmm2 = _mm_mul_ps(y, xmm2);
  xmm3 = _mm_mul_ps(y, xmm3);
  x = _mm_add_ps(x, xmm1);
  x = _mm_add_ps(x, xmm2);
  x = _mm_add_ps(x, xmm3);

  /* Evaluate the first polynom  (0 <= x <= Pi/4) */
  y = *(v4sf*)_ps_coscof_p0;
  v4sf z = _mm_mul_ps(x, x);

  y = _mm_mul_ps(y, z);
  y = _mm_add_ps(y, *(v4sf*)_ps_coscof_p1);
  y = _mm_mul_ps(y, z);
  y = _mm_add_ps(y, *(v4sf*)_ps_coscof_p2);
  y = _mm_mul_ps(y, z);
  y = _mm_mul_ps(y, z);
  v4sf tmp = _mm_mul_ps(z, *(v4sf*)_ps_0p5);
  y = _mm_sub_ps(y, tmp);
  y = _mm_add_ps(y, *(v4sf*)_ps_1);

  /* Evaluate the second polynom  (Pi/4 <= x <= 0) */

  v4sf y2 = *(v4sf*)_ps_sincof_p0;
  y2 = _mm_mul_ps(y2, z);
  y2 = _mm_add_ps(y2, *(v4sf*)_ps_sincof_p1);
  y2 = _mm_mul_ps(y2, z);
  y2 = _mm_add_ps(y2, *(v4sf*)_ps_sincof_p2);
  y2 = _mm_mul_ps(y2, z);
  y2 = _mm_mul_ps(y2, x);
  y2 = _mm_add_ps(y2, x);

  /* select the correct result from the two polynoms */
  xmm3 = poly_mask;
  y2 = _mm_and_ps(xmm3, y2);  //, xmm3);
  y = _mm_andnot_ps(xmm3, y);
  y = _mm_add_ps(y, y2);
  /* update the sign */
  y = _mm_xor_ps(y, sign_bit);

  return y;
}

/* since sin_ps and cos_ps are almost identical, sincos_ps could replace both of
   them.. it is almost as fast, and gives you a free cosine with your sine */
void sincos_ps(v4sf x, v4sf* s, v4sf* c) {
  v4sf xmm1, xmm2, xmm3 = _mm_setzero_ps(), sign_bit_sin, y;
#ifdef USE_SSE2
  v4si emm0, emm2, emm4;
#else
  v2si mm0, mm1, mm2, mm3, mm4, mm5;
#endif
  sign_bit_sin = x;
  /* take the absolute value */
  x = _mm_and_ps(x, *(v4sf*)_ps_inv_sign_mask);
  /* extract the sign bit (upper one) */
  sign_bit_sin = _mm_and_ps(sign_bit_sin, *(v4sf*)_ps_sign_mask);

  /* scale by 4/Pi */
  y = _mm_mul_ps(x, *(v4sf*)_ps_cephes_FOPI);

#ifdef USE_SSE2
  /* store the integer part of y in emm2 */
  emm2 = _mm_cvttps_epi32(y);

  /* j=(j+1) & (~1) (see the cephes sources) */
  emm2 = _mm_add_epi32(emm2, *(v4si*)_pi32_1);
  emm2 = _mm_and_si128(emm2, *(v4si*)_pi32_inv1);
  y = _mm_cvtepi32_ps(emm2);

  emm4 = emm2;

  /* get the swap sign flag for the sine */
  emm0 = _mm_and_si128(emm2, *(v4si*)_pi32_4);
  emm0 = _mm_slli_epi32(emm0, 29);
  v4sf swap_sign_bit_sin = _mm_castsi128_ps(emm0);

  /* get the polynom selection mask for the sine*/
  emm2 = _mm_and_si128(emm2, *(v4si*)_pi32_2);
  emm2 = _mm_cmpeq_epi32(emm2, _mm_setzero_si128());
  v4sf poly_mask = _mm_castsi128_ps(emm2);
#else
  /* store the integer part of y in mm2:mm3 */
  xmm3 = _mm_movehl_ps(xmm3, y);
  mm2 = _mm_cvttps_pi32(y);
  mm3 = _mm_cvttps_pi32(xmm3);

  /* j=(j+1) & (~1) (see the cephes sources) */
  mm2 = _mm_add_pi32(mm2, *(v2si*)_pi32_1);
  mm3 = _mm_add_pi32(mm3, *(v2si*)_pi32_1);
  mm2 = _mm_and_si64(mm2, *(v2si*)_pi32_inv1);
  mm3 = _mm_and_si64(mm3, *(v2si*)_pi32_inv1);

  y = _mm_cvtpi32x2_ps(mm2, mm3);

  mm4 = mm2;
  mm5 = mm3;

  /* get the swap sign flag for the sine */
  mm0 = _mm_and_si64(mm2, *(v2si*)_pi32_4);
  mm1 = _mm_and_si64(mm3, *(v2si*)_pi32_4);
  mm0 = _mm_slli_pi32(mm0, 29);
  mm1 = _mm_slli_pi32(mm1, 29);
  v4sf swap_sign_bit_sin;
  COPY_MM_TO_XMM(mm0, mm1, swap_sign_bit_sin);

  /* get the polynom selection mask for the sine */

  mm2 = _mm_and_si64(mm2, *(v2si*)_pi32_2);
  mm3 = _mm_and_si64(mm3, *(v2si*)_pi32_2);
  mm2 = _mm_cmpeq_pi32(mm2, _mm_setzero_si64());
  mm3 = _mm_cmpeq_pi32(mm3, _mm_setzero_si64());
  v4sf poly_mask;
  COPY_MM_TO_XMM(mm2, mm3, poly_mask);
#endif

  /* The magic pass: "Extended precision modular arithmetic"
     x = ((x - y * DP1) - y * DP2) - y * DP3; */
  xmm1 = *(v4sf*)_ps_minus_cephes_DP1;
  xmm2 = *(v4sf*)_ps_minus_cephes_DP2;
  xmm3 = *(v4sf*)_ps_minus_cephes_DP3;
  xmm1 = _mm_mul_ps(y, xmm1);
  xmm2 = _mm_mul_ps(y, xmm2);
  xmm3 = _mm_mul_ps(y, xmm3);
  x = _mm_add_ps(x, xmm1);
  x = _mm_add_ps(x, xmm2);
  x = _mm_add_ps(x, xmm3);

#ifdef USE_SSE2
  emm4 = _mm_sub_epi32(emm4, *(v4si*)_pi32_2);
  emm4 = _mm_andnot_si128(emm4, *(v4si*)_pi32_4);
  emm4 = _mm_slli_epi32(emm4, 29);
  v4sf sign_bit_cos = _mm_castsi128_ps(emm4);
#else
  /* get the sign flag for the cosine */
  mm4 = _mm_sub_pi32(mm4, *(v2si*)_pi32_2);
  mm5 = _mm_sub_pi32(mm5, *(v2si*)_pi32_2);
  mm4 = _mm_andnot_si64(mm4, *(v2si*)_pi32_4);
  mm5 = _mm_andnot_si64(mm5, *(v2si*)_pi32_4);
  mm4 = _mm_slli_pi32(mm4, 29);
  mm5 = _mm_slli_pi32(mm5, 29);
  v4sf sign_bit_cos;
  COPY_MM_TO_XMM(mm4, mm5, sign_bit_cos);
  _mm_empty(); /* good-bye mmx */
#endif

  sign_bit_sin = _mm_xor_ps(sign_bit_sin, swap_sign_bit_sin);

  /* Evaluate the first polynom  (0 <= x <= Pi/4) */
  v4sf z = _mm_mul_ps(x, x);
  y = *(v4sf*)_ps_coscof_p0;

  y = _mm_mul_ps(y, z);
  y = _mm_add_ps(y, *(v4sf*)_ps_coscof_p1);
  y = _mm_mul_ps(y, z);
  y = _mm_add_ps(y, *(v4sf*)_ps_coscof_p2);
  y = _mm_mul_ps(y, z);
  y = _mm_mul_ps(y, z);
  v4sf tmp = _mm_mul_ps(z, *(v4sf*)_ps_0p5);
  y = _mm_sub_ps(y, tmp);
  y = _mm_add_ps(y, *(v4sf*)_ps_1);

  /* Evaluate the second polynom  (Pi/4 <= x <= 0) */

  v4sf y2 = *(v4sf*)_ps_sincof_p0;
  y2 = _mm_mul_ps(y2, z);
  y2 = _mm_add_ps(y2, *(v4sf*)_ps_sincof_p1);
  y2 = _mm_mul_ps(y2, z);
  y2 = _mm_add_ps(y2, *(v4sf*)_ps_sincof_p2);
  y2 = _mm_mul_ps(y2, z);
  y2 = _mm_mul_ps(y2, x);
  y2 = _mm_add_ps(y2, x);

  /* select the correct result from the two polynoms */
  xmm3 = poly_mask;
  v4sf ysin2 = _mm_and_ps(xmm3, y2);
  v4sf ysin1 = _mm_andnot_ps(xmm3, y);
  y2 = _mm_sub_ps(y2, ysin2);
  y = _mm_sub_ps(y, ysin1);

  xmm1 = _mm_add_ps(ysin1, ysin2);
  xmm2 = _mm_add_ps(y, y2);

  /* update the sign */
  *s = _mm_xor_ps(xmm1, sign_bit_sin);
  *c = _mm_xor_ps(xmm2, sign_bit_cos);
}

// #pragma optimize("", on)

#endif  // MATH_H