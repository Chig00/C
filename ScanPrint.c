#include <stdio.h>

int main() {
  char character[64];
  printf("Character: ");
  scanf("%s", &character);
  printf("Your character is %s\n", character);
  return 0;
}