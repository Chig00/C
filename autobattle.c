/* New warrior protocol:
       Redefine TOTAL_WARRIORS to be one greater than it already is
	   Add the new warrior's name to WARRIOR_NAMES
	   Add the new warriors' stats to WARRIOR_STATS
	   Add the description of the new warrior's special move to WARRIOR_SPECIALS
	   Add any variable relating the the new warrior's special move to SPECIAL_STATS
	   Add a new clause to the if-statement in special() and define the special move
 */

#include <stdio.h> // Input and Output
#include <stdlib.h> // Dynamic memeory allocation and program exit
#include <stdbool.h> // Boolean variables
#include <string.h> // String comparison

// Contains the stats for a warrior
typedef struct Warrior {
	const char* name;
	int max_health;
	int health;
	int attack;
	int speed;
	int charge;
	int cost;
	bool alive;
	int team;
} Warrior;

// Contains the player's identity, score, and a pointer to their warriors
typedef struct Player {
	Warrior* warriors;
	int identity;
	int score;
} Player;

// Contains the number of players, the number of warriors per player,
//   and a pointer to the players
typedef struct GamePackage{
	Player* players;
	int player_count;
	int warrior_count;
} GamePackage;

// Warrior constants
#define TOTAL_WARRIORS 6
const char* const WARRIOR_NAMES[TOTAL_WARRIORS] = {
	"Knight",
	"Huntress",
	"Wizard",
	"Assassin",
	"Mage",
	"Soldier"
};
const int HEALTH_INDEX = 0;
const int ATTACK_INDEX = 1;
const int SPEED_INDEX = 2;
const int COST_INDEX = 3;
#define WARRIOR_STAT_COUNT 4
const int WARRIOR_STATS[TOTAL_WARRIORS][WARRIOR_STAT_COUNT] = {
	{1000, 100, 100, 3}, // Knight
	{800, 120, 120, 5},  // Huntress
	{750, 125, 125, 3},  // Wizard
	{650, 75, 150, 1},   // Assassin
	{600, 200, 75, 3},   // Mage
	{775, 150, 60, 1}    // Soldier
};
const char* const WARRIOR_SPECIALS[TOTAL_WARRIORS] = {
	"Attack with %.2fx power",                         // Knight
	"Attack with %.2fx power",                         // Huntress
	"Attack all enemies with %.2fx power",             // Wizard
	"Attack the lowest health enemy with %.2fx power", // Assassin
	"Attack with %.2fx power",                         // Mage
	"Attack all enemies with %.2fx power"              // Soldier
};
const double SPECIAL_STATS[TOTAL_WARRIORS][3] = {
	{3},
	{5},
	{1.5},
	{3.5},
	{2.75},
	{0.9}
};

// Command line argument ordering constants
const int PLAYER_ARG = 1;
const int WARRIOR_ARG = 2;
const int CHOICE_ARG = 3;

// Input check constants
const int MIN_PLAYERS = 2;
const int MAX_PLAYERS = 10;
const int MIN_WARRIORS = 1;
const int MAX_WARRIORS = 10;

// Miscellaneous constants
const int TIE_GAME = -1;

/* Returns the number of players in the game
   Number gotten from command line arguments if available
   Number gotten from standard input otherwise
 */
int get_player_count(int argc, char* argv[]) {
	int player_count;
	
	if (argc > PLAYER_ARG) {
		sscanf(argv[PLAYER_ARG], "%d", &player_count);
	}
	
	else {
		printf("Number of players: ");
		scanf("%d", &player_count);
	}
	
	return player_count;
}

/* Returns the number of warriors per player
   Number gotten from command line arguments if avliable
   Number gotten from standard input otherwise
 */
int get_warrior_count(int argc, char* argv[]) {
	int warrior_count;
	
	if (argc > WARRIOR_ARG) {
		sscanf(argv[WARRIOR_ARG], "%d", &warrior_count);
	}
	
	else {
		printf("Warriors per player: ");
		scanf("%d", &warrior_count);
	}
	
	return warrior_count;
}

