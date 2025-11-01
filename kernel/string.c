#include "types.h"

int memcmp(const void *a, const void *b, size_t n) {
  const u8 *x = (const u8 *)a;
  const u8 *y = (const u8 *)b;
  for (size_t i = 0; i < n; i++) {
    if (x[i] != y[i]) return (int)x[i] - (int)y[i];
  }
  return 0;
}

void *memset(void *p, int v, size_t n) {
  u8 *x = (u8 *)p;
  for (size_t i = 0; i < n; i++) {
    x[i] = (u8)v;
  }
  return p;
}

void *memcpy(void *d, const void *s, size_t n) {
  u8 *x = (u8 *)d;
  const u8 *y = (const u8 *)s;
  for (size_t i = 0; i < n; i++) {
    x[i] = y[i];
  }
  return d;
}
