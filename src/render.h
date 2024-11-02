#ifndef RENDER_H
#define RENDER_H

#include "core.h"

typedef struct render_settings_t {
  int render_w, render_h;
  int msaa_level;
  int rendertarget_precision;

  // int sun_shadowmap_resolution;
  // bool use_dynamic_sun_shadows;

  u32 version;
} render_settings_t;

#define RENDER_COMMAND_ALIGN 16

#define MAX_IMMEDIATE_INDICES (1 << 24)
#define MAX_IMMEDIATE_VERTICES (1 << 23)

typedef struct bitmap_font_t {
  unsigned w, h, cw, ch;
  resource_handle_t texture;
} bitmap_font_t;

extern v3 g_debug_colors[6];

static inline v3 r_debug_color(size_t i) {
  return g_debug_colors[i % ARRAY_COUNT(g_debug_colors)];
}

typedef enum pixel_format_t {
  PIXEL_FORMAT_R8,
  PIXEL_FORMAT_RG8,
  PIXEL_FORMAT_RGBA8,
  PIXEL_FORMAT_SRGB8_A8,
  PIXEL_FORMAT_R11G11B10F,
  PIXEL_FORMAT_R32G32B32F,
  PIXEL_FORMAT_R32G32B32A32F,
} pixel_format_t;

typedef enum texture_flags_t {
  TEXTURE_FLAG_CUBEMAP = 0x1,
} texture_flags_t;

typedef enum texture_type_t {
  TEXTURE_TYPE_2D,
  TEXTURE_TYPE_3D,
} texture_type_t;

typedef struct texture_desc_t {
  texture_type_t type;
  pixel_format_t format;
  u32 w, h, d;
  u32 flags;
} texture_desc_t;

typedef struct texture_data_t {
  u32 pitch, slice_pitch;
  union {
    void* pixels;
    void* faces[6];
  };
} texture_data_t;

typedef enum upload_texture_flags_t {
  UPLOAD_TEXTURE_GEN_MIPMAPS = (1 << 0),
} upload_texture_flags_t;

typedef struct upload_texture_t {
  u32 upload_flags;
  texture_desc_t desc;
  texture_data_t data;
} upload_texture_t;

typedef enum vertex_format_t {
  VERTEX_FORMAT_POS,
  VERTEX_FORMAT_IMMEDIATE,
  // VERTEX_FORMAT_BRUSH,
  VERTEX_FORMAT_COUNT,
} vertex_format_t;

typedef enum r_blend_mode_t {
  R_BLEND_PREMUL_ALPHA,
  R_BLEND_ADDITIVE,
} r_blend_mode_t;

typedef enum r_primitive_topology_t {
  R_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
  R_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
  R_PRIMITIVE_TOPOLOGY_LINELIST,
  R_PRIMITIVE_TOPOLOGY_LINESTRIP,
  R_PRIMITIVE_TOPOLOGY_POINTLIST,
} r_primitive_topology_t;

extern u32 vertex_format_size[VERTEX_FORMAT_COUNT];

typedef struct vertex_pos_t {
  v3 pos;
} vertex_pos_t;

typedef struct vertex_immediate_t {
  v2 pos;
  v2 tex;
  u32 col;
} vertex_immediate_t;

// typedef struct vertex_brush_t {
//   v3 pos;
//   v2 tex;
//   v2 tex_lightmap;
//   v3 normal;
// } vertex_brush_t;

// typedef struct upload_model_t {
//   r_primitive_topology_t topology;

//   vertex_format_t vertex_format;
//   u32 vertex_count;
//   const void* vertices;

//   u32 index_count;
//   const u16* indices;
// } upload_model_t;

enum { MISSING_TEXTURE_SIZE = 64 };

typedef struct render_api_i render_api_i;
struct render_api_i {
  void (*get_resolution)(int* w, int* h);

  void (*describe_texture)(resource_handle_t handle, texture_desc_t* desc);

  resource_handle_t (*reserve_texture)(void);
  void (*populate_texture)(resource_handle_t handle,
                           const upload_texture_t* params);