/* Returns true if the number of players or warriors is invalid
   Invalidity is due to too few or many players or warriors per player,
     or the total number of warriors being too great  
 */
bool count_check(GamePackage game_package, int argc) {
	if (game_package.player_count < MIN_PLAYERS) {
		printf("Too few players.\nMinimum: %d\n", MIN_PLAYERS);
		return true;
	}
	
	if (game_package.player_count > MAX_PLAYERS) {
		printf("Too many players.\nMaximum: %d\n", MAX_PLAYERS);
		return true;
	}
	
	if (game_package.warrior_count < MIN_WARRIORS) {
		printf("Too few warriors per player.\nMinimum: %d\n", MIN_WARRIORS);
		return true;
	}
	
	if (game_package.warrior_count > MAX_WARRIORS) {
		printf("Too many warriors per player.\nMaximum: %d\n", MAX_WARRIORS);
		return true;
	}
	
	if (
		game_package.player_count * game_package.warrior_count > TOTAL_WARRIORS
		&& argc <= CHOICE_ARG
	) {
		printf("Too many warriors in total.\nMaximum: %d\n", TOTAL_WARRIORS);
		return true;
	}
	
	return false;
}

/* Initialises all of the players
   Players have memory allocated to their warriors
   Players have their identity set (e.g. Player 1)
   Players have their score set to zero
 */
void init_players(GamePackage game_package) {
	for (int i = 0; i < game_package.player_count; i++) {
		game_package.players[i].warriors = malloc(game_package.warrior_count * sizeof(Warrior));
		game_package.players[i].identity = i + 1;
		game_package.players[i].score = 0;
	}
}

/* Sets the warrior's stats according to its code
 */
void init_warrior(Warrior* warrior, int code, Player* player) {
	// The code is received one-indexed, so it is decremented
	code--;
	warrior->name = WARRIOR_NAMES[code];
	warrior->max_health = WARRIOR_STATS[code][HEALTH_INDEX];
	warrior->attack = WARRIOR_STATS[code][ATTACK_INDEX];
	warrior->speed = WARRIOR_STATS[code][SPEED_INDEX];
	warrior->cost = WARRIOR_STATS[code][COST_INDEX];
	warrior->team = player->identity;
}

/* Builds the teams using command line arguments
 */
void arg_build(GamePackage game_package, int argc, char* argv[]) {
	// There can be multiple arguments, so a non-constant index is used
	int index = CHOICE_ARG;
	
	for (int i = 0; i < game_package.player_count; i++) {
		for (int j = 0; j < game_package.warrior_count; j++) {
			// The one-indexed code is obtained and used to initialise the warrior
			int code;
			sscanf(argv[index], "%d", &code);
			
			// Code is checked for validity
			if (code < 1 || code > TOTAL_WARRIORS) {
				printf(
					"Invalid warrior code.\nOnly choose codes from 1 to %d.\n",
					TOTAL_WARRIORS
				);
				exit(1);
			}
			
			init_warrior(
				game_package.players[i].warriors + j, code,
				game_package.players + i
			);
			
			// If there is a subsequent argument, then it is used
			// No subsequent arguments sets the rest of the warriors to the last
			if (argc > index + 1) {
				index++;
			}
		}
	}
}

/* Displays the currently available warriors and their codes
   Includes the stats for each warrior (so that players can make a
     better informed decision)
   Codes are one-indexed
 */
void show_available(bool available[]) {
	for (int i = 0; i < TOTAL_WARRIORS; i++) {
		if (available[i]) {
			printf(
				"%s (%d):\n  Health: %d\n  Attack: %d\n  Speed: %d\n  Special: ",
				WARRIOR_NAMES[i],
				i + 1,
				WARRIOR_STATS[i][HEALTH_INDEX],
				WARRIOR_STATS[i][ATTACK_INDEX],
				WARRIOR_STATS[i][SPEED_INDEX]
			);
			// Separate printf function, so the formatting can be done
			printf(
				WARRIOR_SPECIALS[i],
				SPECIAL_STATS[i][0],
				SPECIAL_STATS[i][1],
				SPECIAL_STATS[i][2]
			);
			printf("\n  Cost: %d\n\n", WARRIOR_STATS[i][COST_INDEX]);
		}
	}
}

