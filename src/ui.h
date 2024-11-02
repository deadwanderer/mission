#ifndef MISSION_DEBUG_UI_H
#define MISSION_DEBUG_UI_H

#include "api_types.h"
#include "string.h"
#include "assert.h"
#include "math.h"

typedef struct ui_id_t {
  u64 value;
} ui_id_t;

#define UI_ID_NULL \
  (ui_id_t) {      \
    0              \
  }

MISSION_INLINE ui_id_t ui_child_id(ui_id_t parent, string_t string) {
  u64 hash = string_hash_with_seed(string, parent.value);

  if (hash == 0) {
    hash = ~(u64)0;
  }

  ui_id_t result = {
      .value = hash,
  };
  return result;
}

MISSION_INLINE ui_id_t ui_id(string_t string) {
  u64 hash = string_hash(string);

  if (hash == 0) {
    hash = ~(u64)0;
  }

  ui_id_t result = {.value = hash};
  return result;
}

MISSION_INLINE ui_id_t ui_id_pointer(void* pointer) {
  ui_id_t result = {
      .value = (uintptr_t)pointer,
  };
  return result;
}

MISSION_INLINE rect2 ui_cut_left(rect2* rect, f32 a) {
  f32 min_x = rect->x0;
  rect->min.x = rect->x0 + a;
  return (rect2){.min = {min_x, rect->min.y},
                 .max = {rect->min.x, rect->max.y}};
}

MISSION_INLINE rect2 ui_cut_right(rect2* rect, f32 a) {
  f32 max_x = rect->x1;
  rect->max.x = rect->x1 - a;
  return (rect2){.min = {rect->max.x, rect->min.y},
                 .max = {max_x, rect->max.y}};
}

MISSION_INLINE rect2 ui_cut_top(rect2* rect, f32 a) {
  f32 max_y = rect->y1;
  rect->max.y = rect->y1 - a;
  return (rect2){.min = {rect->min.x, rect->max.y},
                 .max = {rect->max.x, max_y}};
}

MISSION_INLINE rect2 ui_cut_bottom(rect2* rect, f32 a) {
  f32 min_y = rect->y0;
  rect->min.y = rect->y0 + a;
  return (rect2){.min = {rect->min.x, min_y},
                 .max = {rect->max.x, rect->min.y}};
}

MISSION_INLINE rect2 ui_add_left(rect2* rect, f32 a) {
  f32 min_x = MIN(rect->x0, rect->x0 - a);
  return (rect2){.min = {min_x, rect->min.y},
                 .max = {rect->min.x, rect->max.y}};
}

MISSION_INLINE rect2 ui_add_right(rect2* rect, f32 a) {
  f32 max_x = MAX(rect->x0, rect->x0 + a);
  return (rect2){.min = {rect->max.x, rect->min.y},
                 .max = {max_x, rect->max.y}};
}

MISSION_INLINE rect2 ui_add_top(rect2* rect, f32 a) {
  f32 max_y = MAX(rect->y1, rect->y1 + a);
  return (rect2){.min = {rect->min.x, rect->max.y},
                 .max = {rect->max.x, max_y}};
}

MISSION_INLINE rect2 ui_add_bottom(rect2* rect, f32 a) {
  f32 min_y = MIN(rect->y0, rect->y0 - a);
  return (rect2){.min = {rect->min.x, min_y},
                 .max = {rect->max.x, rect->min.y}};
}

MISSION_INLINE rect2 ui_extend(rect2* rect, f32 a) {
  rect2 result = {
      .min = sub(rect->min, a),
      .max = add(rect->max, a),
  };
  return result;
}

MISSION_INLINE rect2 ui_shrink(rect2* rect, f32 a) {
  return ui_extend(rect, -a);
}

MISSION_INLINE rect2 ui_extend2(rect2* rect, f32 x, f32 y) {
  v2 xy = {x, y};

  rect2 result = {
      .min = sub(rect->min, xy),
      .max = add(rect->max, xy),
  };
  return result;
}

MISSION_INLINE rect2 ui_shrink2(rect2* rect, f32 x, f32 y) {
  return ui_extend2(rect, -x, -y);
}

typedef enum ui_cut_side_t {
  UI_CUT_LEFT,
  UI_CUT_RIGHT,
  UI_CUT_TOP,
  UI_CUT_BOTTOM,
  UI_CUT_COUNT,
} ui_cut_side_t;

typedef struct ui_cut_t {
  rect2* rect;
  ui_cut_side_t side;
} ui_cut_t;

MISSION_INLINE ui_cut_t ui_cut(rect2* rect, ui_cut_side_t side) {
  return (ui_cut_t){
      .rect = rect,
      .side = side,
  };
}

MISSION_INLINE rect2 ui_do_cut(ui_cut_t cut, f32 a) {
  switch (cut.side) {
    case UI_CUT_LEFT: {
      return ui_cut_left(cut.rect, a);
    } break;
    case UI_CUT_RIGHT: {
      return ui_cut_right(cut.rect, a);
    } break;
    case UI_CUT_TOP: {
      return ui_cut_top(cut.rect, a);
    } break;
    case UI_CUT_BOTTOM: {
      return ui_cut_bottom(cut.rect, a);
    } break;
      INVALID_DEFAULT_CASE;
  }

  return (rect2){0};
}

typedef struct ui_style_t {
  f32 animation_rate;

  f32 widget_margin;
  f32 text_margin;

  f32 scrollbar_width;

  v4 text;

  struct {
    v4 background;
    v4 title_bar;
  } window;

  struct {
    v4 empty;
    v4 filled;
  } progress_bar;

  struct {
    v4 background;
    v4 hot;
    v4 active;
    v4 fired;
  } button;

  struct {
    f32 handle_width;
    v4 background;
    v4 foreground;
    v4 hot;
    v4 active;
  } slider;
} ui_style_t;

typedef u32 ui_panel_flags_t;
typedef enum ui_panel_flags_enum_t {
  UI_PANEL_SCROLLABLE_HORZ = (1 << 0),
  UI_PANEL_SCROLLABLE_VERT = (1 << 1),
} ui_panel_flags_enum_t;

MISSION_API void ui_set_style(const ui_style_t* style);

MISSION_API bool ui_begin(f32 dt);
MISSION_API void ui_end(void);

MISSION_API void ui_window_begin(string_t label, rect2 size);
MISSION_API void ui_window_end(void);

MISSION_API void ui_panel_begin(rect2 size);
MISSION_API void ui_panel_begin_ex(ui_id_t id,
                                   rect2 size,
                                   ui_panel_flags_t flags);
MISSION_API void ui_panel_end(void);

MISSION_API rect2* ui_layout_rect(void);
MISSION_API void ui_set_layout_direction(ui_cut_side_t side);
MISSION_API void ui_set_next_rect(rect2 rect);
MISSION_API f32 ui_divide_space(f32 item_count);

MISSION_API void ui_label(string_t label);
MISSION_API void ui_progress_bar(string_t label, f32 progress);
MISSION_API bool ui_button(string_t label);
MISSION_API bool ui_checkbox(string_t label, bool* value);
MISSION_API void ui_slider(string_t label, f32* v, f32 min, f32 max);

#endif  // MISSION_DEBUG_UI_H