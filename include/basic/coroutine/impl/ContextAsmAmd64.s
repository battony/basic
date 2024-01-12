.global     MakeContext
.global     SwitchContext
.text

MakeContext:
    movq (%rdi), %rax

    andq $~0xF, %rax
    subq $0x40, %rax
    movq %rsi, 0x30(%rax)

    movq %rax, (%rdi)

    ret

SwitchContext:
    subq $0x30, %rsp

    movq %r12, 0x00(%rsp)
    movq %r13, 0x08(%rsp)
    movq %r14, 0x10(%rsp)
    movq %r15, 0x18(%rsp)
    movq %rbp, 0x20(%rsp)
    movq %rbx, 0x28(%rsp)

    movq %rsp, (%rdi)

    movq (%rsi), %rsp

    movq 0x00(%rsp), %r12
    movq 0x08(%rsp), %r13
    movq 0x10(%rsp), %r14
    movq 0x18(%rsp), %r15
    movq 0x20(%rsp), %rbp
    movq 0x28(%rsp), %rbx

    addq $0x30, %rsp

    ret
