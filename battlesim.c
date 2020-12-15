#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

const int DEF_RUNS = 100000;
const char DEF_DEBUG = 'n';
const double TOLERANCE = 0.025;
const int MAX_WARR = 6;
const int TOT_WARR = 16;
const char* ALL_WARR[] = {
	"Knight",
	"Huntress",
	"Wizard",
	"Assassin",
	"Mage",
	"Soldier",
	"Ninja",
	"Samurai",
	"Guardian",
	"Sniper",
	"Arcanist",
	"Thief",
	"Psychic",
	"Reaper",
	"Fairy",
	"Babarian"
};
const int STATS[][4] = {
	{1000, 100, 100, 3}, // Knight
	{800, 115, 120, 5},  // Huntress
	{750, 125, 125, 3},  // Wizard
	{650, 80, 150, 1},   // Assassin
	{600, 200, 75, 3},   // Mage
	{775, 155, 60, 1},   // Soldier
	{950, 95, 190, 5},   // Ninja
	{950, 100, 100, 3},  // Samurai
	{850, 80, 85, 5},    // Guardian
	{550, 300, 60, 1},   // Sniper
	{950, 115, 55, 7},   // Arcanist
	{675, 0, 150, 0},    // Thief
	{650, 40, 40, 3},    // Psychic
	{825, 45, 0, 5},     // Reaper
	{475, 160, 135, 1},  // Fairy
	{975, 245, 85, 0}    // Barbarian
};

typedef struct {
	const char* name;
	int mhp;
	int hp;
	int atk;
	int spd;
	int cost;
	int chrg;
	bool alive;
	int team;
} Warrior;

int rand_int(int min, int max) {
	return min + (max - min + 1) * rand() / (RAND_MAX + 1);
}

void summon(Warrior* warrior, int code, int team) {
	warrior->hp = STATS[code][0];
	warrior->atk = STATS[code][1];
	warrior->spd = STATS[code][2];
	warrior->cost = STATS[code][3];
	warrior->name = ALL_WARR[code];
	warrior->mhp = warrior->hp;
	warrior->chrg = 0;
	warrior->alive = true;
	warrior->team = team;
}

void order(Warrior warriors[]) {
	for (int i = 0; i < MAX_WARR - 1; i++) {
		int j;
		Warrior tmp = warriors[i];
		warriors[i] = warriors[j = rand_int(i, MAX_WARR - 1)];
		warriors[j] = tmp;
	}
	
	for (int i = MAX_WARR - 1; i > 0; i--) {
		for (int j = 0; j < i; j++) {
			if (warriors[j].spd < warriors[j + 1].spd) {
				Warrior tmp = warriors[j];
				warriors[j] = warriors[j + 1];
				warriors[j + 1] = tmp;
			}
		}
	}
}

