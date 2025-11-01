/* riscv-os / kernel/entry.S */
.section .text.entry
.globl _entry
_entry:
  /* 为引导核设置栈指针（一个简单的单核栈）。*/
  la   sp, stack_top
  call start         /* 进入 C 的机器态引导代码 */

  /* should never return */
1: j 1b

/* 简单的 8 KiB 启动栈 */
.section .bss
.align 16
.globl stack
stack:
  .skip 8192
.globl stack_top
stack_top:
