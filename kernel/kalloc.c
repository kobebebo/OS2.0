// riscv-os / kernel/kalloc.c
#include "defs.h"
#include "memlayout.h"

struct run { struct run *next; };

static struct {
  struct run *freelist;
} kmem;

// 由链接脚本导出（end = 内核镜像末尾）
extern char end[];

static void freerange(void *pa_start, void *pa_end) {
  u8 *p = (u8*)PGROUNDUP((u64)pa_start);
  for (; p + PGSIZE <= (u8*)pa_end; p += PGSIZE)
    kfree(p);
}

void kinit(void) {
  // 释放 [end, PHYSTOP) 区间的物理页到空闲链表（与 xv6 思路一致）
  freerange(end, (void*)PHYSTOP);
}

void kfree(void *pa) {
  if (pa == 0) return;
  if ((u64)pa % PGSIZE) return;
  if ((u64)pa < KERN_BASE || (u64)pa >= PHYSTOP) return;

  // 用已知模式填充，便于调试 UAF
  memset(pa, 1, PGSIZE);

  struct run *r = (struct run*)pa;
  r->next = kmem.freelist;
  kmem.freelist = r;
}

void* kalloc(void) {
  struct run *r = kmem.freelist;
  if (r) {
    kmem.freelist = r->next;
    memset((void*)r, 0, PGSIZE);
    return (void*)r;
  }
  return 0;
}
