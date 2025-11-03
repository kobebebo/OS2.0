#include "defs.h"
#include "tests.h"

void main(void) {
  console_init();
  printf("Hello OS from riscv-os (Experiment 1)\n");

  /* 页表与 Sv39（Exp3） */
  printf("[INFO] Initializing physical page allocator (takes a moment) ...\n");
  kinit();
  kvminit();
  kvminithart();

  /* 实验四：陷阱/中断初始化（S-mode） */
  trapinit();       // 允许 S 级中断源
  trapinithart();   // stvec 指向 S 级中断入口
  intr_on();        // 开启 S 级全局中断

  printf("Boot OK. Running tests...\n");

  /* Exp1 */
  test_boot_uart();
  test_printf_basic();

  /* Exp2 */
  printf("[INFO] Running Experiment 2 tests...\n");
  test_printf_edge_cases();
  test_console_clear();

  /* Exp3 */
  printf("[INFO] Running Experiment 3 tests...\n");
  test_kalloc_basic();
  test_vm_alias_map();

  /* Exp4 */
  printf("[INFO] Running Experiment 4 tests...\n");
  test_timer_interrupt();
  //test_exception_illegal_insn();   // 需要时再取消注释：会停在异常打印处

  printf("All Exp1+2+3+4 tests finished.\n");
  for(;;) { }
}
