#include "string.h"
#include "arena.h"
#include "math.h"
#include "hashtable.h"

#include "render.h"
#include "render_helpers.h"

#include "input.h"
#include "asset.h"
#include "ui.h"

typedef struct ui_panel_t {
  union {
    struct ui_panel_t* parent;
    struct ui_panel_t* next_free;
  };

  ui_id_t id;
  ui_cut_side_t layout_direction;
  ui_panel_flags_t flags;
  rect2 init_rect;
  rect2 rect;
} ui_panel_t;

typedef struct ui_t {
  bool initialized;

  arena_t arena;

  u64 frame_index;

  f32 dt;

  bool has_focus;
  bool hovered;

  ui_id_t hot;
  ui_id_t active;

  ui_style_t style;

  v2 mouse_p;
  v2 mouse_pressed_p;
  v2 mouse_pressed_offset;

  ui_panel_t* panel;
  ui_panel_t* first_free_panel;

  rect2 next_rect;

  bitmap_font_t font;
} ui_t;

static ui_t ui;

MISSION_INLINE ui_panel_t* ui_push_panel(rect2 rect) {
  if (!ui.first_free_panel) {
    ui.first_free_panel = m_alloc_struct_nozero(&ui.arena, ui_panel_t);
    ui.first_free_panel->next_free = NULL;
  }

  ui_panel_t* panel = ui.first_free_panel;
  ui.first_free_panel = panel->next_free;

  zero_struct(panel);

  panel->layout_direction = UI_CUT_TOP;
  panel->rect = rect;

  panel->parent = ui.panel;
  ui.panel = panel;

  return panel;
}

MISSION_INLINE void ui_pop_panel(void) {
  if (ALWAYS(ui.panel)) {
    ui_panel_t* panel = ui.panel;
    ui.panel = ui.panel->parent;

    panel->next_free = ui.first_free_panel;
    ui.first_free_panel = panel;
  }
}

MISSION_INLINE ui_panel_t* ui_panel(void) {
  static ui_panel_t dummy_panel = {0};

  ui_panel_t* result = &dummy_panel;

  if (ALWAYS(ui.panel)) {
    result = ui.panel;
  }

  return result;
}

rect2* ui_layout_rect(void) {
  ui_panel_t* panel = ui_panel();
  return &panel->rect;
}

void ui_set_layout_direction(ui_cut_side_t side) {
  ui_panel_t* panel = ui_panel();
  panel->layout_direction = side;
}

void ui_set_next_rect(rect2 rect) {
  ui.next_rect = rect;
}

f32 ui_divide_space(f32 item_count) {
  f32 size = 0.0f;
  if (item_count > 0.0001f) {
    ui_panel_t* panel = ui_panel();

    switch (panel->layout_direction) {
      case UI_CUT_LEFT:
      case UI_CUT_RIGHT: {
        size = rect2_width(panel->rect) / item_count;
      } break;
      case UI_CUT_TOP:
      case UI_CUT_BOTTOM: {
        size = rect2_height(panel->rect) / item_count;
      } break;
    }
  }
  return size;
}

typedef struct ui_widget_t {
  ui_id_t id;

  bool new;
  u64 last_touched_frame_index;

  f32 scrollable_height_x;
  f32 scrollable_height_y;
  f32 scroll_offset_x;
  f32 scroll_offset_y;

  v4 interp_color;
} ui_widget_t;

static bulk_t widgets = INIT_BULK_DATA(ui_widget_t);
static hash_t widget_index;

MISSION_INLINE ui_widget_t* ui_get_widget(ui_id_t id) {
  ui_widget_t* result = hash_find_object(&widget_index, id.value);

  if (!result) {
    result = bd_add(&widgets);
    result->id = id;
    result->new = true;
    hash_add_object(&widget_index, id.value, result);
  }

  ASSERT(result->id.value == id.value);

  result->last_touched_frame_index = ui.frame_index;
  return result;
}

MISSION_INLINE bool ui_is_cold(ui_id_t id) {
  return (ui.hot.value != id.value && ui.active.value != id.value);
}

MISSION_INLINE bool ui_is_hot(ui_id_t id) {
  return ui.hot.value == id.value;
}

