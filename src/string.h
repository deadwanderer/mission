#ifndef STRING_H
#define STRING_H

#include "api_types.h"

size_t string_count(const char* string);
size_t string16_count(const wchar_t* string);

string_t string_copy(arena_t* arena, string_t string);

MISSION_INLINE bool string_empty(string_t string) {
  return string.count == 0 || !string.data;
}

MISSION_INLINE string_t string_from_cstr(const char* string) {
  string_t result;
  result.count = string_count(string);
  result.data = string;
  return result;
}

MISSION_INLINE string16_t string16_from_cstr(const wchar_t* string) {
  string16_t result;
  result.count = string16_count(string);
  result.data = string;
  return result;
}

string_t string_copy(arena_t* arena, string_t string);
string_t string_copy_cstr(arena_t* arena, const char* string);
const char* string_null_terminate(arena_t* arena, string_t string);
const wchar_t* string16_null_terminate(arena_t* arena, string16_t string);

string_t string_format(arena_t* arena, const char* fmt, ...);
string_t string_format_va(arena_t* arena, const char* fmt, va_list args);

#define Sf(fmt, ...) string_format(temp, fmt, ##__VA_ARGS__)

string_t string_format_into_buffer(char* buffer,
                                   size_t buffer_size,
                                   const char* fmt,
                                   ...);
string_t string_format_into_buffer_va(char* buffer,
                                      size_t buffer_size,
                                      const char* fmt,
                                      va_list args);

#define STRING_NPOS SIZE_MAX

MISSION_INLINE char string_peek(string_t string, size_t index) {
  if (index > string.count) {
    return 0;
  }
  return string.data[index];
}

string_t substring(string_t string, size_t first, size_t count);

size_t string_find_first_char(string_t string, char c);
size_t string_find_last_char(string_t string, char c);

size_t string_find_first(string_t string, string_t needle);

string_t string_extension(string_t string);
string_t string_strip_extension(string_t string);

string_t string_path_leaf(string_t path);

bool string_match(string_t a, string_t b);
bool string_match_nocase(string_t a, string_t b);
bool string_match_prefix(string_t string, string_t prefix);

MISSION_INLINE bool is_whitespace(char c) {
  return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

MISSION_INLINE bool is_newline(char c) {
  return c == '\n' || c == '\r';
}

MISSION_INLINE bool is_alphabetic(char c) {
  return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

MISSION_INLINE bool is_numeric(char c) {
  return (c >= '0' && c <= '9');
}

extern char to_lowercase_table[256];

MISSION_INLINE char to_lower(char c) {
  return to_lowercase_table[(unsigned char)c];
}

string_t string_trim_left_spaces(string_t string);
string_t string_trim_right_spaces(string_t string);
string_t string_trim_spaces(string_t string);

void string_skip(string_t* string, size_t count);

string_t string_split(string_t* string, string_t separator);
string_t string_split_word(string_t* string);
string_t string_split_line(string_t* string);
size_t string_count_newlines(string_t string);

bool string_parse_int(string_t* string, s64* value);
bool string_parse_f32(string_t* string, f32* value);

string16_t utf16_from_utf8(arena_t* arena, string_t string);
string_t utf8_from_utf16(arena_t* arena, string16_t string);

u64 string_hash(string_t string);
u64 string_hash_with_seed(string_t string, u64 seed);

#endif  // STRING_H