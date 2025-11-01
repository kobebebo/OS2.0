#pragma once
#include "types.h"
#include "memlayout.h"

static inline u64 r_mscratch(){ u64 x; asm volatile("csrr %0, mscratch":"=r"(x)); return x; }

/* ---------- 通用 CSR ---------- */
static inline u64 r_mstatus(){ u64 x; asm volatile("csrr %0, mstatus":"=r"(x)); return x; }
static inline void w_mstatus(u64 x){ asm volatile("csrw mstatus, %0"::"r"(x)); }

static inline u64 r_sstatus(){ u64 x; asm volatile("csrr %0, sstatus":"=r"(x)); return x; }
static inline void w_sstatus(u64 x){ asm volatile("csrw sstatus, %0"::"r"(x)); }

static inline u64 r_sie(){ u64 x; asm volatile("csrr %0, sie":"=r"(x)); return x; }
static inline void w_sie(u64 x){ asm volatile("csrw sie, %0"::"r"(x)); }

static inline u64 r_sip(){ u64 x; asm volatile("csrr %0, sip":"=r"(x)); return x; }
static inline void w_sip(u64 x){ asm volatile("csrw sip, %0"::"r"(x)); }

static inline u64 r_scause(){ u64 x; asm volatile("csrr %0, scause":"=r"(x)); return x; }
static inline u64 r_sepc(){ u64 x; asm volatile("csrr %0, sepc":"=r"(x)); return x; }
static inline void w_sepc(u64 x){ asm volatile("csrw sepc, %0"::"r"(x)); }
static inline u64 r_stval(){ u64 x; asm volatile("csrr %0, stval":"=r"(x)); return x; }

static inline void w_stvec(u64 x){ asm volatile("csrw stvec, %0"::"r"(x)); }
static inline void sfence_vma(){ asm volatile("sfence.vma zero, zero"); }

static inline void w_satp(u64 x){ asm volatile("csrw satp, %0"::"r"(x)); }

/* ---------- M-mode 相关（定时器/入口） ---------- */
static inline void w_mtvec(u64 x){ asm volatile("csrw mtvec, %0"::"r"(x)); }
static inline u64 r_mie(){ u64 x; asm volatile("csrr %0, mie":"=r"(x)); return x; }
static inline void w_mie(u64 x){ asm volatile("csrw mie, %0"::"r"(x)); }
static inline void w_mscratch(u64 x){ asm volatile("csrw mscratch, %0"::"r"(x)); }
static inline u64 r_mhartid(){ u64 x; asm volatile("csrr %0, mhartid":"=r"(x)); return x; }
static inline void mret(){ asm volatile("mret"); }

/* ---------- 本次缺的三位主角：PMP & MEPC ---------- */
/* PMP CSR：pmpaddr0/pmpcfg0（用于放开 S/U 的物理访问） */
static inline void w_pmpaddr0(u64 x){ asm volatile("csrw pmpaddr0, %0"::"r"(x)); }  // NEW
static inline void w_pmpcfg0(u64 x){ asm volatile("csrw pmpcfg0, %0"::"r"(x)); }    // NEW
/* MEPC：机器异常返回地址（M→S 跳 main 用） */
static inline void w_mepc(u64 x){ asm volatile("csrw mepc, %0"::"r"(x)); }          // NEW

/* ---------- 位定义 ---------- */
#define MSTATUS_MPP_MASK   (3ULL << 11)
#define MSTATUS_MPP_M      (3ULL << 11)
#define MSTATUS_MPP_S      (1ULL << 11)
#define MSTATUS_MPP_U      (0ULL << 11)
#define MSTATUS_MIE        (1UL << 3)
#define SSTATUS_SIE        (1UL << 1)

#define SIE_SSIE           (1UL << 1)
#define SIE_STIE           (1UL << 5)
#define SIE_SEIE           (1UL << 9)

#define MIE_MSIE           (1UL << 3)
#define MIE_MTIE           (1UL << 7)
#define MIE_MEIE           (1UL << 11)

/* ---------- Sv39 & PTE ---------- */
typedef u64 pte_t;
typedef u64* pagetable_t;

#define PTE_V (1ULL<<0)
#define PTE_R (1ULL<<1)
#define PTE_W (1ULL<<2)
#define PTE_X (1ULL<<3)
#define PTE_U (1ULL<<4)
#define PTE_G (1ULL<<5)
#define PTE_A (1ULL<<6)
#define PTE_D (1ULL<<7)
#define PTE_FLAGS(pte)   ((pte) & 0x3FFULL)

#define PA2PTE(pa)       ((((u64)(pa)) >> 12) << 10)
#define PTE2PA(pte)      ((((u64)(pte)) >> 10) << 12)

#define PXSHIFT(level)   (12 + 9 * (level))
#define PX(level, va)    ((((u64)(va)) >> PXSHIFT(level)) & 0x1FFULL)

/* satp: MODE=Sv39(8) | PPN */
#define SATP_SV39        (8ULL << 60)
#define MAKE_SATP(pgtbl_pa) (SATP_SV39 | (((u64)(pgtbl_pa)) >> 12))
