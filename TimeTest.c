#include <stdio.h>
#include <time.h>

int main() {
  int base_time = time(NULL);
  int time_count = base_time;
  
  while (1) {
    if (time(NULL) >= time_count + 1) {
      time_count = time(NULL);
      printf("%d\n", time_count - base_time);
    }
  }
  
  return 0;
}