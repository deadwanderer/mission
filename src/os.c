#include "core.h"
#include <stdio.h>

// typedef struct UnicodeDecode UnicodeDecode;
// struct UnicodeDecode {
//   u32 inc;
//   u32 codepoint;
// };

// static u8 utf8_class[32] = {
//     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
//     0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 4, 5,
// };

// static UnicodeDecode utf8_decode(u8* str, u64 max) {
//   UnicodeDecode result = {1, max_U32};
//   U8 byte = str[0];
//   U8 byte_class = utf8_class[byte >> 3];
//   switch (byte_class) {
//     case 1: {
//       result.codepoint = byte;
//     } break;
//     case 2: {
//       if (2 < max) {
//         U8 cont_byte = str[1];
//         if (utf8_class[cont_byte >> 3] == 0) {
//           result.codepoint = (byte & bitmask5) << 6;
//           result.codepoint |= (cont_byte & bitmask6);
//           result.inc = 2;
//         }
//       }
//     } break;
//     case 3: {
//       if (2 < max) {
//         U8 cont_byte[2] = {str[1], str[2]};
//         if (utf8_class[cont_byte[0] >> 3] == 0 &&
//             utf8_class[cont_byte[1] >> 3] == 0) {
//           result.codepoint = (byte & bitmask4) << 12;
//           result.codepoint |= ((cont_byte[0] & bitmask6) << 6);
//           result.codepoint |= (cont_byte[1] & bitmask6);
//           result.inc = 3;
//         }
//       }
//     } break;
//     case 4: {
//       if (3 < max) {
//         U8 cont_byte[3] = {str[1], str[2], str[3]};
//         if (utf8_class[cont_byte[0] >> 3] == 0 &&
//             utf8_class[cont_byte[1] >> 3] == 0 &&
//             utf8_class[cont_byte[2] >> 3] == 0) {
//           result.codepoint = (byte & bitmask3) << 18;
//           result.codepoint |= ((cont_byte[0] & bitmask6) << 12);
//           result.codepoint |= ((cont_byte[1] & bitmask6) << 6);
//           result.codepoint |= (cont_byte[2] & bitmask6);
//           result.inc = 4;
//         }
//       }
//     }
//   }
//   return (result);
// }

// internal UnicodeDecode utf16_decode(U16* str, U64 max) {
//   UnicodeDecode result = {1, max_U32};
//   result.codepoint = str[0];
//   result.inc = 1;
//   if (max > 1 && 0xD800 <= str[0] && str[0] < 0xDC00 && 0xDC00 <= str[1] &&
//       str[1] < 0xE000) {
//     result.codepoint =
//         ((str[0] - 0xD800) << 10) | ((str[1] - 0xDC00) + 0x10000);
//     result.inc = 2;
//   }
//   return (result);
// }

// internal U32 utf8_encode(U8* str, U32 codepoint) {
//   U32 inc = 0;
//   if (codepoint <= 0x7F) {
//     str[0] = (U8)codepoint;
//     inc = 1;
//   } else if (codepoint <= 0x7FF) {
//     str[0] = (bitmask2 << 6) | ((codepoint >> 6) & bitmask5);
//     str[1] = bit8 | (codepoint & bitmask6);
//     inc = 2;
//   } else if (codepoint <= 0xFFFF) {
//     str[0] = (bitmask3 << 5) | ((codepoint >> 12) & bitmask4);
//     str[1] = bit8 | ((codepoint >> 6) & bitmask6);
//     str[2] = bit8 | (codepoint & bitmask6);
//     inc = 3;
//   } else if (codepoint <= 0x10FFFF) {
//     str[0] = (bitmask4 << 4) | ((codepoint >> 18) & bitmask3);
//     str[1] = bit8 | ((codepoint >> 12) & bitmask6);
//     str[2] = bit8 | ((codepoint >> 6) & bitmask6);
//     str[3] = bit8 | (codepoint & bitmask6);
//     inc = 4;
//   } else {
//     str[0] = '?';
//     inc = 1;
//   }
//   return (inc);
// }

// internal U32 utf16_encode(U16* str, U32 codepoint) {
//   U32 inc = 1;
//   if (codepoint == max_U32) {
//     str[0] = (U16)'?';
//   } else if (codepoint < 0x10000) {
//     str[0] = (U16)codepoint;
//   } else {
//     U32 v = codepoint - 0x10000;
//     str[0] = safe_cast_u16(0xD800 + (v >> 10));
//     str[1] = safe_cast_u16(0xDC00 + (v & bitmask10));
//     inc = 2;
//   }
//   return (inc);
// }

