// riscv-os / kernel/console.c
#include "defs.h"

void console_init(void) {
  uart_init();
}

void console_putc(int c) {
  if (c == '\n')
    uart_putc_sync('\r');
  uart_putc_sync(c);
}

void console_puts(const char *s) {
  while (s && *s) console_putc(*s++);
}

/* ANSI 清屏：ESC[2J 清屏 + ESC[H 光标回到左上角
   参考：ANSI/VT100 转义序列文档。 */
void console_clear(void) {
  console_putc('\x1b'); console_putc('['); console_putc('2'); console_putc('J');
  console_putc('\x1b'); console_putc('['); console_putc('H');
}
