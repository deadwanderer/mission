#ifndef RANDOM_H
#define RANDOM_H

static inline u32 random_u32(random_series* r) {
  u32 x = r->state;
  x ^= x << 13;
  x ^= x >> 17;
  x ^= x << 5;
  r->state = x;
  return r->state;
}

static inline f32 random_unilateral(random_series* r) {
  u32 exponent = 127;
  u32 mantissa = random_u32(r) >> 9;
  u32 bits = (exponent << 23) | mantissa;
  f32 result = *(f32*)&bits - 1.0f;
  return result;
}

static inline f32 random_bilateral(random_series* r) {
  return -1.0f + 2.0f * random_unilateral(r);
}

static inline u32 random_choice(random_series* r, u32 range) {
  u32 result = random_u32(r) % range;
  return result;
}

static inline u32 dice_roll(random_series* r, u32 sides) {
  u32 result = 1 + random_choice(r, sides);
  return result;
}

static inline s32 random_range_s32(random_series* r, s32 min, s32 max) {
  if (max < min) {
    max = min;
  }

  s32 result = min + (s32)random_u32(r) % (max - min + 1);
  return result;
}

static inline f32 random_range_f32(random_series* r, f32 min, f32 max) {
  f32 range = random_unilateral(r);
  f32 result = min + range * (max - min);
  return result;
}

static inline v2 random_unilateral2(random_series* r) {
  v2 result = {
      random_unilateral(r),
      random_unilateral(r),
  };
  return result;
}

static inline v3 random_unilateral3(random_series* r) {
  v3 result = {
      random_unilateral(r),
      random_unilateral(r),
      random_unilateral(r),
  };
  return result;
}

static inline v2 random_in_unit_square(random_series* r) {
  v2 result = {
      random_bilateral(r),
      random_bilateral(r),
  };
  return result;
}

static inline v3 random_in_unit_cube(random_series* r) {
  v3 result = {
      random_bilateral(r),
      random_bilateral(r),
      random_bilateral(r),
  };
  return result;
}

static inline v2 random_in_unit_disc(random_series* r) {
  v2 result;
  for (;;) {
    result = random_in_unit_square(r);
    if (dot(result, result) < 1.0) {
      break;
    }
  }
  return result;
}

static inline v3 random_in_unit_sphere(random_series* r) {
  v3 result;
  for (;;) {
    result = random_in_unit_cube(r);
    if (dot(result, result) < 1.0) {
      break;
    }
  }
  return result;
}

#endif  // RANDOM_H