void special(Warrior warriors[], int index, char debug) {
	if (debug == 'y') {
		printf("%s on team %d used their special!\n", warriors[index].name, warriors[index].team);
	}
	
	// Knight
	if (!strcmp(warriors[index].name, ALL_WARR[0])) {
		int highest_hp = 0;
	
		for (int i = 0; i < MAX_WARR; i++) {
			if (warriors[i].team != warriors[index].team && warriors[i].alive) {
				if (warriors[i].hp > highest_hp) {
					highest_hp = warriors[i].hp;
				}
			}
		}
		
		int target;
		
		for (target = 0; target < MAX_WARR; target++) {
			if (warriors[target].team != warriors[index].team && warriors[target].alive) {
				if (warriors[target].hp == highest_hp) {
					break;
				}
			}
		}
		
		int dmg = rand_int(0, 3.65 * warriors[index].atk);
		warriors[target].hp -= dmg;
		warriors[target].chrg++;
		
		if (debug == 'y') {
			printf(
				"%s on team %d dealt %d damage to %s on team %d!\n",
				warriors[index].name, warriors[index].team, dmg, warriors[target].name, warriors[target].team
			);
		}
	}
	
	// Huntress
	else if (!strcmp(warriors[index].name, ALL_WARR[1])) {
		int highest_hp = 0;
	
		for (int i = 0; i < MAX_WARR; i++) {
			if (warriors[i].team != warriors[index].team && warriors[i].alive) {
				if (warriors[i].hp > highest_hp) {
					highest_hp = warriors[i].hp;
				}
			}
		}
		
		int target;
		
		for (target = 0; target < MAX_WARR; target++) {
			if (warriors[target].team != warriors[index].team && warriors[target].alive) {
				if (warriors[target].hp == highest_hp) {
					break;
				}
			}
		}
		
		int dmg = rand_int(0, 5.55 * warriors[index].atk);
		warriors[target].hp -= dmg;
		warriors[target].chrg++;
		
		if (debug == 'y') {
			printf(
				"%s on team %d dealt %d damage to %s on team %d!\n",
				warriors[index].name, warriors[index].team, dmg, warriors[target].name, warriors[target].team
			);
		}
	}
	
	// Wizard
	else if (!strcmp(warriors[index].name, ALL_WARR[2])) {
		for (int i = 0; i < MAX_WARR; i++) {
			if (warriors[i].team != warriors[index].team && warriors[i].alive) {
				int dmg = rand_int(0, 1.55 * warriors[index].atk);
				warriors[i].hp -= dmg;
				warriors[i].chrg++;
				
				if (debug == 'y') {
					printf(
						"%s on team %d dealt %d damage to %s on team %d!\n",
						warriors[index].name, warriors[index].team, dmg, warriors[i].name, warriors[i].team
					);
				}
			}
		}
	}
	
	// Assassin
	else if (!strcmp(warriors[index].name, ALL_WARR[3])) {
		int lowest_hp = 9999;
	
		for (int i = 0; i < MAX_WARR; i++) {
			if (warriors[i].team != warriors[index].team && warriors[i].alive) {
				if (warriors[i].hp < lowest_hp) {
					lowest_hp = warriors[i].hp;
				}
			}
		}
		
		int target;
		
		for (target = 0; target < MAX_WARR; target++) {
			if (warriors[target].team != warriors[index].team && warriors[target].alive) {
				if (warriors[target].hp == lowest_hp) {
					break;
				}
			}
		}
		
		int dmg = rand_int(0, 3.48 * warriors[index].atk);
		warriors[target].hp -= dmg;
		warriors[target].chrg++;
		
		if (debug == 'y') {
			printf(
				"%s on team %d dealt %d damage to %s on team %d!\n",
				warriors[index].name, warriors[index].team, dmg, warriors[target].name, warriors[target].team
			);
		}
	}
	
	// Mage
	else if (!strcmp(warriors[index].name, ALL_WARR[4])) {
		int highest_hp = 0;
	
		for (int i = 0; i < MAX_WARR; i++) {
			if (warriors[i].team != warriors[index].team && warriors[i].alive) {
				if (warriors[i].hp > highest_hp) {
					highest_hp = warriors[i].hp;
				}
			}
		}
		
		int target;
		
		for (target = 0; target < MAX_WARR; target++) {
			if (warriors[target].team != warriors[index].team && warriors[target].alive) {
				if (warriors[target].hp == highest_hp) {
					break;
				}
			}
		}
		
		int dmg = rand_int(0, 2.75 * warriors[index].atk);
		warriors[target].hp -= dmg;
		warriors[target].chrg++;
		
		if (debug == 'y') {
			printf(
				"%s on team %d dealt %d damage to %s on team %d!\n",
				warriors[index].name, warriors[index].team, dmg, warriors[target].name, warriors[target].team
			);
		}
	}
	
	// Soldier
	else if (!strcmp(warriors[index].name, ALL_WARR[5])) {
		for (int i = 0; i < MAX_WARR; i++) {
			if (warriors[i].team != warriors[index].team && warriors[i].alive) {
				int dmg = rand_int(0, 0.875 * warriors[index].atk);
				warriors[i].hp -= dmg;
				warriors[i].chrg++;
				
				if (debug == 'y') {
					printf(
						"%s on team %d dealt %d damage to %s on team %d!\n",
						warriors[index].name, warriors[index].team, dmg, warriors[i].name, warriors[i].team
					);
				}
			}
		}
	}
	
	// Ninja
	else if (!strcmp(warriors[index].name, ALL_WARR[6])) {
		int lowest_hp = 9999;
	
		for (int i = 0; i < MAX_WARR; i++) {
			if (warriors[i].team != warriors[index].team && warriors[i].alive) {
				if (warriors[i].hp < lowest_hp) {
					lowest_hp = warriors[i].hp;
				}
			}
		}
		
		int target;
		
		for (target = 0; target < MAX_WARR; target++) {
			if (warriors[target].team != warriors[index].team && warriors[target].alive) {
				if (warriors[target].hp == lowest_hp) {
					break;
				}
			}
		}
		
		int dmg = rand_int(0, 4.62 * warriors[index].atk);
		warriors[target].hp -= dmg;
		warriors[target].chrg++;
		
		if (debug == 'y') {
			printf(
				"%s on team %d dealt %d damage to %s on team %d!\n",
				warriors[index].name, warriors[index].team, dmg, warriors[target].name, warriors[target].team
			);
		}
	}
	
	// Samurai
	else if (!strcmp(warriors[index].name, ALL_WARR[7])) {
		int highest_hp = 0;
	
		for (int i = 0; i < MAX_WARR; i++) {
			if (warriors[i].team != warriors[index].team && warriors[i].alive) {
				if (warriors[i].hp > highest_hp) {
					highest_hp = warriors[i].hp;
				}
			}
		}
		
		int target;
		
		for (target = 0; target < MAX_WARR; target++) {
			if (warriors[target].team != warriors[index].team && warriors[target].alive) {
				if (warriors[target].hp == highest_hp) {
					break;
				}
			}
		}
		
		int dmg = rand_int(0, warriors[index].atk + 0.68 * (warriors[index].mhp -  warriors[index].hp));
		warriors[target].hp -= dmg;
		warriors[target].chrg++;
		
		if (debug == 'y') {
			printf(
				"%s on team %d dealt %d damage to %s on team %d!\n",
				warriors[index].name, warriors[index].team, dmg, warriors[target].name, warriors[target].team
			);
		}
	}
	
	// Guardian
	else if (!strcmp(warriors[index].name, ALL_WARR[8])) {
		for (int i = 0; i < MAX_WARR; i++) {
			if (warriors[i].team == warriors[index].team && warriors[i].alive) {
				int abst = rand_int(0, 0.8 * warriors[i].atk);
				int sbst = rand_int(0, 1.1 * warriors[i].spd);
				warriors[i].atk += abst;
				warriors[i].spd += sbst;
				
				if (debug == 'y') {
					printf(
						"%s on team %d boosted %s on team %d's power by %d and speed by %d!\n",
						warriors[index].name, warriors[index].team, warriors[i].name, warriors[i].team, abst, sbst
					);
				}
			}
		}
		
		int highest_hp = 0;
	
		for (int i = 0; i < MAX_WARR; i++) {
			if (warriors[i].team != warriors[index].team && warriors[i].alive) {
				if (warriors[i].hp > highest_hp) {
					highest_hp = warriors[i].hp;
				}
			}
		}
		
		int target;
		
		for (target = 0; target < MAX_WARR; target++) {
			if (warriors[target].team != warriors[index].team && warriors[target].alive) {
				if (warriors[target].hp == highest_hp) {
					break;
				}
			}
		}
		
		int dmg = rand_int(0, warriors[index].atk);
		warriors[target].hp -= dmg;
		warriors[target].chrg++;
		
		if (debug == 'y') {
			printf(
				"%s on team %d dealt %d damage to %s on team %d!\n",
				warriors[index].name, warriors[index].team, dmg, warriors[target].name, warriors[target].team
			);
		}
	}
	
	// Sniper
	else if (!strcmp(warriors[index].name, ALL_WARR[9])) {
		int lowest_hp = 9999;
	
		for (int i = 0; i < MAX_WARR; i++) {
			if (warriors[i].team != warriors[index].team && warriors[i].alive) {
				if (warriors[i].hp < lowest_hp) {
					lowest_hp = warriors[i].hp;
				}
			}
		}
		
		int target;
		
		for (target = 0; target < MAX_WARR; target++) {
			if (warriors[target].team != warriors[index].team && warriors[target].alive) {
				if (warriors[target].hp == lowest_hp) {
					break;
				}
			}
		}
		
		int dmg = rand_int(0, warriors[index].atk);
		warriors[target].hp -= dmg;
		warriors[target].chrg++;
		
		if (debug == 'y') {
			printf(
				"%s on team %d dealt %d damage to %s on team %d!\n",
				warriors[index].name, warriors[index].team, dmg, warriors[target].name, warriors[target].team
			);
		}
	}
	
	// Arcanist
	else if (!strcmp(warriors[index].name, ALL_WARR[10])) {
		for (int i = 0; i < MAX_WARR; i++) {
			if (warriors[i].team != warriors[index].team && warriors[i].alive) {
				int dmg = rand_int(0, 2.1 * warriors[index].atk);
				warriors[i].hp -= dmg;
				warriors[i].chrg++;
				
				if (debug == 'y') {
					printf(
						"%s on team %d dealt %d damage to %s on team %d!\n",
						warriors[index].name, warriors[index].team, dmg, warriors[i].name, warriors[i].team
					);
				}
			}
		}
	}
	
	// Thief
	else if (!strcmp(warriors[index].name, ALL_WARR[11])) {
		int highest_hp = 0;
	
		for (int i = 0; i < MAX_WARR; i++) {
			if (warriors[i].team != warriors[index].team && warriors[i].alive) {
				if (warriors[i].hp > highest_hp) {
					highest_hp = warriors[i].hp;
				}
			}
		}
		
		int target;
		
		for (target = 0; target < MAX_WARR; target++) {
			if (warriors[target].team != warriors[index].team && warriors[target].alive) {
				if (warriors[target].hp == highest_hp) {
					break;
				}
			}
		}
		
		int dmg = rand_int(0, warriors[index].atk);
		warriors[target].hp -= dmg;
		warriors[target].chrg++;
		warriors[index].atk += warriors[target].atk;
		
		if (debug == 'y') {
			printf(
				"%s on team %d dealt %d damage to %s on team %d!\n",
				warriors[index].name, warriors[index].team, dmg, warriors[target].name, warriors[target].team
			);
		}
	}
	
	// Psychic
	else if (!strcmp(warriors[index].name, ALL_WARR[12])) {
		for (int i = 0; i < MAX_WARR; i++) {
			if (warriors[i].team != warriors[index].team && warriors[i].alive) {
				int dmg = rand_int(0, 2 * warriors[i].atk);
				warriors[i].hp -= dmg;
				warriors[i].chrg++;
				
				if (debug == 'y') {
					printf(
						"%s on team %d dealt %d damage to %s on team %d!\n",
						warriors[index].name, warriors[index].team, dmg, warriors[i].name, warriors[i].team
					);
				}
			}
		}
	}
	
	// Reaper
	else if (!strcmp(warriors[index].name, ALL_WARR[13])) {
		for (int i = 0; i < MAX_WARR; i++) {
			if (warriors[i].team != warriors[index].team && warriors[i].alive) {
				int dmg = rand_int(0, warriors[i].mhp - warriors[i].hp);
				warriors[i].hp -= dmg;
				warriors[i].chrg++;
				
				if (debug == 'y') {
					printf(
						"%s on team %d dealt %d damage to %s on team %d!\n",
						warriors[index].name, warriors[index].team, dmg, warriors[i].name, warriors[i].team
					);
				}
			}
		}
	}
	
	// Fairy
	else if (!strcmp(warriors[index].name, ALL_WARR[14])) {
		for (int i = 0; i < MAX_WARR; i++) {
			if (warriors[i].team != warriors[index].team && warriors[i].alive) {
				int dmg = rand_int(0, 0.55 * warriors[index].atk);
				warriors[i].hp -= dmg;
				
				if (debug == 'y') {
					printf(
						"%s on team %d dealt %d damage to %s on team %d!\n",
						warriors[index].name, warriors[index].team, dmg, warriors[i].name, warriors[i].team
					);
				}
			}
			
			else if (warriors[i].team == warriors[index].team && warriors[i].alive && i != index) {
				int bst = rand_int(0, 2 * warriors[i].cost);
				warriors[i].chrg += bst;
				
				if (debug == 'y') {
					printf(
						"%s on team %d charged %d charge for %s on team %d!\n",
						warriors[index].name, warriors[index].team, bst, warriors[i].name, warriors[i].team
					);
				}
			}
		}
	}
	
	// Barbarian
	else if (!strcmp(warriors[index].name, ALL_WARR[15])) {
		for (int i = 0; i < MAX_WARR; i++) {
			if (warriors[i].team == warriors[index].team && warriors[i].alive) {
				int boost = rand_int(0, 0.1 * warriors[i].spd);
				warriors[i].spd += boost;
				
				if (debug == 'y') {
					printf(
						"%s on team %d boosted %s on team %d's speed by %d!\n",
						warriors[index].name, warriors[index].team, warriors[i].name, warriors[i].team, boost
					);
				}
			}
		}
		
		int highest_hp = 0;
	
		for (int i = 0; i < MAX_WARR; i++) {
			if (warriors[i].team != warriors[index].team && warriors[i].alive) {
				if (warriors[i].hp > highest_hp) {
					highest_hp = warriors[i].hp;
				}
			}
		}
		
		int target;
		
		for (target = 0; target < MAX_WARR; target++) {
			if (warriors[target].team != warriors[index].team && warriors[target].alive) {
				if (warriors[target].hp == highest_hp) {
					break;
				}
			}
		}
		
		int dmg = rand_int(0, warriors[index].atk);
		warriors[target].hp -= dmg;
		warriors[target].chrg++;
		
		if (debug == 'y') {
			printf(
				"%s on team %d dealt %d damage to %s on team %d!\n",
				warriors[index].name, warriors[index].team, dmg, warriors[target].name, warriors[target].team
			);
		}
	}
	
	if (debug == 'y') {
		printf("\n");
	}
	
	warriors[index].chrg = 0;
}

