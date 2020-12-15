#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
	char* name;
	char* desc;
	int max_hp;
	int hp;
	int attack;
	int speed;
	int charge;
	int cost;
	char* attack_name;
	char* special_name;
	char* special_desc;
} Warrior;

const int WARRIOR_COUNT = 6;

const char* _NAME = "";
const char* _DESC = "";
const int _MAX_HP = ;
const int _ATTACK = ;
const int _SPEED = ;
const int _COST  = ;
const char* _ATTACK_NAME = "";
const char* _SPECIAL_NAME = "";
const char* _SPECIAL_NAME = "";

int main() {
	srand(time(NULL));

	Warrior warriors[6];

	while (true) {
		int choices[6];

		for (int i = 0; i < 6; i++) {
			bool clone = true;

			while (clone) {
				clone = false;

				choices[i] = WARRIOR_COUNT * rand() / (RAND_MAX + 1);

				for (int j = 0; j < i; j++) {
					if (choices[j] == choices[i]) {
						clone = true;
						break;
					}
				}
			}

			if (choices[i] == 0) {
				warriors[i].name = _NAME;
				warriors[i].desc = _DESC;
				warriors[i].max_hp = _MAX_HP;
				warriors[i].hp = warriors[i].max_hp;
				warriors[i].attack = _ATTACK;
				warriors[i].speed = _SPEED;
				warriors[i].charge = 0;
				warriors[i].cost = _COST;
				warriors[i].attack_name = _ATTACK_NAME;
				warriors[i].special_name = _SPECIAL_NAME;
				warriors[i].special_desc = _SPECIAL_DESC;
			}

			else if (choices[i] == 1) {
			}

			else if (choices[i] == 2) {
			}

			else if (choices[i] == 3) {
			}

			else if (choices[i] == 4) {
			}

			else if (choices[i] == 5) {
			}
		}

		printf("You summoned %s, %s, and %s!\n", warriors[0].name, warriors[2].name, warriors[4].name);
		printf("Your opponent summoned %s, %s, and %s!\n", warriors[1].name, warriors[3].name, warriors[5].name);

		bool winner;

		while (true) {
			int order[6];

			for (int i = 0; i < 6; i++) {
				int max_speed = 0;

				for (int j = 0; j < 6; j++) {
					bool clone = false;

					for (int k = 0; k < i; k++) {
						if (j == order[k]) {
							clone = true;
							break;
						}
					}

					if (clone) {
						continue;
					}

					if (warriors[j].speed > max_speed) {
						max_speed = warriors[j].speed;
					}
				}

				for (int j = 0; j < 6; j++) {
					bool clone = false;

					for (int k = 0; k < i; k++) {
						if (j == order[k]) {
							clone = true;
							break;
						}
					}

					if (clone) {
						continue;
					}

					if (warriors[j].speed == max_speed) {
						order[i] = j;
					}
				}
			}

			for (int i = 0; i < 6; i++) {
				if (warriors[order[i]].alive) {
					printf("It's %s's turn!\n", warriors[order[i]]);

					if (order[i] % 2) {
						
					}

					else {
						while (true) {
							printf("Attack (a), special (s), overview (o), or check (c)? ");
							char option;
							scanf("%c", &option);

							if (option == 'a') {
								
							}

							else if (option == 's') {
								
							}

							else if (option == 'o') {
								
							}

							else if (option == 'c') {
								
							}

							else {
								printf("Invalid input.\n");
							}
						}
					}
				}
			}

			for (int i = 0; i < 6; i++) {
				if (warriors[i].hp < 0 && warriors[i].alive) {
					printf("%s has been defeated!\n", warriors[i].name);
					warriors[i]alive = false;
				}
			}

			if !(warriors[0].alive + warriors[2].alive + warriors[4].alive) {
				winner = false;
				break;
			}

			else if !(warriors[1].alive + warriors[3].alive + warriors[5].alive) {
				winner = true;
				break;
			}
		}

		if (winner) {
			printf("You win!\n");
		}

		else {
			printf("You lose...\n");
		}
	}
}