MISSION_INLINE bool ui_is_active(ui_id_t id) {
  return ui.active.value == id.value;
}

MISSION_INLINE void ui_set_hot(ui_id_t id) {
  if (!ui.active.value) {
    ui.hot = id;
  }
}

MISSION_INLINE void ui_set_active(ui_id_t id) {
  ui.active = id;
}

MISSION_INLINE void ui_clear_hot(void) {
  ui.hot.value = 0;
}

MISSION_INLINE void ui_clear_active(void) {
  ui.active.value = 0;
}

void ui_set_style(const ui_style_t* style) {
  ui.style = *style;
}

MISSION_INLINE f32 ui_widget_padding(void) {
  return 2.0f * ui.style.widget_margin + 2.0f * ui.style.text_margin;
}

MISSION_INLINE bool ui_override_rect(rect2* override) {
  bool result = false;

  if (rect2_area(ui.next_rect) > 0.0f) {
    *override = ui.next_rect;
    ui.next_rect = (rect2){0};

    result = true;
  }
  return result;
}

MISSION_INLINE rect2 ui_default_label_rect(string_t label) {
  rect2 rect;
  if (!ui_override_rect(&rect)) {
    ui_panel_t* panel = ui_panel();

    f32 a = ui_widget_padding();

    switch (panel->layout_direction) {
      case UI_CUT_LEFT:
      case UI_CUT_RIGHT: {
        a += (f32)label.count * ui.font.cw;
      } break;
      case UI_CUT_TOP:
      case UI_CUT_BOTTOM: {
        a += (f32)ui.font.ch;
      } break;
    }

    rect = ui_do_cut(
        (ui_cut_t){.side = panel->layout_direction, .rect = &panel->rect}, a);
  }
  return rect;
}

bool ui_begin(f32 dt) {
  if (!ui.initialized) {
    image_t* font_image =
        get_image_from_string(S("gamedata/textures/font.png"));
    ui.font = (bitmap_font_t){
        .w = font_image->w,
        .h = font_image->h,
        .cw = 10,
        .ch = 12,
        .texture = font_image->gpu,
    };

    ui.initialized = true;

    ui_style_t* style = &ui.style;
    style->animation_rate = 40.0f;
    style->widget_margin = 2.0f;
    style->text_margin = 4.0f;
    style->scrollbar_width = 12.0f;
    style->text = make_v4(0.90f, 0.90f, 0.90f, 1.0f);
    style->window.background = make_v4(0.15f, 0.15f, 0.15f, 1.0f);
    style->window.title_bar = make_v4(0.45f, 0.25f, 0.25f, 1.0f);
    style->progress_bar.empty = make_v4(0.18f, 0.18f, 0.18f, 1.0f);
    style->progress_bar.filled = make_v4(0.15f, 0.28f, 0.45f, 1.0f);
    style->button.background = make_v4(0.28f, 0.28f, 0.28f, 1.0f);
    style->button.hot = make_v4(0.25f, 0.35f, 0.65f, 1.0f);
    style->button.active = make_v4(0.35f, 0.45f, 0.85f, 1.0f);
    style->button.fired = make_v4(0.45f, 0.30f, 0.25f, 1.0f);
    style->slider.handle_width = 32.0f;
    style->slider.background = make_v4(0.18f, 0.18f, 0.18f, 1.0f);
    style->slider.foreground = make_v4(0.28f, 0.28f, 0.28f, 1.0f);
    style->slider.hot = make_v4(0.25f, 0.35f, 0.65f, 1.0f);
    style->slider.active = make_v4(0.35f, 0.45f, 0.85f, 1.0f);
  }

  ui.frame_index += 1;

  for (bd_iter_t it = bd_iter(&widgets); bd_iter_valid(&it);
       bd_iter_next(&it)) {
    ui_widget_t* widget = it.data;

    widget->new = false;

    if (widget->last_touched_frame_index + 1 < ui.frame_index) {
      hash_rem(&widget_index, widget->id.value);
      bd_rem_item(&widgets, widget);
    }
  }

  ui_clear_hot();
  ui.hovered = false;
  ui.dt = dt;

  int mouse_x, mouse_y;
  get_mouse(&mouse_x, &mouse_y);

  ui.mouse_p = (v2){(f32)mouse_x, (f32)mouse_y};

  return ui.has_focus;
}

