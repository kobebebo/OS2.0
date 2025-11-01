# riscv-os / Makefile
CROSS      ?= riscv64-unknown-elf-
CC         := $(CROSS)gcc
LD         := $(CROSS)ld
OBJCOPY    := $(CROSS)objcopy

CFLAGS := -Wall -Wextra -O2 -g3 -ffreestanding -fno-builtin -nostdlib -nostartfiles \
          -fno-omit-frame-pointer -mcmodel=medany -march=rv64gc -mabi=lp64 \
          -Iinclude
LDFLAGS := -T linker.ld -nostdlib -z max-page-size=4096

KSRCS = kernel/start.c kernel/main.c kernel/console.c kernel/uart.c kernel/printf.c kernel/string.c\
		kernel/kalloc.c kernel/vm.c kernel/trap.c kernel/timervec.c
ASRCS = kernel/entry.S
OBJS  = $(KSRCS:.c=.o) $(ASRCS:.S=.o)

all: kernel.elf

kernel.elf: $(OBJS) linker.ld
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(CC) $(CFLAGS) -D__ASSEMBLY__ -c $< -o $@


qemu: kernel.elf
	qemu-system-riscv64 -machine virt -bios none -kernel kernel.elf \
		-m 128M -smp 1 -nographic

clean:
	rm -f $(OBJS) kernel.elf
.PHONY: all qemu clean