/* Prompts the user a warrior code until a valid one is inputted
 */
int get_code(bool available[], int index) {
	while (true) {
		int code;
		printf("Player %d, choose a warrior: ", index + 1);
		scanf("%d", &code);
		
		if (code > 0 && code <= TOTAL_WARRIORS) {
			if (available[code - 1]) {
				printf("\n");
				return code;
			}
		}
		
		printf("Invalid warrior code.\nChoose an available code.\n");
	}
}

/* Builds the teams using standard input
 */
void stdin_build(GamePackage game_package) {
	bool available[TOTAL_WARRIORS];
	int inits = 0; // Number of warriors intialised
	
	// The array of Booleans is initialised to true for all values
	for (int i = 0; i < TOTAL_WARRIORS; i++) {
		available[i] = true;
	}
	
	while (inits < game_package.player_count * game_package.warrior_count) {
		for (int i = 0; i < game_package.player_count; i++) {
			// The currently available warriors are displayed and the code is obtained
			show_available(available);
			int code = get_code(available, i);
			
			// The warrior's index only increments after all players have chosen
			init_warrior(
				(
					game_package.players[i].warriors
					+ inits / game_package.player_count
				),
				code,
				game_package.players + i
			);
			
			// -1, because codes are one-indexed
			available[code - 1] = false;
			inits++;
		}
		
		// Two loop condition checks for the two loops
		if (inits == game_package.player_count * game_package.warrior_count) {
			break;
		}
		
		// Loop in the opposite direction for fairness
		for (int i = game_package.player_count - 1; i > -1; i--) {
			// The currently available warriors are displayed and the code is obtained
			show_available(available);
			int code = get_code(available, i);
			
			// The warrior's index only increments after all players have chosen
			init_warrior(
				(
					game_package.players[i].warriors
					+ inits / game_package.player_count
				),
				code,
				game_package.players + i
			);
			
			// -1, because codes are one-indexed
			available[code - 1] = false;
			inits++;
		}
	}
}

/* Build the players' teams
   If the teams were defined with command line arguments, then they are used
   If teams were not predefined, each player takes turns choosing their warrior
   The last pick for the previous round of picks become the first for the next
 */
void build_teams(GamePackage game_package, int argc, char* argv[]) {
	if (argc > CHOICE_ARG) {
		arg_build(game_package, argc, argv);
	}
	
	else {
		stdin_build(game_package);
	}
}

/* Initialises the common warrior store
   The memory location of all of the warriors is stored, because
     two player's warriors will not necessarily be in contiguous
	 memory blocks
 */
void make_arena(
	Warrior* warriors[], Player* player_one,
	Player* player_two, int warrior_count
) {
	for (int i = 0; i < 2 * warrior_count; i++) {
		warriors[i] =
			i % 2 ? player_one->warriors + i / 2
			: player_two->warriors + i / 2
		;
	}
}

/* Sets the health of all warriors to their maximum_health
   Sets the special charge of all warrior to zero
   Sets all of the warriors to be alive
 */
void heal(Warrior* warriors[], int warrior_count) {
	for (int i = 0; i < 2 * warrior_count; i++) {
		warriors[i]->health = warriors[i]->max_health;
		warriors[i]->charge = 0;
		warriors[i]->alive = true;
	}
}

/* Displays the full stats for both teams
 */
