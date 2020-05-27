#include <stdio.h>

void print_flag() {
  printf("flag{program_analysis_is_fun}");
}

int main(int argc, char *argv[]){
  int passwd;
  printf("Enter Password: ");
  scanf("%d", &passwd);
  if (passwd == 1234){
    print_flag();
  } else {
    printf("Invalid Password!\n");
  }
  return 0;
}
