// riscv-os / kernel/printf.c
#include "defs.h"
#include <stdarg.h>

static void putc(int c){ console_putc(c); }

static void printuint(unsigned long x, int base) {
  char buf[32]; int i = 0;
  do {
    int d = x % base; x /= base;
    buf[i++] = "0123456789abcdef"[d];
  } while (x);
  while (i--) putc(buf[i]);
}

static void printint(long x, int base, int sgn) {
  unsigned long xx;
  if (sgn && x < 0) { putc('-'); xx = (unsigned long)(-x); }
  else xx = (unsigned long)x;
  printuint(xx, base);
}

static void printptr(const void *p) {
  unsigned long x = (unsigned long)p;
  putc('0'); putc('x');
  for (int i = (int)(sizeof(unsigned long)*2)-1; i >= 0; --i) {
    int d = (x >> (i*4)) & 0xF;
    putc("0123456789abcdef"[d]);
  }
}

void printf(const char *fmt, ...) {
  va_list ap; va_start(ap, fmt);
  for (; *fmt; ++fmt) {
    if (*fmt != '%') { putc(*fmt); continue; }

    ++fmt;
    int longflag = 0;
    if (*fmt == 'l') { longflag = 1; ++fmt; }     // 识别 %l 修饰

    switch (*fmt) {
      case 'd':
        if (longflag) printint(va_arg(ap, long), 10, 1);
        else          printint(va_arg(ap, int),  10, 1);
        break;
      case 'u':
        if (longflag) printuint(va_arg(ap, unsigned long), 10);
        else          printuint(va_arg(ap, unsigned int),  10);
        break;
      case 'x':
        if (longflag) printuint(va_arg(ap, unsigned long), 16);
        else          printuint(va_arg(ap, unsigned int),  16);
        break;
      case 'p':
        printptr(va_arg(ap, void*));
        break;
      case 's': {
        const char *s = va_arg(ap, const char*);
        if (!s) s = "(null)";
        while (*s) putc(*s++);
        break;
      }
      case 'c':
        putc(va_arg(ap, int));
        break;
      case '%':
        putc('%');
        break;
      default:
        /* 未知格式：按字面输出，方便排错 */
        putc('%'); putc(*fmt);
        break;
    }
  }
  va_end(ap);
}