void attack(Warrior warriors[], int index, char debug) {
	int highest_hp = 0;
	
	for (int i = 0; i < MAX_WARR; i++) {
		if (warriors[i].team != warriors[index].team && warriors[i].alive) {
			if (warriors[i].hp > highest_hp) {
				highest_hp = warriors[i].hp;
			}
		}
	}
	
	int target;
	
	for (target = 0; target < MAX_WARR; target++) {
		if (warriors[target].team != warriors[index].team && warriors[target].alive) {
			if (warriors[target].hp == highest_hp) {
				break;
			}
		}
	}
	
	int dmg = rand_int(0, warriors[index].atk);
	warriors[target].hp -= dmg;
	warriors[target].chrg++;
	warriors[index].chrg++;
	
	if (debug == 'y') {
		printf(
			"%s on team %d dealt %d damage to %s on team %d!\n\n",
			warriors[index].name, warriors[index].team, dmg, warriors[target].name, warriors[target].team
		);
	}
}

void turn(Warrior warriors[], int index, char debug) {
	if (warriors[index].alive) {
		if (warriors[index].chrg == warriors[index].cost) {
			special(warriors, index, debug);
		}
		
		else {
			attack(warriors, index, debug);
		}
	}
}

int check(Warrior warriors[], char debug) {
	int team_1 = MAX_WARR / 2;
	int team_2 = MAX_WARR / 2;
	
	for (int i = 0; i < MAX_WARR; i++) {
		if (warriors[i].hp > warriors[i].mhp) {
			warriors[i].hp = warriors[i].mhp;
		}
		
		if (warriors[i].chrg > warriors[i].cost) {
			warriors[i].chrg = warriors[i].cost;
		}
		
		if (warriors[i].hp <= 0 && warriors[i].alive) {
			if (debug == 'y') {
				printf("%s on team %d has been defeated!\n\n", warriors[i].name, warriors[i].team);
			}
			
			warriors[i].hp = 0;
			warriors[i].chrg = 0;
			warriors[i].alive = false;
		}
		
		if (!warriors[i].alive) {
			if (warriors[i].team == 1) {
				team_1--;
			}
			
			else {
				team_2--;
			}
		}
	}
	
	if (team_1 && !team_2) {
		return 1;
	}
	
	else if (!team_1 && team_2) {
		return 2;
	}
	
	else if (!(team_1 || team_2)) {
		return 3;
	}
	
	return 0;
}

