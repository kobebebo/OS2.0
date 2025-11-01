OUTPUT_ARCH(riscv)
ENTRY(_entry)

SECTIONS
{
  . = 0x80000000;

  .text : {
    *(.text.entry)
    *(.text .text.*)
  }
  PROVIDE(etext = .);

  .rodata : {
    *(.rodata .rodata.*)
  }

  .data : {
    *(.sdata .sdata.* .data .data.*)
  }
  PROVIDE(edata = .);

  .bss : {
    *(.sbss .sbss.* .bss .bss.* COMMON)
  }
  PROVIDE(end = .);
}