void ui_end(void) {
  if (ui_button_pressed(BUTTON_FIRE1 | BUTTON_FIRE2)) {
    ui.has_focus = ui.hovered;
  }
  ASSERT(ui.panel == NULL);
}

typedef enum ui_interaction_t {
  UI_PRESSED = (1 << 0),
  UI_HELD = (1 << 1),
  UI_RELEASED = (1 << 2),
  UI_FIRED = (1 << 3),
} ui_interaction_t;

MISSION_INLINE u32 ui_button_behavior(ui_id_t id, rect2 rect) {
  u32 result = 0;

  rect2 hit_rect = rect;

  if (ui.panel) {
    hit_rect = rect2_intersect(ui.panel->init_rect, rect);
  }

  if (ui_is_active(id)) {
    result |= UI_HELD;

    if (ui_button_released(BUTTON_FIRE1)) {
      if (rect2_contains_point(hit_rect, ui.mouse_p)) {
        result |= UI_FIRED;
      }
      result |= UI_RELEASED;

      ui_clear_active();
    }
  } else if (rect2_contains_point(hit_rect, ui.mouse_p)) {
    ui_set_hot(id);
  }

  if (ui_is_hot(id) && ui_button_pressed(BUTTON_FIRE1)) {
    result |= UI_PRESSED;
    ui.mouse_pressed_p = ui.mouse_p;
    ui.mouse_pressed_offset = sub(ui.mouse_p, rect2_center(rect));
    ui_set_active(id);
  }

  return result;
}

MISSION_INLINE void ui_check_hovered(rect2 rect) {
  if (rect2_contains_point(rect, ui.mouse_p)) {
    ui.hovered = true;
  }
}

void ui_window_begin(string_t label, rect2 rect) {
  ASSERT(ui.initialized);

  ui_check_hovered(rect);

  rect2 bar = ui_add_top(&rect, (f32)ui.font.ch + 2.0f * ui.style.text_margin);
  ui_check_hovered(bar);

  {
    r_immediate_draw_t* draw = r_immediate_draw_begin(NULL);
    r_push_rect2_filled(draw, bar, ui.style.window.title_bar);
    r_immediate_draw_end(draw);
  }

  {
    r_immediate_draw_t* draw = r_immediate_draw_begin(
        &(r_immediate_params_t){.texture = ui.font.texture, .clip_rect = bar});
    r_push_text(
        draw, &ui.font,
        add(bar.min, make_v2(ui.style.text_margin, ui.style.text_margin)),
        ui.style.text, label);
    r_immediate_draw_end(draw);
  }

  {
    r_immediate_draw_t* draw = r_immediate_draw_begin(NULL);
    r_push_rect2_filled(draw, rect, ui.style.window.background);
    r_immediate_draw_end(draw);
  }

  ui_id_t id = ui_id(label);
  ui_panel_begin_ex(id, rect, UI_PANEL_SCROLLABLE_VERT);
}

void ui_window_end(void) {
  ASSERT(ui.initialized);
  ui_panel_end();
}

void ui_panel_begin(rect2 rect) {
  ui_panel_begin_ex(UI_ID_NULL, rect, 0);
}