  resource_handle_t (*upload_texture)(const upload_texture_t* params);
  void (*destroy_texture)(resource_handle_t texture);

  // resource_handle_t (*upload_model)(const upload_model_t* params);
  // void (*destroy_model)(resource_handle_t model);
};

void equip_render_api(render_api_i* api);
extern const render_api_i* const render;

typedef struct r_view_t {
  v2 camera_p;
  rect2 clip_rect;
  mat4 camera, projection;
  // resource_handle_t skybox;
  // resource_handle_t fogmap;
  // v3 fog_offset;
  // v3 fog_dim;
  // v3 sun_color;
  // f32 fog_density;
  // f32 fog_absorption;
  // f32 fog_scattering;
  // f32 fog_phase_k;
} r_view_t;

typedef enum r_command_kind_t {
  R_COMMAND_NONE,
  // R_COMMAND_MODEL,
  R_COMMAND_IMMEDIATE,
  R_COMMAND_END_SCENE_PASS,
  R_COMMAND_COUNT,
} r_command_kind_t;

typedef enum r_render_stage_t {
  R_RENDER_STAGE_SCENE,
  R_RENDER_STAGE_UI,
} r_render_state_t;

typedef enum r_render_flags_t {
  R_RENDER_FLAG_NO_SHADOW = 0x1,
} r_render_flags_t;

typedef struct r_command_base_t {
  unsigned char kind;
  unsigned char view;
  unsigned short flags;
#ifndef NDEBUG
  string_t identifier;
#endif
} r_command_base_t;

// typedef struct r_command_model_t {
//   r_command_base_t base;

//   mat4 transform;

//   resource_handle_t model;
//   resource_handle_t texture;
//   resource_handle_t lightmap;
// } r_command_model_t;

typedef struct r_immediate_params_t {
  r_primitive_topology_t topology;
  r_blend_mode_t blend_mode;

  rect2 clip_rect;
  resource_handle_t texture;

  bool depth_test;
  f32 depth_bias;
} r_immediate_params_t;

typedef struct r_immediate_draw_t {
  r_immediate_params_t params;

  u32 ioffset;
  u32 icount;

  u32 vcount;
  u32 voffset;
} r_immediate_draw_t;

typedef struct r_command_immediate_t {
  r_command_base_t base;
  r_immediate_draw_t draw_call;
} r_command_immediate_t;
// Immediate-mode API

r_immediate_draw_t* r_immediate_draw_begin(const r_immediate_params_t* params);
u32 r_immediate_vertex(r_immediate_draw_t* draw_call,
                       const vertex_immediate_t* vertex);
void r_immediate_index(r_immediate_draw_t* draw_call, u32 index);
void r_immediate_draw_end(r_immediate_draw_t* draw_call);

enum { R_MAX_VIEWS = 32 };
typedef unsigned char r_view_index_t;

typedef struct r_list_t {
  r_immediate_draw_t* immediate_draw_call;

  r_view_index_t view_count;
  r_view_t views[R_MAX_VIEWS];

  r_view_index_t view_stack_at;
  r_view_index_t view_stack[R_MAX_VIEWS];

  size_t command_list_size;
  char* command_list_base;
  char* command_list_at;

  u32 max_immediate_icount;
  u32 immediate_icount;
  u32* immediate_indices;

  u32 max_immediate_vcount;
  u32 immediate_vcount;
  vertex_immediate_t* immediate_vertices;
} r_list_t;

void r_command_identifier(string_t identifier);
// void r_draw_model(mat4 transform,
//                   resource_handle_t model,
//                   resource_handle_t texture,
//                   resource_handle_t lightmap);
void r_end_scene_pass(void);

void r_set_command_list(r_list_t* list);
void r_reset_command_list(void);

void r_default_view(r_view_t* view);
void r_push_view(const r_view_t* view);
void r_push_view_screenspace(void);
void r_push_view_screenspace_clip_rect(rect2 clip_rect);
void r_copy_top_view(r_view_t* view);
r_view_t* r_get_top_view(void);
v3 r_to_view_space(const r_view_t* view, v3 p, f32 w);
void r_pop_view(void);

#endif  // RENDER_H