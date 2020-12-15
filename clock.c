#include <stdio.h>
#include <stdbool.h>
#include <time.h>

int main() {
	int y = time(NULL);
	
	while (true) {
		int curr_time = time(NULL);
		
		if (y < curr_time) {
			y = curr_time;
			int x = curr_time;
			int s = x % 60;
			x -= s;
			x /= 60;
			int m = x % 60;
			x -= m;
			x /= 60;
			int h = x % 24;
			
			printf("time: %d\nh: %d\nm: %d\ns: %d\n", curr_time, h, m, s);
		}
	}
}