void ui_panel_begin_ex(ui_id_t id, rect2 rect, ui_panel_flags_t flags) {
  ASSERT(ui.initialized);

  r_push_view_screenspace_clip_rect(rect);

  rect2 inner_rect = ui_shrink(&rect, ui.style.widget_margin);

  f32 offset_y = 0.0f;

  if (flags & UI_PANEL_SCROLLABLE_VERT) {
    rect2 scroll_area = ui_cut_right(&rect, ui.style.scrollbar_width);
    ui_widget_t* widget = ui_get_widget(id);

    if (widget->scrollable_height_y > 0.0f) {
      ui_id_t scrollbar_id = ui_child_id(id, S("scrollbar"));

      f32 scroll_area_height = rect2_height(scroll_area);

      f32 visible_area_ratio =
          rect2_height(inner_rect) / widget->scrollable_height_y;
      f32 handle_size = visible_area_ratio * scroll_area_height;
      f32 handle_half_size = 0.5f * handle_size;

      f32 pct = widget->scroll_offset_y /
                (widget->scrollable_height_y - rect2_height(inner_rect));

      if (ui_is_active(scrollbar_id)) {
        f32 relative_y = CLAMP((ui.mouse_p.y - ui.mouse_pressed_offset.y) -
                                   scroll_area.min.y - handle_half_size,
                               0.0f, scroll_area_height - handle_size);
        pct = 1.0f - (relative_y / (scroll_area_height - handle_size));
        widget->scroll_offset_y =
            pct * (widget->scrollable_height_y - rect2_height(inner_rect));
      }

      f32 height_exclusive = scroll_area_height - handle_size;
      f32 handle_offset = pct * height_exclusive;

      rect2 top = ui_cut_top(&scroll_area, handle_offset);
      rect2 handle = ui_cut_top(&scroll_area, handle_size);
      rect2 bottom = scroll_area;

      ui_button_behavior(scrollbar_id, handle);

      v4 color = ui.style.slider.foreground;

#if 0
      if (ui_is_hot(id)) {
        color = ui.style.slider.hot;
      }

      if (ui_is_active(id)) {
        color = ui.style.slider.active;
      }

      if (widget->t >= 0.0f) {
        f32 rate = 0.5f;

        f32 t = widget->t;
        t *= t;
        t *= t;

        color = v4_lerps(color, ui.style.slider.active, t);

        widget->t -= ui.dt / rate;

        if (widget->t < 0.0f) {
          widget->t = 0.0f;
        }
      }
#endif

      {
        r_immediate_draw_t* draw = r_immediate_draw_begin(NULL);
        r_push_rect2_filled(draw, top, ui.style.slider.background);
        r_push_rect2_filled(draw, handle, color);
        r_push_rect2_filled(draw, bottom, ui.style.slider.background);
        r_immediate_draw_end(draw);
      }

      offset_y = widget->scroll_offset_y;
    }
  }

  inner_rect = ui_shrink(&rect, ui.style.widget_margin);
  rect2 init_rect = inner_rect;

  inner_rect = rect2_add_offset(inner_rect, make_v2(0, offset_y));

  if (flags & UI_PANEL_SCROLLABLE_VERT) {
    inner_rect.min.y = inner_rect.max.y;
  }

  ui_panel_t* panel = ui_push_panel(inner_rect);
  panel->id = id;
  panel->flags = flags;
  panel->init_rect = init_rect;
}

void ui_panel_end(void) {
  ASSERT(ui.initialized);

  ui_panel_t* panel = ui.panel;
  ui_widget_t* widget = ui_get_widget(panel->id);

  if (panel->flags & UI_PANEL_SCROLLABLE_VERT) {
    widget->scrollable_height_y = abs_ss(panel->rect.max.y - panel->rect.min.y);
  }

  r_pop_view();
  ui_pop_panel();
}

void ui_label(string_t label) {
  if (NEVER(!ui.initialized)) {
    return;
  }

  rect2 rect = ui_default_label_rect(label);

  rect = ui_shrink(&rect, ui.style.widget_margin);

  rect2 text_rect = ui_shrink(&rect, ui.style.text_margin);

  {
    r_immediate_draw_t* draw = r_immediate_draw_begin(&(r_immediate_params_t){
        .texture = ui.font.texture, .clip_rect = text_rect});
    r_push_text(draw, &ui.font, text_rect.min, ui.style.text, label);
    r_immediate_draw_end(draw);
  }
}

void ui_progress_bar(string_t label, f32 progress) {
  if (NEVER(!ui.initialized)) {
    return;
  }

  rect2 rect = ui_default_label_rect(label);

  rect = ui_shrink(&rect, ui.style.widget_margin);

  rect2 text_rect = ui_shrink(&rect, ui.style.text_margin);

  f32 width = rect2_width(rect);

  rect2 filled = ui_cut_left(&rect, progress * width);
  rect2 empty = rect;

  {
    r_immediate_draw_t* draw = r_immediate_draw_begin(NULL);
    r_push_rect2_filled(draw, filled, ui.style.progress_bar.filled);
    r_push_rect2_filled(draw, empty, ui.style.progress_bar.empty);
    r_immediate_draw_end(draw);
  }
  {
    r_immediate_draw_t* draw = r_immediate_draw_begin(
        &(r_immediate_params_t){.texture = ui.font.texture});
    r_push_text(draw, &ui.font, text_rect.min, ui.style.text, label);
    r_immediate_draw_end(draw);
  }
}

