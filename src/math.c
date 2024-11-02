#include "core.h"

#ifdef NO_STD_LIB
extern inline f32 logf(f32 x);
extern inline f32 expf(f32 x);
extern inline f32 sinf(f32 x);
extern inline f32 cosf(f32 x);
extern inline f32 tanf(f32 x);
extern inline f32 sqrtf(f32 x);
extern inline f32 fmodf(f32 x, f32 y);
extern inline f32 fabsf(f32 x);
#endif

void fatal_math_error(const char* message) {
  FATAL_ERROR(message);
}

static void mat_swap(mat* m, int row_a, int row_b) {
  for (int j = 0; j < m->m; j++) {
    SWAP(f32, M(m, row_a, j), M(m, row_b, j));
  }
}

static void mat_scale(mat* m, int row, f32 scale) {
  for (int j = 0; j < m->m; j++) {
    M(m, row, j) *= scale;
  }
}

static void mat_add(mat* m, int row_a, int row_b, f32 scale) {
  for (int j = 0; j < m->m; j++) {
    M(m, row_b, j) += M(m, row_a, j) * scale;
  }
}

static int find_max_pivot(mat* m, int k) {
  int result = k;
  f32 max = abs_ss(M(m, k, k));

  for (int i = k + 1; i < m->n; i++) {
    f32 val = abs_ss(M(m, i, k));
    if (val > max) {
      result = i;
    }
  }

  return result;
}

static void back_substitute(mat* m, f32* x) {
  zero_array(x, m->n);

  for (int i = m->n - 1; i >= 0; i--) {
    f32 s = 0.0f;
    for (int j = i + 1; j < m->n; j++) {
      s += M(m, i, j) * x[j];
    }
    x[i] = M(m, i, m->n) - s;
  }
}

bool solve_system_of_equations(mat* m, f32* x) {
  if (m->m <= 0 || m->n <= 0) {
    FATAL_ERROR("matrices need rows and columns...");
  }

  if (m->m != m->n + 1) {
    FATAL_ERROR(
        "For Gaussian substitution, expected a m*n matrix where m = n + 1");
  }

  for (int k = 0; k < 3; k++) {
    mat_swap(m, k, find_max_pivot(m, k));

    if (abs_ss(M(m, k, k)) < 0.0001f) {
      return false;
    }
    mat_scale(m, k, 1.0f / M(m, k, k));

    for (int i = k + 1; i < 3; i++) {
      mat_add(m, k, i, -M(m, i, k));
    }
  }

  back_substitute(m, x);

  return true;
}