void full_stats(Player* player_one, Player* player_two, int warrior_count) {
	printf("Player %d's team:\n", player_one->identity);
	
	for (int i = 0; i < warrior_count; i++) {
		printf("  %s:\n", player_one->warriors[i].name);
		printf(
			"    Health: %d/%d\n",
			player_one->warriors[i].health,
			player_one->warriors[i].max_health
		);
		printf("    Attack: %d\n", player_one->warriors[i].attack);
		printf("    Speed: %d\n", player_one->warriors[i].speed);
		printf(
			"    Special charge: %d/%d\n\n",
			player_one->warriors[i].charge,
			player_one->warriors[i].cost
		);
	}
	
	printf("Player %d's team:\n", player_two->identity);
	
	for (int i = 0; i < warrior_count; i++) {
		printf("  %s:\n", player_two->warriors[i].name);
		printf(
			"    Health: %d/%d\n",
			player_two->warriors[i].health,
			player_two->warriors[i].max_health
		);
		printf("    Attack: %d\n", player_two->warriors[i].attack);
		printf("    Speed: %d\n", player_two->warriors[i].speed);
		printf(
			"    Special charge: %d/%d\n\n",
			player_two->warriors[i].charge,
			player_two->warriors[i].cost
		);
	}
}

/* Displays the health and special charge for both teams
 */
void partial_stats(Player* player_one, Player* player_two, int warrior_count) {
	printf("Player %d's team:\n", player_one->identity);
	
	for (int i = 0; i < warrior_count; i++) {
		printf(
			"  %s: %d/%d %d/%d\n",
			player_one->warriors[i].name,
			player_one->warriors[i].health,
			player_one->warriors[i].max_health,
			player_one->warriors[i].charge,
			player_one->warriors[i].cost
		);
	}
	
	printf("\nPlayer %d's team:\n", player_two->identity);
	
	for (int i = 0; i < warrior_count; i++) {
		printf(
			"  %s: %d/%d %d/%d\n",
			player_two->warriors[i].name,
			player_two->warriors[i].health,
			player_two->warriors[i].max_health,
			player_two->warriors[i].charge,
			player_two->warriors[i].cost
		);
	}
	
	printf("\n");
}

/* Returns a negative number if the warrior is faster
   Returns a positive number if the warrior is slower
 */
int speed_sort(const void* warrior_one, const void* warrior_two) {
	return (*((Warrior**) warrior_two))->speed - (*((Warrior**) warrior_one))->speed;
}

/* Orders the warriors by speed
   Unlike in Battle Sim, speed ties are impossible,
     so no shuffling is done beforehand
 */
void order(Warrior* warriors[], int warrior_count) {
	qsort(warriors, 2 * warrior_count, sizeof(Warrior*), speed_sort);
}

/* Attacks the highest health enemy
 */
void attack(Warrior* warriors[], int index, int warrior_count) {
	int highest_health = 0;
	
	// The highest health of all of the enemies is recorded
	for (int i = 0; i < 2 * warrior_count; i++) {
		if (warriors[i]->team != warriors[index]->team && warriors[i]->alive) {
			if (warriors[i]->health > highest_health) {
				highest_health = warriors[i]->health;
			}
		}
	}
	
	int target;
	
	// The enemy with the highest health is recorded
	for (target = 0; target < 2 * warrior_count; target++) {
		if (warriors[target]->team != warriors[index]->team && warriors[target]->alive) {
			if (warriors[target]->health == highest_health) {
				break;
			}
		}
	}
	
	// The enemy is attacked
	warriors[target]->health -= warriors[index]->attack;
	warriors[target]->charge++;
	warriors[index]->charge++;
	
	printf(
		"%s on team %d dealt %d damage to %s on team %d!\n\n",
		warriors[index]->name, warriors[index]->team, warriors[index]->attack,
		warriors[target]->name, warriors[target]->team
	);
}

/* Attacks all enemies
 */
void splash(Warrior* warriors[], int index, int warrior_count) {
	for (int i = 0; i < 2 * warrior_count; i++) {
		if (warriors[i]->team != warriors[index]->team && warriors[i]->alive) {
			warriors[i]->health -= warriors[index]->attack;
			warriors[i]->charge++;
			
			printf(
				"%s on team %d dealt %d damage to %s on team %d!\n",
				warriors[index]->name, warriors[index]->team, warriors[index]->attack,
				warriors[i]->name, warriors[i]->team
			);
		}
	}
	
	printf("\n");
}

