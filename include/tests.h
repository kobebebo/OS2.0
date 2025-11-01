#pragma once
#include "defs.h"
#include "memlayout.h"

/* ===== 实验一 ===== */
static inline void test_boot_uart(void) {
  const char *msg = "[TEST] UART bootstrap path OK.\n";
  for (const char *p = msg; *p; ++p) console_putc(*p);
}
static inline void test_printf_basic(void) {
  printf("[TEST] printf: %%d=%d, %%x=%x, %%s=%s, %%c=%c, %%p=%p\n",
         -42, 0x1234abcd, "hello", 'A', (void*)0x80000000UL);
}

/* ===== 实验二 ===== */
static inline void test_printf_edge_cases(void) {
  printf("[TEST] printf edge: null-string=%s, zero=%d, neg=%d\n",
         (char*)0, 0, -1);
  printf("[TEST] widths(not supported, should print raw): %%08x=%08x\n", 0xAB);
  printf("[TEST] unsigned: %%u=%u, %%lu=%lu\n", (unsigned)4294967295u, (unsigned long)18446744073709551615ul);
  printf("[TEST] hex: %%x=%x, %%lx=%lx\n", 0xdeadbeef, 0x12345678abcdef00ul);
  int int_min = (int)0x80000000u;
  printf("[TEST] INT_MIN=%d\n", int_min);
}
static inline void test_console_clear(void) {
  printf("[TEST] will clear screen in 1s...\n");
  for (volatile unsigned long i = 0; i < 5000000UL; ++i) { }
  console_clear();
  printf("=== SCREEN CLEARED ===\n");
}

/* ===== 实验三：物理页分配 + Sv39 映射 ===== */
static inline void test_kalloc_basic(void) {
  void *p1 = kalloc(), *p2 = kalloc(), *p3 = kalloc();
  printf("[TEST] kalloc: p1=%p p2=%p p3=%p\n", p1, p2, p3);
  if (!p1 || !p2 || !p3) printf("[WARN] kalloc returned NULL (low memory?)\n");

  // 简单写读校验
  if (p1) { ((volatile u64*)p1)[0] = 0x1122334455667788ULL; }
  if (p2) { ((volatile u64*)p2)[0] = 0xaabbccddeeff0011ULL; }

  if (p1) printf("[TEST] p1[0]=0x%lx\n", (unsigned long)((volatile u64*)p1)[0]);
  if (p2) printf("[TEST] p2[0]=0x%lx\n", (unsigned long)((volatile u64*)p2)[0]);

  if (p3) kfree(p3);
  if (p2) kfree(p2);
  if (p1) kfree(p1);
}

// 给一页创建别名映射，验证同一物理页的两个 VA 可见同一数据
static inline void test_vm_alias_map(void) {
  void *page = kalloc();
  if (!page) { printf("[WARN] no page for alias test\n"); return; }

  u64 pa = (u64)page;
  u64 alias_va = PHYSTOP + 0x400000;     // ★ 放在恒等映射之外
  int r = mappages(kernel_pagetable, alias_va, PGSIZE, pa, PTE_R|PTE_W);
  if (r < 0) { printf("[ERR] mappages alias failed\n"); return; }

  sfence_vma();                          // ★ 刷新 TLB

  volatile u64 *a = (volatile u64*)alias_va;
  volatile u64 *p = (volatile u64*)page;
  a[0] = 0xdeadbeefcafebabeULL;
  printf("[TEST] alias write, page[0]=0x%lx\n", (unsigned long)p[0]);
  kfree(page);
}



/* ===== 实验四：时钟中断 & 异常演示 ===== */
static inline void test_timer_interrupt(void) {
  u64 t0 = ticks;
  printf("[TEST] ticks(now)=%lu, waiting for ~5 ticks...\n", (unsigned long)t0);
  while (ticks < t0 + 5) { /* busy-wait */ }
  printf("[TEST] ticks=%lu OK\n", (unsigned long)ticks);
}

static inline void test_exception_illegal_insn(void) {
  printf("[TEST] trigger illegal instruction, kernel should print trap info.\n");
  asm volatile(".word 0xffffffff");  // 非法指令，触发异常
}
