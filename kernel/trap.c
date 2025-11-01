// kernel/trap.c
#include "defs.h"

volatile u64 ticks = 0;

/* 开启 S 态将使用的中断源：这里走 SSIP 路径 */
void trapinit(void) {
  w_sie(r_sie() | SIE_SSIE);
}

/* 设置 S 态陷阱入口 */
void trapinithart(void) {
  w_stvec((u64)kerneltrap_handler);   // C 函数，见下
}

/* 开/关 S 态全局中断（sstatus.SIE） */
void intr_on(void)  { w_sstatus(r_sstatus() |  SSTATUS_SIE); }
void intr_off(void) { w_sstatus(r_sstatus() & ~SSTATUS_SIE); }

/* 设备中断分发：
 * 返回 2=时钟类（此处为 SSIP），1=其他设备中断，0=非设备(异常)
 */
int devintr(void) {
  u64 sc = r_scause();

  /* 最高位=1 表示“中断”；低 12 位编码中断原因 */
  if (sc >> 63) {
    u64 code = sc & 0xfff;

    /* SSIP：Supervisor Software Interrupt
     * 机器态 timervec 把 MTI 转为 SSIP 后会到这里
     */
    if (code == 1) {
      ticks++;
      /* 处理完必须清 SSIP，否则会持续触发 */
      w_sip(r_sip() & ~2ULL);
      return 2;
    }

    /* 可按需扩展：STIP=5、SEIP=9 等 */
    return 1;
  }

  return 0;  // 非设备类：交由上层按异常处理
}

/* S 态陷阱入口：由 stvec 指向这里 */
void kerneltrap_handler(void) __attribute__((interrupt("supervisor")));
void kerneltrap_handler(void) {
  u64 scause = r_scause();
  u64 sepc   = r_sepc();
  u64 stval  = r_stval();

  int which = devintr();
  if (which == 0) {
    /* 非设备（异常）直接打印并停住，便于定位 */
    printf("kerneltrap: scause=0x%lx sepc=0x%lx stval=0x%lx\n",
           (unsigned long)scause, (unsigned long)sepc, (unsigned long)stval);
    for(;;) { }
  }
  /* 设备中断处理完直接返回，编译器会生成 sret */
}
