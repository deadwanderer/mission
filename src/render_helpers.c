#include "render_helpers.h"
#include "render.h"

void r_push_line(r_immediate_draw_t* draw_call, v2 start, v2 end, v4 color) {
  u32 color_packed = pack_color(color);
  u32 i0 = r_immediate_vertex(
      draw_call, &(vertex_immediate_t){.pos = start, .col = color_packed});
  r_immediate_index(draw_call, i0);
  u32 i1 = r_immediate_vertex(
      draw_call, &(vertex_immediate_t){.pos = end, .col = color_packed});
  r_immediate_index(draw_call, i1);
}

void r_push_line_gradient(r_immediate_draw_t* draw_call,
                          v2 start,
                          v2 end,
                          v4 start_color,
                          v4 end_color) {
  u32 i0 = r_immediate_vertex(
      draw_call,
      &(vertex_immediate_t){.pos = start, .col = pack_color(start_color)});
  r_immediate_index(draw_call, i0);
  u32 i1 = r_immediate_vertex(
      draw_call,
      &(vertex_immediate_t){.pos = end, .col = pack_color(end_color)});
  r_immediate_index(draw_call, i1);
}

// void r_push_arrow_gradient(r_immediate_draw_t* draw_call,
//                            v2 start,
//                            v2 end,
//                            v4 start_color,
//                            v4 end_color) {
//   f32 head_size = 1.0f;

//   v2 arrow_vector = sub(end, start);
//   f32 arrow_length = vlen(arrow_vector);

//   v2 arrow_direction = normalize_or_zero(arrow_vector);

//   f32 shaft_length = max(0.0f, arrow_length - 3.0f * head_size);

//   v2 shaft_vector = mul(shaft_length, arrow_direction);
//   v2 shaft_end = add(start, shaft_vector);

//   v2 t, b;
//   get_tangent_vectors(arrow_direction, &t, &b);

//   size_t arrow_segment_count = 8;
//   for (size_t i = 0; i < arrow_segment_count; i++) {
//     f32 circ0 = 2.0f * PI32 * ((f32)(i + 0) / (f32)arrow_segment_count);
//     f32 circ1 = 2.0f * PI32 * ((f32)(i + 1) / (f32)arrow_segment_count);

//     f32 s0, c0;
//     sincos_ss(circ0, &s0, &c0);

//     f32 s1, c1;
//     sincos_ss(circ1, &s1, &c1);

//     v2 v0 = add(shaft_end, add(mul(t, head_size * s0), mul(b, head_size *
//     c0))); v2 v1 = add(shaft_end, add(mul(t, head_size * s1), mul(b,
//     head_size * c1))); r_push_line_gradient(draw_call, v0, v1, end_color,
//     end_color); r_push_line_gradient(draw_call, v0, end, end_color,
//     end_color);
//   }
//   r_push_line_gradient(draw_call, start, shaft_end, start_color, end_color);
// }

// void r_push_arrow(r_immediate_draw_t* draw_call, v2 start, v2 end, v4 color)
// {
//   r_push_arrow_gradient(draw_call, start, end, color, color);
// }

void r_push_rect2_filled(r_immediate_draw_t* draw_call, rect2 rect, v4 color) {
  u32 color_packed = pack_color(color);
  u32 i0 = r_immediate_vertex(
      draw_call,
      &(vertex_immediate_t){
          .pos = {rect.min.x, rect.min.y}, .tex = {0, 0}, .col = color_packed});
  u32 i1 = r_immediate_vertex(
      draw_call,
      &(vertex_immediate_t){
          .pos = {rect.max.x, rect.min.y}, .tex = {1, 0}, .col = color_packed});
  u32 i2 = r_immediate_vertex(
      draw_call,
      &(vertex_immediate_t){
          .pos = {rect.max.x, rect.max.y}, .tex = {1, 1}, .col = color_packed});
  u32 i3 = r_immediate_vertex(
      draw_call,
      &(vertex_immediate_t){
          .pos = {rect.min.x, rect.max.y}, .tex = {0, 1}, .col = color_packed});

  r_immediate_index(draw_call, i0);
  r_immediate_index(draw_call, i1);
  r_immediate_index(draw_call, i2);

  r_immediate_index(draw_call, i0);
  r_immediate_index(draw_call, i2);
  r_immediate_index(draw_call, i3);
}