MISSION_INLINE f32 ui_animate_towards(f32 in_rate,
                                      f32 out_rate,
                                      f32 t,
                                      f32 target) {
  if (t < target) {
    t += ui.dt / in_rate;

    if (t > target) {
      t = target;
    }
  } else if (t > target) {
    t -= ui.dt / out_rate;

    if (t < target) {
      t = target;
    }
  }
  return t;
}
MISSION_INLINE v4 ui_animate_towards_exp4(f32 rate, v4 at, v4 target) {
  f32 t = rate * ui.dt;
  v4 result = v4_lerps(target, at, saturate(t));
  return result;
}

typedef enum ui_widget_state_t {
  UI_WIDGET_STATE_COLD,
  UI_WIDGET_STATE_HOT,
  UI_WIDGET_STATE_ACTIVE,
} ui_widget_state_t;

MISSION_INLINE ui_widget_state_t ui_get_widget_state(ui_id_t id) {
  ui_widget_state_t result = UI_WIDGET_STATE_COLD;

  if (ui_is_hot(id)) {
    result = UI_WIDGET_STATE_HOT;
  } else if (ui_is_active(id)) {
    result = UI_WIDGET_STATE_ACTIVE;
  }
  return result;
}

MISSION_INLINE v4 ui_animate_colors(ui_id_t id,
                                    ui_widget_state_t state,
                                    f32 rate,
                                    v4 cold,
                                    v4 hot,
                                    v4 active,
                                    v4 fired) {
  (void)fired;

  ui_widget_t* widget = ui_get_widget(id);

  if (widget->new) {
    widget->interp_color = cold;
  }

  v4 target = cold;
  switch (state) {
    case UI_WIDGET_STATE_COLD: {
      target = cold;
    } break;
    case UI_WIDGET_STATE_HOT: {
      target = hot;
    } break;
    case UI_WIDGET_STATE_ACTIVE: {
      target = active;
    } break;
  }

  if (state == UI_WIDGET_STATE_ACTIVE) {
    rate *= 0.5f;
  }

  widget->interp_color =
      ui_animate_towards_exp4(rate, widget->interp_color, target);
  return widget->interp_color;
}

bool ui_button(string_t label) {
  bool result = false;

  if (NEVER(!ui.initialized))
    return result;

  ui_id_t id = ui_id(label);

  rect2 rect = ui_default_label_rect(label);
  rect = ui_shrink(&rect, ui.style.widget_margin);

  rect2 text_rect = ui_shrink(&rect, ui.style.text_margin);

  u32 interaction = ui_button_behavior(id, rect);
  result = interaction & UI_FIRED;

  ui_widget_state_t state = ui_get_widget_state(id);

  v4 color = ui_animate_colors(id, state, ui.style.animation_rate,
                               ui.style.button.background, ui.style.button.hot,
                               ui.style.button.active, ui.style.button.fired);

  {
    r_immediate_draw_t* draw =
        r_immediate_draw_begin(&(r_immediate_params_t){.clip_rect = rect});
    r_push_rect2_filled(draw, rect, color);
    r_immediate_draw_end(draw);
  }

  {
    r_immediate_draw_t* draw = r_immediate_draw_begin(&(r_immediate_params_t){
        .texture = ui.font.texture, .clip_rect = text_rect});
    r_push_text(draw, &ui.font, text_rect.min, ui.style.text, label);
    r_immediate_draw_end(draw);
  }

  return result;
}

