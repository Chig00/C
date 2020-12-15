#include <stdio.h> // Used for IO
#include <stdlib.h> // Used for RNG
#include <time.h> // Used for RNG seed
#include <string.h> // Used for command line argument checking

const int PLAYER_COUNT = 8; // The default number of players
const int BASE_RATING = 10000; // The default base rating
const int RUNS = 100000; // The number of times that the tournament will be
                      //   ran before displaying the results
const int BASE_TRANSFER = 100; // The transferred rating for players of equal skill

// Ordering for the arguments
const int RUNS_ARGUMENT = 1; // The argument for the number of runs should be first
const int COUNT_ARGUMENT = 2; // The argument for the player count should be second
const int RATING_ARGUMENT = 3; // The argument for the base rating should be thrid
const int SKILL_ARGUMENT = 4; // The arguments for the players' skill should be last

// Each player has their skill and rating
typedef struct {
	int skill;
	int rating;
} Player;

/* Returns a random number in the range [0, total)
 */
int rand_int(int total) {
	return total * rand() / (RAND_MAX + 1);
}

/* Transfers the rating from the loser to the winner
   Transfer = Base * LRating / WRating
 */
void transfer(Player* winner, Player* loser) {
	int transfer;
	
	// If the winner's rating is greater than zero, then the normal formula is applied
	if (winner->rating > 0) {
		transfer = BASE_TRANSFER * loser->rating / winner->rating;
	}
	
	// Else, the winner's rating is assumed to be 1
	else {
		transfer = BASE_TRANSFER * loser->rating;
	}
	
	winner->rating += transfer;
	loser->rating -= transfer;
}

/* The two players passed as arguments fight
   The odds of a player winning is determined by the player's skill
   If the player's skill is x and the opponent's skill is y, then
     the odds of the first player winning is equal to x / (x + y)
	 and the odds of the second player winning is equal to y / (x + y)
   The winner gains rating from the loser (as in the loser transfers some
     of the their rating directly to the winner)
   The rating points transferred is equal to the base rating transfer
     * the loser's rating / the winner's rating
   This system results in some specific interactions:
       Two players of equal rating facing off each other will transfer
	     the base rating transfer from the loser to the winner
	   If the winner had a higher rating than the loser, then the winner
	     will be transferred rating that is less than the base
	   If the winner has a lower rating than than the loser, then the
	     winner will be transferred rating that is greater than the base
	   The greater the difference from each other's ratings, the greater
	     the difference from the base rating transfer
 */
void duel(Player* player_one, Player* player_two) {
	// If player one wins, then player two transfers their rating to player one
	if (rand_int(player_one->skill + player_two->skill) < player_one->skill) {
		transfer(player_one, player_two);
	}
	
	// Else, player one transfers their rating to player two
	else {
		transfer(player_two, player_one);
	}
}

/* A single run of the tournament
   Each player faces all other players in a duel
   Each duel will have a winner and a loser, no ties
   The winner gains the same amount of rating as the loser loses
   The total rating for all players should not change
 */
void tournament(Player* players, int player_count) {
	for (int i = 0; i < player_count; i++) {
		for (int j = 0; j < player_count; j++) {
			// Mirror matches are a waste of time and are skipped
			if (i != j) {
				duel(players + i, players + j);
			}
		}
	}
}

/* Rating Simulator by Chigozie Agomo
   This simulates a group of players entering a tourney
   The players all have two attributes, their skill and rating
   Skill determines the probability of a player beating another
   The chance that a player of skill x beating a player with skill y
     is x / (x + y)
   Losing players transfer some of their rating to the winning players
   The rating transferred is based on the ratio of the rating of the two
     players
   A player with double the rating of another will win half the number
     of rating points if they beat the other player, but will lose
	 double the number of rating points if the other player beats
	 them
   This means that expected outcomes have little impact on the ratings
     of players, but great upsets will cause a large transfer of rating
   At the end of the tournament, the players' skill and rating will be
     displayed and players with greater skill tend to get a greater rating
 */
int main(int argc, char* argv[]) {
	// The RNG is seeded
	srand(time(NULL));
	
	int player_count; // The number of players entering the tournament
	
	// If a command line argument was used, the number of players
	//   is set to the argument's value
	if (argc > COUNT_ARGUMENT && strcmp(argv[COUNT_ARGUMENT], "d")) {
		sscanf(argv[COUNT_ARGUMENT], "%d", &player_count);
	}
	
	// Else, the number of players defaults to 8
	else {
		player_count = PLAYER_COUNT;
	}
	
	// The memory to store the players is dynamically allocated
	Player* players = malloc(player_count * sizeof(Player));
	
	int base_rating; // The rating that all players start off at
	
	// If a command line argument was used, the base rating
	//   of each player is set to the argument's value
	if (argc > RATING_ARGUMENT && strcmp(argv[RATING_ARGUMENT], "d")) {
		sscanf(argv[RATING_ARGUMENT], "%d", &base_rating);
	}
	
	// Else, the base rating of each player defaults to 10000
	else {
		base_rating = BASE_RATING;
	}
	
	int* skill_levels = malloc(player_count * sizeof(int)); // The skill level for each player
	
	// If skill levels were passed as arguments, then they are used
	if (argc > SKILL_ARGUMENT) {
		for (int i = 0; i < player_count; i++) {
			// If there was an individual skill level inputted for this player,
			//   then it is used
			if (argc > SKILL_ARGUMENT + i) {
				sscanf(argv[SKILL_ARGUMENT + i], "%d", skill_levels + i);
			}
			
			// Else, the skill level is made equal to the first skill level passed
			else {
				sscanf(argv[SKILL_ARGUMENT], "%d", skill_levels + i);
			}
		}
	}
	
	// Else, the skill levels will range from [1, player_count]
	else {
		for (int i = 0; i < player_count; i++) {
			skill_levels[i] = player_count - i;
		}
	}
	
	// The players are initialised to their skill levels and base ratings
	for (int i = 0; i < player_count; i++) {
		players[i].skill = skill_levels[i];
		players[i].rating = base_rating;
	}
	
	int runs;
	
	// If the command line argument was passed, then the number of runs
	//   takes the value passed
	if (argc > RUNS_ARGUMENT && strcmp(argv[RUNS_ARGUMENT], "d")) {
		sscanf(argv[RUNS_ARGUMENT], "%d", &runs);
	}
	
	// Else, the number of runs defaults to 100
	else {
		runs = RUNS;
	}
	
	// The tournament is held
	for (int i = 0; i < runs; i++) {
		tournament(players, player_count);
	}
	
	// The results are displayed
	for (int i = 0; i < player_count; i++) {
		printf(
			"Player %d:\n  Skill  - %d\n  Rating - %d\n\n",
			i + 1, players[i].skill, players[i].rating
		);
	}
}