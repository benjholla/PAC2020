section .data
msg db 'Owned!!',0xa
section .text
global _start
_start:
 
;write(int fd, char *msg, unsigned int len)
mov eax,4
mov ebx,1
mov ecx,msg
mov edx,8
int 0x80
 
;exit(int ret)
mov eax,1
mov ebx,0
int 0x80
