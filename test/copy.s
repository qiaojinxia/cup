#@author qiaojinxia 2022-05-01
#CopyMemory(内存拷贝) 64bit copy memory from src to dest
#void *memcpy(void *dest, const void *src, size_t n);
#dest(%rdi) : to write memory begin address
#src(%rsi) : write from  memeory begin address
#size(%rdx): memory bytes size
# load 10bytes can be divded into load 8bytes 1 times use movsq Code and load 2 bytes two times use movsb Code  (10 / 8 = 1...2)
.text
.globl _mempcy
_mempcy:
    push %rbp
    mov %rsp, %rbp
    push %rsi
    mov %rax,%rsi
    push %rcx
    shr $3,%rcx
    rep movsq
    pop %rcx
    and $7,%rcx
    rep movsb
    pop %rsi
    mov %rbp,%rsp
    pop %rbp
    ret
#@author qiaojinxia 2022-05-02
#Memset(内存设置初始值) set default value c to fill n bytes continuous memory
#void *(memset)(void *s, int c, size_t n)
#dest(%rdi) : the begin address to fill n bytes
#content(%eax) : fill memory use reg eax low 8bit
#n (%ecx): fill memory n bytes size
# fill memory  8 bytes  every time  if surplus fill size less than 8 bytes fill one bytes every times  with %rax low 8 bit
.globl _memset
_memset:
    push %rbp
    mov %rsp, %rbp
    push %rax
    push %rcx
    mov %rdx,%rcx
    movzbq %sil,%rsi
    mov $0x0101010101010101,%rax
    imul %rsi,%rax
    push %rcx
    shr $3,%rcx
    rep stosq
    pop %rcx
    and $7,%rcx
    rep stosb
    pop %rcx
    pop %rax
    mov %rbp,%rsp
    pop %rbp
    ret





