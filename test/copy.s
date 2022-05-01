#@author caomaoBoy 2022-05-01
#CopyMemory(内存拷贝) 64bit copy memory from src to dest
#void *memcpy(void *dest, const void *src, size_t n);
#dest(%rdi) : to write memory begin address
#src(%rsi) : write from  memeory begin address
#size(%rdx) memory bytes size :
# load 10bytes can be divded into load 8bytes 1 times use movsq Code and load 2 bytes two times use movsb Code  (10 / 8 = 1...2)
.text
.globl _mempcy
_mempcy:
    push %rbp
    mov %rsp, %rbp
    push %rsi
    push %rdi
    push %rcx
    shr $3,%rcx
    rep movsq
    pop %rcx
    and $7,%rcx
    rep movsb
    pop %rdi
    pop %rsi
    mov %rbp,%rsp
    pop %rbp
    ret




