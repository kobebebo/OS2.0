// kernel/start.c
#include "defs.h"

/* 声明 main 作为 S 态入口 */
extern void main(void);

/* 每核给 M 态 timervec 用的临时区域（见 xv6 设计，含保存区、mtimecmp 地址与间隔） */
static u64 mscratch0[32 * 8] __attribute__((aligned(16))); // 最多 8 个核足够了

void timerinit() 
{
    int id = r_mhartid();
    int interval = 1000000;  // 触发间隔（周期），QEMU下约为0.1秒

    // 设置CLINT，安排首次计时器中断:contentReference[oaicite:8]{index=8}
    *(uint_64*)CLINT_MTIMECMP(id) = *(uint_64*)CLINT_MTIME + interval;

    // 配置mscratch指向的scratch信息：
    // scratch[0..3]用于保存a0–a3，scratch[4]放CLINT_MTIMECMP地址，scratch[5]放计时器间隔
    uint_64 *scratch = &mscratch0[32 * id];
    scratch[4] = CLINT_MTIMECMP(id);
    scratch[5] = interval;
    w_mscratch((uint_64)scratch);

    // 设置机器态中断向量入口为timervec
    w_mtvec((uint_64)timervec);
    // 开启机器态计时器中断和全局中断使能
    w_mie(r_mie() | MIE_MTIE);
    w_mstatus(r_mstatus() | MSTATUS_MIE);
}


void start(void) {
  /* 允许 S 态访问所有物理地址（PMP） */
  w_pmpaddr0(~0ULL >> 10);
  w_pmpcfg0(0x0f);

  /* 关闭分页 */
  w_satp(0);

  /* M → S 切换准备 */
  u64 m = r_mstatus();
  m &= ~MSTATUS_MPP_MASK;
  m |=  MSTATUS_MPP_S;
  w_mstatus(m);

  w_mepc((u64)main);

  /* 初始化机器定时器中断（MTI），交由 timervec 处理并转发为 SSIP） */
  timerinit();

  mret();  /* 进入 S 态，从 main() 开始执行 */
}
