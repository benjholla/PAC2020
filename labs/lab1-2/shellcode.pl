#!/usr/bin/perl
print "\x31\xc0";             # xor eax,eax
print "\x31\xdb";             # xor ebx,ebx
print "\x31\xc9";             # xor ecx,ecx
print "\x31\xd2";             # xor edx,edx
print "\xb0\x04";             # mov al,0x4
print "\xb3\x01";             # mov bl,0x1
print "\x68\x64\x21\x21\x21"; # push 0x21212164
print "\x68\x4f\x77\x6e\x65"; # push 0x656e774f
print "\x89\xe1";             # mov ecx,esp
print "\xb2\x08";             # mov dl,0x8
print "\xcd\x80";             # int 0x80
print "\xb0\x01";             # mov al,0x1
print "\x31\xdb";             # xor ebx,ebx
print "\xcd\x80";             # int 0x80