/* Attacks the lowest health enemy
 */
void snipe(Warrior* warriors[], int index, int warrior_count) {
	int lowest_health = 9999;
	
	// The lowest health of all of the enemies is recorded
	for (int i = 0; i < 2 * warrior_count; i++) {
		if (warriors[i]->team != warriors[index]->team && warriors[i]->alive) {
			if (warriors[i]->health < lowest_health) {
				lowest_health = warriors[i]->health;
			}
		}
	}
	
	int target;
	
	// The enemy with the lowest health is recorded
	for (target = 0; target < 2 * warrior_count; target++) {
		if (warriors[target]->team != warriors[index]->team && warriors[target]->alive) {
			if (warriors[target]->health == lowest_health) {
				break;
			}
		}
	}
	
	// The enemy is attacked
	warriors[target]->health -= warriors[index]->attack;
	warriors[target]->charge++;
	
	printf(
		"%s on team %d dealt %d damage to %s on team %d!\n\n",
		warriors[index]->name, warriors[index]->team, warriors[index]->attack,
		warriors[target]->name, warriors[target]->team
	);
}

/* Uses the warrior's special move
 */
void special(Warrior* warriors[], int index, int warrior_count) {
	printf(
		"%s on team %d used their special!\n",
		warriors[index]->name, warriors[index]->team
	);
	
	// Knight's Special Move
	if (!strcmp(warriors[index]->name, WARRIOR_NAMES[0])) {
		int original = warriors[index]->attack;
		// Knight's special move is a normal attack with added power
		warriors[index]->attack *= SPECIAL_STATS[0][0];
		attack(warriors, index, warrior_count);
		// The attack is returned to its normal value
		// Division is not used, so the original value is always returned to
		warriors[index]->attack = original;
	}
	
	// Huntress' Special Move
	else if (!strcmp(warriors[index]->name, WARRIOR_NAMES[1])) {
		int original = warriors[index]->attack;
		// Huntress' special move is a normal attack with added power
		warriors[index]->attack *= SPECIAL_STATS[1][0];
		attack(warriors, index, warrior_count);
		// The attack is returned to its normal value
		// Division is not used, so the original value is always returned to
		warriors[index]->attack = original;
	}
	
	// Wizard's Special Move
	else if (!strcmp(warriors[index]->name, WARRIOR_NAMES[2])) {
		int original = warriors[index]->attack;
		// Wizard's special move is a splash attack with added power
		warriors[index]->attack *= SPECIAL_STATS[2][0];
		splash(warriors, index, warrior_count);
		// The attack is returned to its normal value
		// Division is not used, so the original value is always returned to
		warriors[index]->attack = original;
	}
	
	// Assassin's Special Move
	else if (!strcmp(warriors[index]->name, WARRIOR_NAMES[3])) {
		int original = warriors[index]->attack;
		// Assassin's special move is an attack on the lowest health enemy with added power
		warriors[index]->attack *= SPECIAL_STATS[3][0];
		snipe(warriors, index, warrior_count);
		// The attack is returned to its normal value
		// Division is not used, so the original value is always returned to
		warriors[index]->attack = original;
	}
	
	// Mage's Special Move
	else if (!strcmp(warriors[index]->name, WARRIOR_NAMES[4])) {
		int original = warriors[index]->attack;
		// Mage's special move is a normal attack with added power
		warriors[index]->attack *= SPECIAL_STATS[4][0];
		attack(warriors, index, warrior_count);
		// The attack is returned to its normal value
		// Division is not used, so the original value is always returned to
		warriors[index]->attack = original;
	}
	
	// Soldier's Special Move
	else if (!strcmp(warriors[index]->name, WARRIOR_NAMES[5])) {
		int original = warriors[index]->attack;
		// Soldier's special move is a splash attack with reduced power
		warriors[index]->attack *= SPECIAL_STATS[5][0];
		splash(warriors, index, warrior_count);
		// The attack is returned to its normal value
		// Division is not used, so the original value is always returned to
		warriors[index]->attack = original;
	}
	
	// Using a super depletes the warrior's special charge
	warriors[index]->charge = 0;
}