bool ui_checkbox(string_t label, bool* value) {
  bool result = false;

  if (NEVER(!ui.initialized)) {
    return result;
  }

  ui_id_t id = ui_id(label);

  rect2 rect = ui_default_label_rect(label);
  rect = ui_shrink(&rect, ui.style.widget_margin);

  rect2 box_rect = ui_cut_left(&rect, rect2_height(rect));
  rect2 label_rect = ui_cut_left(&rect, (f32)label.count * ui.font.cw);
  label_rect = ui_shrink(&label_rect, ui.style.text_margin);

  u32 interaction = ui_button_behavior(id, box_rect);
  result = interaction & UI_FIRED;

  if (result) {
    if (value) {
      *value = !*value;
    }
  }

  ui_widget_state_t state = ui_get_widget_state(id);

  if (value && *value && state == UI_WIDGET_STATE_COLD) {
    state = UI_WIDGET_STATE_ACTIVE;
  }

  v4 color = ui_animate_colors(id, state, ui.style.animation_rate,
                               ui.style.button.background, ui.style.button.hot,
                               ui.style.button.active, ui.style.button.fired);

  {
    r_immediate_draw_t* draw = r_immediate_draw_begin(NULL);
    r_push_rect2_filled(draw, box_rect, color);
    r_immediate_draw_end(draw);
  }
  {
    r_immediate_draw_t* draw = r_immediate_draw_begin(&(r_immediate_params_t){
        .texture = ui.font.texture,
    });
    r_push_text(draw, &ui.font, label_rect.min, ui.style.text, label);
    r_immediate_draw_end(draw);
  }

  return result;
}

void ui_slider(string_t label, f32* v, f32 min, f32 max) {
  if (NEVER(!v)) {
    return;
  }

  ui_id_t id = ui_id_pointer(v);

  rect2 rect;
  if (!ui_override_rect(&rect)) {
    ui_panel_t* panel = ui_panel();
    rect = ui_cut_top(&panel->rect, (f32)ui.font.ch + ui_widget_padding());
  }

  rect = ui_shrink(&rect, ui.style.widget_margin);

  rect2 label_rect = ui_cut_left(
      &rect, (f32)label.count * ui.font.cw + 2.0f * ui.style.text_margin);
  label_rect = ui_shrink(&label_rect, ui.style.text_margin);

  rect2 slider_body = rect;

  f32 handle_width = ui.style.slider.handle_width;
  f32 handle_half_width = 0.5f * handle_width;

  f32 width = rect2_width(rect);
  f32 relative_x = CLAMP((ui.mouse_p.x - ui.mouse_pressed_offset.x) -
                             rect.min.x - handle_half_width,
                         0.0f, width - handle_width);

  f32 pct = 0.0f;

  if (ui_is_active(id)) {
    pct = relative_x / (width - handle_width);
    *v = min + pct * (max - min);
  } else {
    f32 value = *v;
    pct = (value - min) / (max - min);
  }

  f32 width_exclusive = width - handle_width;
  f32 handle_offset = pct * width_exclusive;

  rect2 left = ui_cut_left(&rect, handle_offset);
  rect2 handle = ui_cut_left(&rect, handle_width);
  rect2 right = rect;

  ui_button_behavior(id, handle);

  ui_widget_state_t state = ui_get_widget_state(id);

  v4 color = ui_animate_colors(id, state, ui.style.animation_rate,
                               ui.style.slider.foreground, ui.style.slider.hot,
                               ui.style.slider.active, ui.style.slider.active);

  {
    r_immediate_draw_t* draw = r_immediate_draw_begin(NULL);
    r_push_rect2_filled(draw, left, ui.style.slider.background);
    r_push_rect2_filled(draw, handle, color);
    r_push_rect2_filled(draw, right, ui.style.slider.background);
    r_immediate_draw_end(draw);
  }

  string_t value_text = Sf("%.03f", *v);
  f32 text_width = (f32)value_text.count * (f32)ui.font.cw;
  v2 text_p = {
      0.5f * (slider_body.min.x + slider_body.max.x) - 0.5f * text_width,
      slider_body.min.y + ui.style.text_margin};

  {
    r_immediate_draw_t* draw = r_immediate_draw_begin(
        &(r_immediate_params_t){.texture = ui.font.texture});
    r_push_text(draw, &ui.font, label_rect.min, ui.style.text, label);
    r_push_text(draw, &ui.font, text_p, ui.style.text, value_text);
    r_immediate_draw_end(draw);
  }
}