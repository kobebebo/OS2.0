// riscv-os / kernel/vm.c
#include "defs.h"
#include "memlayout.h"

// 对外暴露：内核页表
pagetable_t kernel_pagetable;

// 内部：walk 到最后一级，alloc=1 时按需分配中间页表页
static pte_t* walk(pagetable_t pagetable, u64 va, int alloc) {
  if (va >= (1ULL<<39)) return 0; // Sv39 有效 VA 检查

  for (int level = 2; level > 0; level--) {
    pte_t *pte = &pagetable[PX(level, va)];
    if (*pte & PTE_V) {
      pagetable = (pagetable_t)PTE2PA(*pte);
    } else {
      if (!alloc) return 0;
      void *newpg = kalloc();
      if (!newpg) return 0;
      memset(newpg, 0, PGSIZE);
      *pte = PA2PTE((u64)newpg) | PTE_V;
      pagetable = (pagetable_t)newpg;
    }
  }
  return &pagetable[PX(0, va)];
}

// 将 [va, va+size) 映射到 [pa, pa+size)，按页粒度，perm 为 PTE 权限
int mappages(pagetable_t pagetable, u64 va, u64 size, u64 pa, int perm) {
  u64 a = PGROUNDDOWN(va);
  u64 last = PGROUNDDOWN(va + size - 1);
  for (;;) {
    pte_t *pte = walk(pagetable, a, 1);
    if (pte == 0) return -1;
    if (*pte & PTE_V) {
      printf("mappages: remap at va=0x%lx\n", a);
      return -1;
    }
    *pte = PA2PTE(pa) | perm | PTE_V;
    if (a == last) break;
    a += PGSIZE;
    pa += PGSIZE;
  }
  return 0;
}

// 内核页表建立（仿照 xv6：代码段 RX、数据段 RW、设备区 RW）
extern char etext[];  // 由链接脚本提供：.text 末尾

void kvminit(void) {
  kernel_pagetable = (pagetable_t)kalloc();
  if (!kernel_pagetable) { printf("kvminit: no mem\n"); for(;;){} }
  memset(kernel_pagetable, 0, PGSIZE);

  // 设备区：恒等映射，RW
  mappages(kernel_pagetable, UART0,   PGSIZE,   UART0,   PTE_R|PTE_W);
  mappages(kernel_pagetable, VIRTIO0, PGSIZE,   VIRTIO0, PTE_R|PTE_W);
  mappages(kernel_pagetable, PLIC,    0x400000, PLIC,    PTE_R|PTE_W);
  mappages(kernel_pagetable, CLINT,   0x10000,  CLINT,   PTE_R|PTE_W);

  // 代码段：从 KERN_BASE 到 text_end（页对齐），RX
  u64 text_start = KERN_BASE;
  u64 text_end   = PGROUNDUP((u64)etext);          // ★ 向上对齐页边界
  mappages(kernel_pagetable, text_start, text_end - text_start,
           text_start, PTE_R|PTE_X);

  // 数据段：从 text_end 起到 PHYSTOP，RW（避免与上一页重叠）
  if (PHYSTOP > text_end)
    mappages(kernel_pagetable, text_end, PHYSTOP - text_end,
             text_end, PTE_R|PTE_W);
}


// 切换到内核页表（写 satp + sfence）
void kvminithart(void) {
  u64 pa = (u64)kernel_pagetable;          // 目前恒等映射，VA==PA
  w_satp(MAKE_SATP(pa));
  sfence_vma();
}
