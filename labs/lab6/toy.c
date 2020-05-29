#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
  char buf[100];
  size_t size = read(0, buf, 100);

  if (size > 0 && buf[0] == 'h'){
    if (size > 1 && buf[1] == 'a'){
      if (size > 2 && buf[2] == 'c'){
        if (size > 3 && buf[3] == 'k'){
          // __builtin_trap(); // uncomment to force a crash at this program point
          printf(buf); // vulnerable to format string attacks!
        }
      }
    }
  }

  return 0;
}

