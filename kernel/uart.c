// riscv-os / kernel/uart.c
#include "defs.h"
#include "memlayout.h"
#include "types.h"

#define R(off)   ((volatile u8 *)(UART0 + (off)))

/* 16550A 寄存器偏移 */
#define RHR 0x00  /* Receive Holding Register (read) */
#define THR 0x00  /* Transmit Holding Register (write) */
#define IER 0x01  /* Interrupt Enable */
#define FCR 0x02  /* FIFO Control (write) */
#define LCR 0x03  /* Line Control */
#define LSR 0x05  /* Line Status */

#define LCR_WLEN8 0x03   /* 8 data bits, no parity, 1 stop bit */
#define FCR_FIFO_EN 0x01
#define LSR_TX_IDLE 0x20 /* THR empty */

void uart_init(void) {
  *R(IER) = 0x00;                 /* 关闭中断：实验一用轮询 */
  *R(FCR) = FCR_FIFO_EN;          /* 打开 FIFO（最简） */
  *R(LCR) = LCR_WLEN8;            /* 8N1 */
}

void uart_putc_sync(int c) {
  /* 等待 THR 空，再写入一个字节 */
  while ((*R(LSR) & LSR_TX_IDLE) == 0) { /* spin */ }
  *R(THR) = (u8)c;
}
