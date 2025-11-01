.globl timervec
.align 4
timervec:
    # 取出scratch指针（当前CPU的mscratch0地址）:contentReference[oaicite:11]{index=11}
    csrr    t0, mscratch

    # 保存调用者寄存器a0–a3到scratch[0..3]
    sd      a0, 0(t0)
    sd      a1, 8(t0)
    sd      a2, 16(t0)
    sd      a3, 24(t0)

    # 加载CLINT计时器寄存器地址和间隔
    ld      t1, 32(t0)        # t1 = CLINT_MTIMECMP地址（scratch[4]）
    ld      t2, 40(t0)        # t2 = 间隔值（cycles，scratch[5]）
    # 更新下一次触发时间：mtimecmp += interval
    ld      t3, 0(t1)         # t3 = 当前mtimecmp值
    add     t3, t3, t2        # t3 = t3 + interval
    sd      t3, 0(t1)         # 写回mtimecmp寄存器，安排下次时钟中断

    # 设置S态软件中断挂起位SSIP，向S态发送中断:contentReference[oaicite:12]{index=12}:contentReference[oaicite:13]{index=13}
    csrs    mip, 2            # mip寄存器的第1位对应SSIP = 1

    # 恢复保存的寄存器并返回
    ld      a3, 24(t0)
    ld      a2, 16(t0)
    ld      a1, 8(t0)
    ld      a0, 0(t0)
    mret                       # 返回之前的S/U态，让S态trap处理刚才触发的SSIP
