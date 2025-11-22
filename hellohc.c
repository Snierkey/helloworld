long _start(){
    asm volatile(
        "mov $1,%%rax\n"
        "mov $1,%%rdi\n"
        "mov $msg,%%rsi\n"
        "mov $13,%%rdx\n"
        "syscall\n"
        "mov $60,%%rax\n"
        "xor %%rdi,%%rdi\n"
        "syscall\n"
        :
        :
        : "rax","rdi","rsi","rdx"
    );
    return 0;
}

char msg[13] = "hello, world\n";
