
/*
 * Copyright (C) 2025 Masoud Bolhassani <masoud.bolhassani@gmail.com>
 *
 * This file is part of Kelbaran.
 *
 * Kelbaran is released under the GNU General Public License v3 (GPL-3.0).
 * See LICENSE file for details.
 */

#include "libc_stubs.h"

// Minimal memset implementation
void *memset(void *dest, int val, size_t len) {
  unsigned char *ptr = dest;
  while (len-- > 0) {
    *ptr++ = (unsigned char)val;
  }
  return dest;
}

// Minimal memcpy implementation
void *memcpy(void *dest, const void *src, size_t len) {
  unsigned char *d = dest;
  const unsigned char *s = src;
  while (len-- > 0) {
    *d++ = *s++;
  }
  return dest;
}

// Minimal strcpy implementation (assumes dest is large enough)
char *strcpy(char *dest, const char *src) {
  char *d = dest;
  while ((*d++ = *src++))
    ;
  return dest;
}

// Minimal __libc_init_array (does nothing)
void __libc_init_array(void) {
  // No global/static C++ constructors used, safe to leave empty.
}

size_t strlen(const char *s) {
  size_t len = 0;
  while (s[len])
    ++len;
  return len;
}

int strcmp(const char *s1, const char *s2) {
  while (*s1 && (*s1 == *s2)) {
    s1++;
    s2++;
  }
  return (unsigned char)*s1 - (unsigned char)*s2;
}

// Minimal strcat implementation
char *strcat(char *dest, const char *src) {
  char *d = dest + strlen(dest);
  while ((*d++ = *src++))
    ;
  return dest;
}

// Minimal abs implementation
int abs(int v) {
  return v < 0 ? -v : v;
}

void safe_strncpy(char *dest, const char *src, size_t max_len) {
  if (max_len == 0) return;
  size_t src_len = strlen(src);
  size_t copy_len = (src_len < max_len) ? src_len : max_len - 1;
  memcpy(dest, src, copy_len);
  dest[copy_len] = '\0';  // Ensure null termination
}

// Convert unsigned integer to string
void uint_to_str(uint32_t value, char *buffer, int buffer_size) {
  if (buffer_size <= 0) return;
  
  char temp[12]; // Max digits for 32-bit uint plus null terminator
  int i = 0;
  
  if (value == 0) {
    temp[i++] = '0';
  } else {
    while (value > 0 && i < 11) {
      temp[i++] = '0' + (value % 10);
      value /= 10;
    }
  }
  
  temp[i] = '\0';
  
  // Reverse the string
  int len = i;
  for (int j = 0; j < len && j < buffer_size - 1; j++) {
    buffer[j] = temp[len - 1 - j];
  }
  buffer[len < buffer_size ? len : buffer_size - 1] = '\0';
}

// Simple sprintf implementation (supports %s and %lu only)
int simple_sprintf(char *buffer, const char *format, ...) {
  va_list args;
  va_start(args, format);
  
  char *buf_ptr = buffer;
  const char *fmt_ptr = format;
  
  while (*fmt_ptr) {
    if (*fmt_ptr == '%') {
      fmt_ptr++;
      if (*fmt_ptr == 's') {
        // String argument
        char *str_arg = va_arg(args, char*);
        while (*str_arg) {
          *buf_ptr++ = *str_arg++;
        }
      } else if (*fmt_ptr == 'l' && *(fmt_ptr + 1) == 'u') {
        // Long unsigned integer
        uint32_t uint_arg = va_arg(args, uint32_t);
        char num_str[12];
        uint_to_str(uint_arg, num_str, sizeof(num_str));
        char *num_ptr = num_str;
        while (*num_ptr) {
          *buf_ptr++ = *num_ptr++;
        }
        fmt_ptr++; // Skip the 'u' as well
      } else {
        // Unknown format, just copy the %
        *buf_ptr++ = '%';
        *buf_ptr++ = *fmt_ptr;
      }
    } else {
      *buf_ptr++ = *fmt_ptr;
    }
    fmt_ptr++;
  }
  
  *buf_ptr = '\0';
  va_end(args);
  return buf_ptr - buffer;
}