void r_push_rect2_filled_gradient(r_immediate_draw_t* draw_call,
                                  rect2 rect,
                                  v4 colors[4]) {
  u32 i0 = r_immediate_vertex(
      draw_call, &(vertex_immediate_t){.pos = {rect.min.x, rect.min.y},
                                       .tex = {0, 0},
                                       .col = pack_color(colors[0])});
  u32 i1 = r_immediate_vertex(
      draw_call, &(vertex_immediate_t){.pos = {rect.max.x, rect.min.y},
                                       .tex = {1, 0},
                                       .col = pack_color(colors[1])});
  u32 i2 = r_immediate_vertex(
      draw_call, &(vertex_immediate_t){.pos = {rect.max.x, rect.max.y},
                                       .tex = {1, 1},
                                       .col = pack_color(colors[2])});
  u32 i3 = r_immediate_vertex(
      draw_call, &(vertex_immediate_t){.pos = {rect.min.x, rect.max.y},
                                       .tex = {0, 1},
                                       .col = pack_color(colors[3])});

  r_immediate_index(draw_call, i0);
  r_immediate_index(draw_call, i1);
  r_immediate_index(draw_call, i2);

  r_immediate_index(draw_call, i0);
  r_immediate_index(draw_call, i2);
  r_immediate_index(draw_call, i3);
}

// void r_push_rect3_outline(r_immediate_draw_t* draw_call,
//                           rect3 bounds,
//                           v4 color) {
//   v3 v000 = {bounds.min.x, bounds.min.y, bounds.min.z};
//   v3 v100 = {bounds.max.x, bounds.min.y, bounds.min.z};
//   v3 v010 = {bounds.min.x, bounds.max.y, bounds.min.z};
//   v3 v110 = {bounds.max.x, bounds.max.y, bounds.min.z};

//   v3 v001 = {bounds.min.x, bounds.min.y, bounds.max.z};
//   v3 v101 = {bounds.max.x, bounds.min.y, bounds.max.z};
//   v3 v011 = {bounds.min.x, bounds.max.y, bounds.max.z};
//   v3 v111 = {bounds.max.x, bounds.max.y, bounds.max.z};

//   // bottom plane
//   r_push_line(draw_call, v000, v100, color);
//   r_push_line(draw_call, v100, v110, color);
//   r_push_line(draw_call, v110, v010, color);
//   r_push_line(draw_call, v010, v000, color);

//   // top plane
//   r_push_line(draw_call, v001, v101, color);
//   r_push_line(draw_call, v101, v111, color);
//   r_push_line(draw_call, v111, v011, color);
//   r_push_line(draw_call, v011, v001, color);

//   // "pillars"
//   r_push_line(draw_call, v000, v001, color);
//   r_push_line(draw_call, v100, v101, color);
//   r_push_line(draw_call, v010, v011, color);
//   r_push_line(draw_call, v110, v111, color);
// }

void r_push_text(struct r_immediate_draw_t* draw_call,
                 const struct bitmap_font_t* font,
                 v2 p,
                 v4 color,
                 string_t string) {
  ASSERT(RESOURCE_HANDLES_EQUAL(draw_call->params.texture, font->texture));

  ASSERT(font->w / font->cw == 16);
  ASSERT(font->w % font->cw == 0);
  ASSERT(font->h / font->ch >= 16);

  u32 color_packed = pack_color(color);

  f32 cw = (f32)font->cw;
  f32 ch = (f32)font->ch;

  f32 newline_offset = 0;
  for (size_t i = 0; i < string.count; i++) {
    if (i + 1 < string.count && string.data[i] == '\n') {
      newline_offset += ch;
    }
  }

  v2 at = p;
  at.y += newline_offset;

  for (size_t i = 0; i < string.count; i++) {
    char c = string.data[i];

    f32 cx = (f32)(c % 16);
    f32 cy = (f32)(c / 16);

    if (is_newline(c)) {
      at.y -= ch;
      at.x = p.x;
    } else {
      f32 u0 = cx / 16.0f;
      f32 u1 = u0 + (1.0f / 16.0f);

      f32 v0 = cy / 16.0f;
      f32 v1 = v0 + (1.0f / 16.0f);

      u32 i0 = r_immediate_vertex(draw_call, &(vertex_immediate_t){
                                                 .pos = {at.x, at.y},
                                                 .tex = {u0, v1},
                                                 .col = color_packed,
                                             });
      u32 i1 = r_immediate_vertex(draw_call, &(vertex_immediate_t){
                                                 .pos = {at.x + cw, at.y},
                                                 .tex = {u1, v1},
                                                 .col = color_packed,
                                             });
      u32 i2 = r_immediate_vertex(draw_call, &(vertex_immediate_t){
                                                 .pos = {at.x + cw, at.y + ch},
                                                 .tex = {u1, v0},
                                                 .col = color_packed,
                                             });
      u32 i3 = r_immediate_vertex(draw_call, &(vertex_immediate_t){
                                                 .pos = {at.x, at.y + ch},
                                                 .tex = {u0, v0},
                                                 .col = color_packed,
                                             });

      r_immediate_index(draw_call, i0);
      r_immediate_index(draw_call, i1);
      r_immediate_index(draw_call, i2);
      r_immediate_index(draw_call, i0);
      r_immediate_index(draw_call, i2);
      r_immediate_index(draw_call, i3);

      at.x += cw;
    }
  }
}