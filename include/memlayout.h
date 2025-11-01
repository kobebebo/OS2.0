#pragma once

#define UART0       0x10000000UL
#define VIRTIO0     0x10001000UL
#define PLIC        0x0c000000UL
#define CLINT       0x02000000UL

/* QEMU virt: CLINT 定时器寄存器地址 */
#define CLINT_MTIME        (CLINT + 0xBFF8)                 /* 64-bit mtime */
#define CLINT_MTIMECMP(h)  (CLINT + 0x4000 + 8UL*(h))       /* per-hart */

#define KERN_BASE   0x80000000UL
#define PHYSTOP     (KERN_BASE + 128UL*1024*1024)

#define PGSIZE      4096UL
#define PGROUNDUP(sz)   (((sz) + PGSIZE - 1) & ~(PGSIZE - 1))
#define PGROUNDDOWN(a)  ((a) & ~(PGSIZE - 1))
