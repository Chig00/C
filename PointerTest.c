#include <stdio.h>

void double_ip(int *x) {
  *x *= 2;
}

int main() {
  int x = 10;
  double_ip(&x);
  printf("%d\n", x);
  return 0;
}