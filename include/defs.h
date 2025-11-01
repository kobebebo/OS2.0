#pragma once
#include "types.h"
#include "riscv.h"
#include "memlayout.h"

/* console / printf */
void console_init(void);
void console_putc(int c);
void console_puts(const char *s);
void console_clear(void);
void printf(const char *fmt, ...);

/* uart */
void uart_init(void);
void uart_putc_sync(int c);

/* string */
int  memcmp(const void *, const void *, size_t);
void *memset(void *, int, size_t);
void *memcpy(void *, const void *, size_t);
size_t strlen(const char *s);

/* 物理内存分配 */
void  kinit(void);
void* kalloc(void);
void  kfree(void*);

/* 页表 / Sv39 */
extern pagetable_t kernel_pagetable;
void kvminit(void);
void kvminithart(void);
int  mappages(pagetable_t pt, u64 va, u64 size, u64 pa, int perm);

/* 实验四：陷阱/中断 */
void trapinit(void);        // 使能 S 级要用到的中断位
void trapinithart(void);    // 设置 stvec
void intr_on(void);         // 开总中断（SIE）
void intr_off(void);
int  devintr(void);         // 设备中断分发
void kerneltrap_handler(void) __attribute__((interrupt("supervisor"))); // S 中断入口
void timervec(void) __attribute__((interrupt("machine")));              // M 中断入口

extern volatile u64 ticks;  // 时钟节拍计数
