#include <stdio.h>
#include <stdbool.h>

const int WIDTH = 10;
const int HEIGHT = 10;

int main() {
	bool grid[WIDTH][HEIGHT];
	bool new_grid[WIDTH][HEIGHT];
	
	for (int i = 0; i < WIDTH; i++) {
		for (int j = 0; j < HEIGHT; j++) {
			grid[i][j] = false;
			new_grid[i][j] = false;
		}
	}
	
	int x = 0;
	
	while (true) {
		for (int i = 0; i < WIDTH; i++) {
			for (int j = HEIGHT - 1; j > 0; j--) {
				bool full = true;
					
				for (int k = j; k < HEIGHT; k++) {
					if (!grid[i][k]) {
						full = false;
						break;
					}
				}
				
				if (!full) {
					new_grid[i][j] = grid[i][j - 1];
				}
			}
			
			new_grid[i][0] = false;
		}
		
		for (int i = 0; i < WIDTH; i++) {
			for (int j = 0; j < HEIGHT; j++) {
				grid[i][j] = new_grid[i][j];
			}
		}
		
		grid[x % WIDTH][0] = true;
		
		for (int j = 0; j < WIDTH; j++) {
			for (int i = 0; i < HEIGHT; i++) {
				printf("%d ", grid[i][j]);
			}
			
			printf("\n");
		}
		
		x++;
		getchar();
	}
}