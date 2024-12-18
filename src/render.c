#include "render.h"

#define SCREENSPACE_VIEW_INDEX (1)

v3 g_debug_colors[] = {
    {1, 0, 0}, {0, 1, 0}, {0, 0, 1}, {1, 1, 0}, {1, 0, 1}, {0, 1, 1},
};

static render_api_i inst;
const render_api_i* const render = &inst;

void equip_render_api(render_api_i* api) {
  inst = *api;
}

u32 vertex_format_size[VERTEX_FORMAT_COUNT] = {
    [VERTEX_FORMAT_POS] = sizeof(vertex_pos_t),
    [VERTEX_FORMAT_IMMEDIATE] = sizeof(vertex_immediate_t),
    // [VERTEX_FORMAT_BRUSH] = sizeof(vertex_brush_t),
};

static r_list_t* g_list;

void r_set_command_list(r_list_t* new_list) {
  g_list = new_list;
  r_reset_command_list();
}

void r_reset_command_list(void) {
  g_list->immediate_draw_call = NULL;

  g_list->command_list_at = g_list->command_list_base;
  g_list->view_count = 0;
  g_list->view_stack_at = 0;

  g_list->immediate_icount = 0;
  g_list->immediate_vcount = 0;

  r_view_t default_view;
  r_default_view(&default_view);
  r_push_view(&default_view);

  {
    r_view_t view = {0};

    int w, h;
    render->get_resolution(&w, &h);

    view.clip_rect = (rect2){
        .x0 = 0.0f,
        .y0 = 0.0f,
        .x1 = (f32)w,
        .y1 = (f32)h,
    };

    f32 a = 2.0f / (f32)w;
    f32 b = 2.0f / (f32)h;

    view.camera = make_mat4(a, 0, 0, -1, 0, b, 0, -1, 0, 0, 1, 0, 0, 0, 0, 1);
    view.projection = mat4_identity;

    r_push_view(&view);
  }
}

static STRING_STORAGE(64) g_next_command_identifier;

void r_command_identifier(string_t identifier) {
  STRING_INTO_STORAGE(g_next_command_identifier, identifier);
}

static void* r_submit_command(r_command_kind_t kind, size_t command_size) {
  ASSERT(g_list);

  void* result = NULL;

  if (ALWAYS(kind > R_COMMAND_NONE && kind < R_COMMAND_COUNT)) {
    size_t size_used =
        align_forward(g_list->command_list_at - g_list->command_list_base,
                      RENDER_COMMAND_ALIGN);
    size_t size_left = g_list->command_list_size - size_used;
    if (ALWAYS(size_left >= command_size)) {
      char* at = align_address(g_list->command_list_at, RENDER_COMMAND_ALIGN);
      zero_memory(at, command_size);

      r_command_base_t* base = (void*)at;
      base->kind = kind;

      if (ALWAYS(g_list->view_stack_at > 0)) {
        base->view = g_list->view_stack[g_list->view_stack_at - 1];
      } else {
        base->view = 0;
      }

#ifndef NDEBUG
      if (g_next_command_identifier.count > 0) {
        base->identifier =
            string_copy(temp, STRING_FROM_STORAGE(g_next_command_identifier));
      }
      g_next_command_identifier.count = 0;
#endif

      g_list->command_list_at = at + command_size;

      result = at;
    }
  }
  return result;
}

void r_push_view(const r_view_t* view) {
  if (ALWAYS(g_list->view_count < R_MAX_VIEWS)) {
    r_view_index_t index = g_list->view_count++;
    copy_struct(&g_list->views[index], view);

    if (ALWAYS(g_list->view_stack_at < R_MAX_VIEWS)) {
      g_list->view_stack[g_list->view_stack_at++] = index;
    }
  }
}

void r_push_view_screenspace(void) {
  if (ALWAYS(g_list->view_count < R_MAX_VIEWS)) {
    r_view_index_t index = SCREENSPACE_VIEW_INDEX;

    if (ALWAYS(g_list->view_stack_at < R_MAX_VIEWS)) {
      g_list->view_stack[g_list->view_stack_at++] = index;
    }
  }
}

void r_push_view_screenspace_clip_rect(rect2 clip_rect) {
  r_view_t view;
  r_default_view(&view);

  int w, h;
  render->get_resolution(&w, &h);

  f32 a = 2.0f / (f32)w;
  f32 b = 2.0f / (f32)h;

  view.camera = make_mat4(a, 0, 0, -1, 0, b, 0, -1, 0, 0, 1, 0, 0, 0, 0, 1);

  r_view_t* parent = r_get_top_view();

  view.clip_rect = rect2_intersect(clip_rect, parent->clip_rect);
  r_push_view(&view);
}