// string16_t utf16_from_utf8(arena_t* arena, string_t utf8) {
//   if (NEVER(utf8.count > INT_MAX)) {
//     utf8.count = INT_MAX;
//   }

//   if (utf8.count == 0) {
//     return (string16_t){0};
//   }

//   // int utf16_count =
//   //     MultiByteToWideChar(CP_UTF8, 0, utf8.data, (int)utf8.count, NULL,
//   0); u64 cap = utf8.count * 2; u64 utf16_count = 0; wchar_t* utf16_data =
//   m_alloc_string16(arena, cap + 1);

//   if (ALWAYS(utf16_data)) {
//     // MultiByteToWideChar(CP_UTF8, 0, utf8.data, (int)utf8.count,
//     //                     (wchar_t*)utf16_data, utf16_count);
//     u8* ptr = utf8.data;
//     u8* opl = ptr + utf8.count;
//     UnicodeDecode consume;
//     for (; ptr < opl; ptr += consume.inc) {
//       consume = utf8_decode(ptr, opl - ptr);
//       utf16_count += utf16_encode(utf16_data + utf16_count,
//       consume.codepoint);
//     }
//     utf16_data[utf16_count] = 0;
//   }

//   string16_t result = {
//       .count = (size_t)utf16_count,
//       .data = utf16_data,
//   };

//   return result;
// }

// string_t utf8_from_utf16(arena_t* arena, string16_t utf16) {
//   if (NEVER(utf16.count > INT_MAX)) {
//     utf16.count = INT_MAX;
//   }

//   if (utf16.count == 0) {
//     return (string_t){0};
//   }

//   // int utf8_count = WideCharToMultiByte(CP_UTF8, 0, utf16.data,
//   // (int)utf16.count,
//   //                                      NULL, 0, NULL, NULL);
//   u64 cap = utf16.count * 3;
//   u64 utf8_count = 0;
//   char* utf8_data = m_alloc_string(arena, cap + 1);

//   if (ALWAYS(utf8_data)) {
//     // WideCharToMultiByte(CP_UTF8, 0, utf16.data, (int)utf16.count,
//     //                     (char*)utf8_data, utf8_count, NULL, NULL);
//     u16* ptr = utf16.data;
//     u16* opl = ptr + utf16.count;
//     UnicodeDecode consume;
//     for (; ptr < opl; ptr += consume.inc) {
//       consume = utf16_decode(ptr, opl - ptr);
//       utf8_count += utf8_encode(utf8_data + utf8_count, consume.codepoint);
//     }
//     utf8_data[utf8_count] = 0;
//   }

//   string_t result = {
//       .count = (size_t)utf8_count,
//       .data = utf8_data,
//   };

//   return result;
// }

void fatal_error(int line, string_t file, const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  fatal_error_va(line, file, fmt, args);
  // va_end (args);
}

void fatal_error_va(int line, string_t file, const char* fmt, va_list args) {
  char buffer[4096];

  string_t message =
      string_format_into_buffer_va(buffer, sizeof(buffer), fmt, args);
  string_t formatted = string_format_into_buffer(
      buffer + message.count, sizeof(buffer) - message.count,
      "Fatal error: %.*s\nLine: %d\nFile: %.*s\n", strexpand(message), line,
      strexpand(file));

  fprintf(stderr, formatted.data);
  fflush(stderr);

  __debugbreak();
}

// void* vm_reserve(void* address, size_t size) {
//   void* result = VirtualAlloc(address, size, MEM_RESERVE, PAGE_NOACCESS);
//   return result;
// }

// bool vm_commit(void* address, size_t size) {
//   void* result = VirtualAlloc(address, size, MEM_COMMIT, PAGE_READWRITE);

//   if (!result) {
//     output_last_error(strlit16("vm_commit failed"));
//   }

//   return !!result;
// }

// void vm_decommit(void* address, size_t size) {
//   VirtualFree(address, size, MEM_DECOMMIT);
// }

// void vm_release(void* address) {
//   VirtualFree(address, 0, MEM_RELEASE);
// }

void debug_print_va(const char* fmt, va_list args) {
  string_t string = string_format_va(temp, fmt, args);
  // OutputDebugStringA(string.data);
  fprintf(stdout, string.data);
  fflush(stdout);
}

void debug_print(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);

  debug_print_va(fmt, args);

  va_end(args);
}