void log_data(int warr_data[][TOT_WARR + 1][2], int team_data[][TOT_WARR][2], Warrior warriors[], int winner, char debug) {
	if (winner == 1 || winner == 2) {
		if (debug == 'y') {
			printf("\nTeam %d wins!\n", winner);
		}
		
		for (int i = 0; i < MAX_WARR; i++) {
			for (int j = 0; j < TOT_WARR; j++) {
				if (!strcmp(warriors[i].name, ALL_WARR[j])) {
					warr_data[j][TOT_WARR][1]++;
					
					if (warriors[i].team == winner) {
						warr_data[j][TOT_WARR][0]++;
					}
					
					for (int k = 0; k < MAX_WARR; k++) {
						if (warriors[k].team != warriors[i].team) {
							for (int l = 0; l < TOT_WARR; l++) {
								if (!strcmp(warriors[k].name, ALL_WARR[l])) {
									warr_data[j][l][1]++;
									
									if (warriors[i].team == winner) {
										warr_data[j][l][0]++;
									}
								}
							}
						}
					}
					
					for (int k = 0; k < MAX_WARR; k++) {
						if (warriors[k].team == warriors[i].team && k != i) {
							for (int l = 0; l < TOT_WARR; l++) {
								if (!strcmp(warriors[k].name, ALL_WARR[l])) {
									team_data[j][l][1]++;
									
									if (warriors[i].team == winner) {
										team_data[j][l][0]++;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	
	else if (debug == 'y') {
		printf("\nIt's a tie!\n");
	}
	
	if (debug == 'y') {
		for (int i = 0; i < MAX_WARR; i++) {
			printf("%s on team %d's HP: %d / %d\n", warriors[i].name, warriors[i].team, warriors[i].hp, warriors[i].mhp);
		}
		
		printf("\n");
	}
	
}

void display_data(int warr_data[][TOT_WARR + 1][2], int team_data[][TOT_WARR][2]) {
	printf("         Matchup:");
	
	for (int i = 0; i < TOT_WARR - strlen("Matchup"); i++) {
		printf(" ");
	}
	
	printf("         Teamup:\n         ");
	
	for (int i = 0; i < TOT_WARR; i++) {
		printf("%c", ALL_WARR[i][0]);
	}
	
	printf("          ");
	
	for (int i = 0; i < TOT_WARR; i++) {
		printf("%c", ALL_WARR[i][0]);
	}
	
	printf("\n");
	
	for (int i = 0; i < TOT_WARR; i++) {
		printf("%8s ", ALL_WARR[i]);
		
		for (int j = 0; j < TOT_WARR; j++) {
			double matchup = (double) warr_data[i][j][0] / warr_data[i][j][1];
			
			if (matchup < 0.5 - 4 * TOLERANCE) {
				printf("%c", -82);
			}
			
			else if (matchup < 0.5 - 2 * TOLERANCE) {
				printf("<");
			}
			
			else if (matchup < 0.5 - TOLERANCE) {
				printf("(");
			}
			
			else if (matchup > 0.5 + 4 * TOLERANCE) {
				printf("%c", -81);
			}
			
			else if (matchup > 0.5 + 2 * TOLERANCE) {
				printf(">");
			}
			
			else if (matchup > 0.5 + TOLERANCE) {
				printf(")");
			}
			
			else if (matchup == 0.5) {
				printf("=");
			}
			
			else {
				printf("-");
			}
		}
		
		if (warr_data[i][TOT_WARR][1]) {
			printf(" %f ", (double) warr_data[i][TOT_WARR][0] / warr_data[i][TOT_WARR][1]);
		}
		
		else {
			printf(" XXXXXXXX ");
		}
		
		for (int j = 0; j < TOT_WARR; j++) {
			double teamup = (double) team_data[i][j][0] / team_data[i][j][1];
			
			if (teamup < 0.5 - 4 * TOLERANCE) {
				printf("%c", -82);
			}
			
			else if (teamup < 0.5 - 2 * TOLERANCE) {
				printf("<");
			}
			
			else if (teamup < 0.5 - TOLERANCE) {
				printf("(");
			}
			
			else if (teamup > 0.5 + 4 * TOLERANCE) {
				printf("%c", -81);
			}
			
			else if (teamup > 0.5 + 2 * TOLERANCE) {
				printf(">");
			}
			
			else if (teamup > 0.5 + TOLERANCE) {
				printf(")");
			}
			
			else if (teamup == 0.5) {
				printf("=");
			}
			
			else {
				printf("-");
			}
		}
		
		printf(" %-8s (%d)\n", ALL_WARR[i], i);
	}
	
	printf("         ");
	
	for (int i = 0; i < TOT_WARR; i++) {
		printf("%c", ALL_WARR[i][0]);
	}
	
	printf("          ");
	
	for (int i = 0; i < TOT_WARR; i++) {
		printf("%c", ALL_WARR[i][0]);
	}
	
	printf("\n\n");
}
	
int main(int argc, char* argv[]) {
	srand(time(NULL));
	
	Warrior warriors[MAX_WARR];
	
	int runs;
	
	if (argc > 1) {
		sscanf(argv[1], "%d", &runs);
	}
	
	else {
		runs = DEF_RUNS;
	}
	
	char debug;
	
	if (argc > 2) {
		sscanf(argv[2], "%c", &debug);
	}
	
	else {
		debug = DEF_DEBUG;
	}
	
	if (runs < 1 || debug != 'y' && debug != 'n' || 3 < argc && argc < MAX_WARR + 3 || argc > MAX_WARR + 3) {
		printf("Invalid arguments.\n");
		return EXIT_FAILURE;
	}
	
	int codes[MAX_WARR];
	
	if (argc == MAX_WARR + 3) {
		
		for (int i = 0; i < MAX_WARR; i++) {
			sscanf(argv[i + 3], "%d", &codes[i]);
			
			if (codes[i] < 0 || codes[i] >= TOT_WARR) {
				printf("Invalid codes.\n");
				return EXIT_FAILURE;
			}
		}
	}
	
	int warr_data[TOT_WARR][TOT_WARR + 1][2];
	int team_data[TOT_WARR][TOT_WARR][2];
	
	for (int i = 0; i < TOT_WARR; i++) {
		for (int j = 0; j <= TOT_WARR; j++) {
			for (int k = 0; k < 2; k++) {
				warr_data[i][j][k] = 0;
				team_data[i][j][k] = 0;
			}
		}
	}
	
	for (int i = 0; i < runs; i++) {
		if (argc < 4) {
			for (int i = 0; i < MAX_WARR; i++) {
				summon(&warriors[i], rand_int(0, TOT_WARR - 1), 1 + 2 * i / MAX_WARR);
			}
		}
		
		else if (argc == MAX_WARR + 3) {
			for (int i = 0; i < MAX_WARR; i++) {
				summon(&warriors[i], codes[i], 1 + 2 * i / MAX_WARR);
			}
		}
		
		if (debug == 'y') {
			printf("Team 1:");
			
			for (int j = 0; j < MAX_WARR; j++) {
				if (warriors[j].team == 1) {
					printf(" %s", warriors[j].name);
				}
			}
			
			printf("\nTeam 2:");
			
			for (int j = 0; j < MAX_WARR; j++) {
				if (warriors[j].team == 2) {
					printf(" %s", warriors[j].name);
				}
			}
			
			printf("\n");
		}
		
		int winner = 0;
		
		while (!winner) {
			order(warriors);
			
			for (int j = 0; j < MAX_WARR; j++) {
				turn(warriors, j, debug);
				
				if (winner = check(warriors, debug)) {
					break;
				}
			}
		}
		
		log_data(warr_data, team_data, warriors, winner, debug);
	}
	
	display_data(warr_data, team_data);
}