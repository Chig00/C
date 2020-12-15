#include <stdio.h>
#include <time.h>

void pause() {
  int base = time(NULL);
  while (base == time(NULL)) {}
}

int main() {
  int timer;
  printf("Timer duration: ");
  scanf("%d", &timer);
  
  for (timer; timer >= 0; timer--) {
    printf("%d\n", timer);
    pause();
  }
  
  return 0;
}