void r_default_view(r_view_t* view) {
  zero_struct(view);

  int w, h;
  render->get_resolution(&w, &h);

  view->clip_rect = (rect2){
      .x0 = 0.0f,
      .y0 = 0.0f,
      .x1 = (f32)w,
      .y1 = (f32)h,
  };

  view->camera = mat4_identity;
  view->projection = mat4_identity;
}

static thread_local r_view_t g_null_view;

r_view_t* r_get_top_view(void) {
  r_view_t* result = NULL;

  if (ALWAYS(g_list->view_stack_at > 0)) {
    result = &g_list->views[g_list->view_stack[g_list->view_stack_at - 1]];
  } else {
    result = &g_null_view;
    r_default_view(result);
  }
  return result;
}

void r_copy_top_view(r_view_t* view) {
  if (ALWAYS(g_list->view_stack_at > 0)) {
    *view = g_list->views[g_list->view_stack[g_list->view_stack_at - 1]];
  } else {
    r_default_view(view);
  }
}

void r_pop_view(void) {
  if (ALWAYS(g_list->view_stack_at > 1)) {
    g_list->view_stack_at--;
  }
}

v3 r_to_view_space(const r_view_t* view, v3 p, f32 w) {
  v4 pw = {p.x, p.y, p.z, w};

  pw = mul(view->camera, pw);
  pw = mul(view->projection, pw);
  pw.xyz = div(pw.xyz, pw.w);

  int width, height;
  render->get_resolution(&width, &height);

  pw.x += 1.0f;
  pw.y += 1.0f;
  pw.x *= 0.5f * (f32)width;
  pw.y *= 0.5f * (f32)height;

  return pw.xyz;
}

// void r_draw_model(mat4 transform,
//                   resource_handle_t model,
//                   resource_handle_t texture,
//                   resource_handle_t lightmap) {
//   r_command_model_t* command =
//       r_submit_command(R_COMMAND_MODEL, sizeof(r_command_model_t));
//   command->transform = transform;
//   command->model = model;
//   command->texture = texture;
//   command->lightmap = lightmap;
// }

r_immediate_draw_t* r_immediate_draw_begin(const r_immediate_params_t* params) {
  r_command_immediate_t* command =
      r_submit_command(R_COMMAND_IMMEDIATE, sizeof(r_command_immediate_t));

  if (params) {
    copy_struct(&command->draw_call.params, params);
  }

  command->draw_call.ioffset = g_list->immediate_icount;
  command->draw_call.voffset = g_list->immediate_vcount;

  if (command->draw_call.params.clip_rect.min.x == 0 &&
      command->draw_call.params.clip_rect.min.y == 0 &&
      command->draw_call.params.clip_rect.max.x == 0 &&
      command->draw_call.params.clip_rect.max.y == 0) {
    int w, h;
    render->get_resolution(&w, &h);

    command->draw_call.params.clip_rect.max = make_v2((f32)w, (f32)h);
  }

  g_list->immediate_draw_call = &command->draw_call;

  return &command->draw_call;
}

void r_end_scene_pass(void) {
  r_submit_command(R_COMMAND_END_SCENE_PASS, sizeof(r_command_base_t));
}

u32 r_immediate_vertex(r_immediate_draw_t* draw_call,
                       const vertex_immediate_t* vertex) {
  ASSERT(draw_call == g_list->immediate_draw_call);
  u32 result = UINT16_MAX;

  if (ALWAYS(g_list->immediate_vcount < g_list->max_immediate_vcount)) {
    draw_call->vcount += 1;

    result = (u32)g_list->immediate_vcount++;
    g_list->immediate_vertices[result] = *vertex;
  }

  return result;
}

void r_immediate_index(r_immediate_draw_t* draw_call, u32 index) {
  ASSERT(draw_call == g_list->immediate_draw_call);

  if (ALWAYS(g_list->immediate_icount < g_list->max_immediate_icount)) {
    draw_call->icount += 1;
    g_list->immediate_indices[g_list->immediate_icount++] = index;
  }
}

void r_immediate_draw_end(r_immediate_draw_t* draw_call) {
  (void)draw_call;
  ASSERT(draw_call == g_list->immediate_draw_call);

  g_list->immediate_draw_call = NULL;
}