/* The highest health enemy is attacked if the special isn't fully charged
   If the warrior's special move is charged, then it is used instead
 */
void turn(Warrior* warriors[], int index, int warrior_count) {
	if (warriors[index]->alive) {
		if (warriors[index]->charge == warriors[index]->cost) {
			special(warriors, index, warrior_count);
		}
		
		else {
			attack(warriors, index, warrior_count);
		}
	}
}

/* Checks if the game is over
   If all of the warriors on a single team are defeated, then the game is over
   If a warrior's health is less than or equal to zero and they haven't
     been marked as defeated, then their defeat is announced and their health
	 is set to zero
 */
int check(Player* player_one, Player* player_two, int warrior_count) {
	// Determines the winner
	// 0 means game unfinished
	// 1 means player_one wins
	// 2 means player_two wins
	// 3 means game ended in a tie
	int winner = 3;
	bool defeated = true;
	
	for (int i = 0; i < warrior_count; i++) {
		if (player_one->warriors[i].alive) {
			if (player_one->warriors[i].health <= 0) {
				player_one->warriors[i].health = 0;
				player_one->warriors[i].alive = false;
				printf(
					"%s on team %d has been defeated!\n\n", 
					player_one->warriors[i].name,
					player_one->identity
				);
			}
			
			else if (defeated) {
				// Player two didnt't win at this point
				winner -= 2;
				defeated = false;
			}
		}
		
		if (player_one->warriors[i].health > player_one->warriors[i].max_health) {
			player_one->warriors[i].health = player_one->warriors[i].max_health;
		}
		
		if (player_one->warriors[i].charge > player_one->warriors[i].cost) {
			player_one->warriors[i].charge = player_one->warriors[i].cost;
		}
		
		else if (player_one->warriors[i].charge < 0) {
			player_one->warriors[i].charge = 0;
		}
	}
	
	defeated = true;
	
	for (int i = 0; i < warrior_count; i++) {
		if (player_two->warriors[i].alive) {
			if (player_two->warriors[i].health <= 0) {
				player_two->warriors[i].health = 0;
				player_two->warriors[i].alive = false;
				printf(
					"%s on team %d has been defeated!\n\n", 
					player_two->warriors[i].name,
					player_two->identity
				);
			}
			
			else if (defeated) {
				// Player one didnt't win at this point
				winner -= 1;
				defeated = false;
			}
		}
		
		if (player_two->warriors[i].health > player_two->warriors[i].max_health) {
			player_two->warriors[i].health = player_two->warriors[i].max_health;
		}
		
		if (player_two->warriors[i].charge > player_two->warriors[i].cost) {
			player_two->warriors[i].charge = player_two->warriors[i].cost;
		}
		
		else if (player_two->warriors[i].charge < 0) {
			player_two->warriors[i].charge = 0;
		}
	}
	
	return winner;
}	

/* Displays the results of the fight
   The winner and full stats of the warriors are displayed
 */
void results(int winner, Player* player_one, Player* player_two, int warrior_count) {
	switch (winner) {
		case 1:
			printf("Player %d wins!\n\n", player_one->identity);
			player_one->score++;
			break;
		
		case 2:
			printf("Player %d wins!\n\n", player_two->identity);
			player_two->score++;
			break;
		
		default:
			printf(
				"Players %d and %d tied!\n\n",
				player_one->identity,
				player_two->identity
			);
	}
	
	full_stats(player_one, player_two, warrior_count);
}

/* Hosts a fight between two player's teams
   Normal attacks target the highest attack opponent
   Special attacks may have different targetting conditions
   Special attacks are always performed if the warrior's
     charge is equal to their special cost
   The battle ends when all of the warriors on a team lose
     all of their health
   The winner's score is incremented
 */
