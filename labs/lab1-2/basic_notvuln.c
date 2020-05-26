#include <stdio.h>
int main(int argc, char **argv){
  char buf[64];
  // LEN-1 so that we don't write a null byte
  // past the bounds if n=sizeof(buf)
  strncpy(buf,argv[1],64-1);
}
