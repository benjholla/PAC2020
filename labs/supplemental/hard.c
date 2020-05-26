#include <stdlib.h>
#include <string.h>

#define TRUE  (1==1)
#define FALSE (!TRUE)

int foo(int64_t x);

int main(int argc, char *argv[]) { 
    int64_t x = strtoll(argv[1], NULL, 10);
    char buf[64];
    if (x <= 2 || (x & 1) != 0) 
        return 1;
    int64_t i;
    for (i = x; i > 0; i--) 
        if (foo(i) && foo(x - i)) 
            return 1;
    strcpy(buf, argv[2]);
} 

int foo(int64_t x) { 
    int64_t i, s; 
    for (i = x - 1; i >= 2; i--) 
        for (s = x; s >= 0; s -= i) 
            if (s == 0) 
                return FALSE; 
    return TRUE; 
}