void duel(Player* player_one, Player* player_two, int warrior_count) {
	// Common store for warriors, so that speed ordering can be performed
	Warrior** warriors = malloc(2 * warrior_count * sizeof(Warrior*));
	make_arena(warriors, player_one, player_two, warrior_count);
	
	// Warriors' health should start at their maximum health
	//   and special charge  should start at zero
	heal(warriors, warrior_count);
	
	// The full stats for both teams is displayed
	full_stats(player_one, player_two, warrior_count);
	
	int winner = 0;
	
	while (!winner) {
		// A summary for the match so far is displayed for each round
		partial_stats(player_one, player_two, warrior_count);
		
		// User input is requested, so the players can track the match as it goes on
		getchar();
		
		// Speed can change in the game, so reordering is performed
		order(warriors, warrior_count);
		
		for (int i = 0; i < 2 * warrior_count; i++) {
			
			// A warrior has their turn
			// A turn consists of attacking the opposing team's highest health warrior
			//   or using a special move
			turn(warriors, i, warrior_count);
			
			// If a team is elimated, then the winning team is reported
			// Also if a warrior is defeated, then their defeat is announced
			// The defeated warrior has their health and super charge set to zero
			if (winner = check(player_one, player_two, warrior_count)) {
				// The common store is not necessary after the battle ends
				free(warriors);
				break;
			}
		}
	}
	
	getchar();
	
	// The winner is announced, the end of battle stats are shown,
	//   and the players' scores are updated
	results(winner, player_one, player_two, warrior_count);
	
	getchar();
}

/* Returns the index (zero-indexed) of the winner
   Multiple winners (a tie) will result in TIE_GAME being returned instead
 */
int get_winner(GamePackage game_package) {
	int best = 0;
	
	// The highest score is found and stored
	for (int i = 0; i < game_package.player_count; i++) {
		if (game_package.players[i].score > best) {
			best = game_package.players[i].score;
		}
	}
	
	int winner = TIE_GAME;
	
	for (int i = 0; i < game_package.player_count; i++) {
		if (game_package.players[i].score == best) {
			if (winner == TIE_GAME) {
				winner = i;
			}
			
			// If two players share the top score, then there is a tie
			else {
				return TIE_GAME;
			}
		}
	}
	
	return winner;
}

/* Makes each player's team fight all the other teams
   A point is awarded to the winner of each fight
   The winner is the player with the most points
 */
void game(GamePackage game_package) {
	for (int i = 0; i < game_package.player_count - 1; i++) {
		for (int j = i + 1; j < game_package.player_count; j++) {
			duel(game_package.players + i, game_package.players + j, game_package.warrior_count);
		}
	}
	
	int winner = get_winner(game_package);
	
	if (winner == TIE_GAME) {
		printf("It's a tie!\n");
	}
	
	else {
		printf("Player %d wins the tournament!\n", winner + 1);
	}
	
	for (int i = 0; i < game_package.player_count; i++) {
		printf("Player %d: %d wins\n", i + 1, game_package.players[i].score);
	}
}

/* AUTO Battle: Battle Sim 2
   Each player chooses warriors that automatically do battle
     against all other teams
   The player that wins the most battles wins
 */
int main(int argc, char* argv[]) {
	// Contains data necessary for the game to be played
	GamePackage game_package;
	
	// Numbers of players and warriors set
	game_package.player_count = get_player_count(argc, argv);
	game_package.warrior_count = get_warrior_count(argc, argv);
	
	// Numbers checked for validity
	if (count_check(game_package, argc)) {
		return 1;
	}
	
	// Players allocated memory
	game_package.players = malloc(game_package.player_count * sizeof(Player));
	
	printf("\n");
	
	// Players set to intial values and teams built
	init_players(game_package);
	build_teams(game_package, argc, argv);
	
	// Game begins
	game(game_package);
}