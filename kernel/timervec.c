// kernel/timervec.c
#include "defs.h"

/* 机器态中断入口：
 * - 写 mtimecmp 预约下一次；
 * - 置位 SSIP，把“软件中断”交给 S 模式处理；
 */
void timervec(void) __attribute__((interrupt("machine")));
void timervec(void) {
  u64 *scratch = (u64*)( (uintptr_t) r_mscratch() );  // ← 先转 uintptr_t 再转指针
  volatile u64 *mtimecmp = (volatile u64*)(uintptr_t)scratch[4];
u64 interval = scratch[5];
volatile u64 *mtime = (volatile u64*)(uintptr_t)CLINT_MTIME;

*mtimecmp = *mtime + interval;
/* 把时钟“转交”给 S 态：置位 SSIP */
asm volatile("csrs mip, %0" :: "r"(1UL<<1));   // 置 mip.SSIP → 触发 S-mode 软件中断
}

