#include <stdio.h>
#include <string.h>

// simple case 2 * 5 = 10;
const long long c = 10;

// hard case
// a * b = c, where C is the product of two primes
// 104639 * 104729 = 10958737831
//const long long c = 10958737831LL;

int accepted() {
  printf("Welcome!\n");
}

int rejected() {
  printf("Go away!");
}

int main(int argc, char **argv) {
  int a;
  scanf("%d", &a);

  int b;
  scanf("%d", &b);

  printf("Asserting: %lld * %lld == %lld\n", (long long) a, (long long) b, c);

  if ((long long) a * (long long) b == c) {
    accepted();
  } else {
    rejected();
  }
}

