#include <time.h>
#include "decklist.h"

#define BUFFER_SIZE (DECK_SIZE * DECK_WIDTH + 1)
#define BENCH_SIZE 5

// The number of cards in hand at the start of the game
const int HAND_SIZE = 7;
// The number of prizes placed
const int PRIZES = 6;
// The maximum number of copies of a card in a deck that isn't basic energy
const int MAX_COPIES = 4;
// Used for return codes to return multiple indices in a single integer
const int CHOICE_SHIFT = 6;

// An index that is guaranteed to never be acceptable
#define ERROR_INDEX ((1 << CHOICE_SHIFT) - 1)

const int EXCEPTION_COUNT = 1;
const char* EXCEPTIONS[] = {
	"Doctor"
};

typedef struct Player {
	CardStore deck;
	CardStore hand;
	CardStore trash;
	CardStore prizes;
	Fighter* active;
	Fighter* bench[BENCH_SIZE];
	char** effects;
	int effect_count;
} Player;

/* Builds a deck and stores its code in the given buffer
 */
void build_deck(char* buffer) {
	char deck[DECK_SIZE][DECK_WIDTH];
	int size = 0;
	
	// The deck is initialised to be empty
	for (int i = 0; i < DECK_SIZE; i++) {
		for (int j = 0; j < DECK_WIDTH; j++) {
			deck[i][j] = ' ';
		}
	}
	
	// The deck is generated from user input
	while (true) {
		printf(
			"\nDeck Size: %d/%d\nFighter (f)\nSupporter (s)\n"
			"Energy (e)\nGenerator (g)\nDone (d)\n\nOption: ",
			size, DECK_SIZE
		);
		
		fgets(buffer, BUFFER_SIZE, stdin);
		char option;
		sscanf(buffer, "%c", &option);
		
		// If the player chose fighter
		if (option == 'f') {
			printf("\n");
			
			for (int i = 0; i < FIGHTER_COUNT; i++) {
				printf("%s (%d)\n", ALL_FIGHTERS[i].name, i);
			}
			
			printf("\nIndex (-1 to cancel): ");
			fgets(buffer, BUFFER_SIZE, stdin);
			int index;
			sscanf(buffer, "%d", &index);
			
			if (index < -1 || index >= FIGHTER_COUNT) {
				printf("\nInvalid index.\n");
			}
			
			// If a valid index was chosen, the player can choose
			//   how many copies of the card that they want
			else if (index != -1) {
				print_fighter(ALL_FIGHTERS + index);
				
				int count = 0;
				
				// The number of copies is counted
				for (int i = 0; i < DECK_SIZE; i++) {
					if (deck[i][0] == 'f') {
						int index2;
						sscanf(deck[i] + 2, "%d", &index2);
						
						if (index == index2) {
							count++;
						}
					}
				}
				
				printf(
					"\nYou have %d copies of this card in your deck. "
					"How many would you like to have in total? ",
					count
				);
				fgets(buffer, BUFFER_SIZE, stdin);
				int count2;
				sscanf(buffer, "%d", &count2);
				
				// If more than 4 copies are requested, the request is rejected
				if (count2 > MAX_COPIES) {
					printf(
						"\nYou may only have %d copies of a non-basic energy card.\n\n",
						MAX_COPIES
					);
				}
				
				// If a negative number of cards is requested, the request is rejected
				else if (count2 < 0) {
					printf("\nInvalid card copy count.\n\n");
				}
				
				else {
					int change = count2 - count;
					
					// If the deck has insufficient space, the request is rejected
					if (size + change > DECK_SIZE) {
						printf("\nInsufficient deck space.\n\n");
					}
					
					// Cards are added to the deck
					else if (change > 0) {
						for (int i = 0; i < DECK_SIZE, change > 0; i++) {
							if (deck[i][0] == ' ') {
								deck[i][0] = 'f';
								sprintf(deck[i] + 2, "%d", index);
								change--;
								size++;
							}
						}
					}
					
					// Cards are removed from the deck
					else if (change < 0) {
						for (int i = 0; i < DECK_SIZE, change < 0; i++) {
							if (deck[i][0] == 'f') {
								int index2;
								sscanf(deck[i] + 2, "%d", &index2);
								
								if (index == index2) {
									for (int j = 0; j < DECK_WIDTH; j++) {
										deck[i][j] = ' ';
									}
									
									change++;
									size--;
								}
							}
						}
					}
				}
			}
		}
		
		// If the player chose supporter
		else if (option == 's') {
			printf("\n");
			
			for (int i = 0; i < SUPPORTER_COUNT; i++) {
				printf("%s (%d)\n", ALL_SUPPORTERS[i].name, i);
			}
			
			printf("\nIndex (-1 to cancel): ");
			fgets(buffer, BUFFER_SIZE, stdin);
			int index;
			sscanf(buffer, "%d", &index);
			
			if (index < -1 || index >= SUPPORTER_COUNT) {
				printf("\nInvalid index.\n");
			}
			
			// If a valid index was chosen, the player can choose
			//   how many copies of the card that they want
			else if (index != -1) {
				print_supporter(ALL_SUPPORTERS + index);
				
				int count = 0;
				
				// The number of copies is counted
				for (int i = 0; i < DECK_SIZE; i++) {
					if (deck[i][0] == 's') {
						int index2;
						sscanf(deck[i] + 2, "%d", &index2);
						
						if (index == index2) {
							count++;
						}
					}
				}
				
				printf(
					"\nYou have %d copies of this card in your deck. "
					"How many would you like to have in total? ",
					count
				);
				fgets(buffer, BUFFER_SIZE, stdin);
				int count2;
				sscanf(buffer, "%d", &count2);
				
				// If more than 4 copies are requested, the request is rejected
				if (count2 > MAX_COPIES) {
					printf(
						"\nYou may only have %d copies of a non-basic energy card.\n\n",
						MAX_COPIES
					);
				}
				
				// If a negative number of cards is requested, the request is rejected
				else if (count2 < 0) {
					printf("\nInvalid card copy count.\n\n");
				}
				
				else {
					int change = count2 - count;
					
					// If the deck has insufficient space, the request is rejected
					if (size + change > DECK_SIZE) {
						printf("\nInsufficient deck space.\n\n");
					}
					
					// Cards are added to the deck
					else if (change > 0) {
						for (int i = 0; i < DECK_SIZE, change > 0; i++) {
							if (deck[i][0] == ' ') {
								deck[i][0] = 's';
								sprintf(deck[i] + 2, "%d", index);
								change--;
								size++;
							}
						}
					}
					
					// Cards are removed from the deck
					else if (change < 0) {
						for (int i = 0; i < DECK_SIZE, change < 0; i++) {
							if (deck[i][0] == 's') {
								int index2;
								sscanf(deck[i] + 2, "%d", &index2);
								
								if (index == index2) {
									for (int j = 0; j < DECK_WIDTH; j++) {
										deck[i][j] = ' ';
									}
									
									change++;
									size--;
								}
							}
						}
					}
				}
			}
		}
		
		// If the player chose energy
		else if (option == 'e') {
			printf("\n");
			
			for (int i = 0; i < ENERGY_COUNT; i++) {
				printf("%s (%d)\n", ALL_ENERGY[i].name, i);
			}
			
			printf("\nIndex (-1 to cancel): ");
			fgets(buffer, BUFFER_SIZE, stdin);
			int index;
			sscanf(buffer, "%d", &index);
			
			if (index < -1 || index >= ENERGY_COUNT) {
				printf("\nInvalid index.\n");
			}
			
			// If a valid index was chosen, the player can choose
			//   how many copies of the card that they want
			else if (index != -1) {
				print_energy(ALL_ENERGY + index);
				
				int count = 0;
				
				// THe number of copies is counted
				for (int i = 0; i < DECK_SIZE; i++) {
					if (deck[i][0] == 'e') {
						int index2;
						sscanf(deck[i] + 2, "%d", &index2);
						
						if (index == index2) {
							count++;
						}
					}
				}
				
				printf(
					"\nYou have %d copies of this card in your deck. "
					"How many would you like to have in total? ",
					count
				);
				fgets(buffer, BUFFER_SIZE, stdin);
				int count2;
				sscanf(buffer, "%d", &count2);
				
				// If more than 4 copies of special energy are requested,
				//   the request is rejected
				if (count2 > MAX_COPIES && strlen(ALL_ENERGY[index].effects)) {
					printf(
						"\nYou may only have %d copies of a non-basic energy card.\n\n",
						MAX_COPIES
					);
				}
				
				// If a negative number of cards is requested, the request is rejected
				else if (count2 < 0) {
					printf("\nInvalid card copy count.\n\n");
				}
				
				else {
					int change = count2 - count;
					
					// If the deck has insufficient space, the request is rejected
					if (size + change > DECK_SIZE) {
						printf("\nInsufficient deck space.\n\n");
					}
					
					// Cards are added to the deck
					else if (change > 0) {
						for (int i = 0; i < DECK_SIZE, change > 0; i++) {
							if (deck[i][0] == ' ') {
								deck[i][0] = 'e';
								sprintf(deck[i] + 2, "%d", index);
								change--;
								size++;
							}
						}
					}
					
					// Cards are removed from the deck
					else if (change < 0) {
						for (int i = 0; i < DECK_SIZE, change < 0; i++) {
							if (deck[i][0] == 'e') {
								int index2;
								sscanf(deck[i] + 2, "%d", &index2);
								
								if (index == index2) {
									for (int j = 0; j < DECK_WIDTH; j++) {
										deck[i][j] = ' ';
									}
									
									change++;
									size--;
								}
							}
						}
					}
				}
			}
		}
		
		// If the player has finished their deck
		else if (option == 'd') {
			// It is rejected if there aren't 60 cards in the deck
			if (size != DECK_SIZE) {
				printf("\nInvalid deck size: %d/%d\n", size, DECK_SIZE);
			}
			
			// It is rejected if there isn't at least one rank 0 fighter
			//   in the deck
			else {
				bool basics = false;
				
				for (int i = 0; i < DECK_SIZE; i++) {
					if (deck[i][0] == 'f') {
						int index;
						sscanf(deck[i] + 2, "%d", &index);
						
						if (!ALL_FIGHTERS[index].rank) {
							basics = true;
							break;
						}
					}
				}
				
				if (basics) {
					break;
				}
				
				else {
					printf("\nA deck must have at least 1 rank 0 fighter.\n");
				}
			}
		}
		
		// Produces a deck consisting of all of cards
		else if (option == 'g') {
			printf("\n");
			
			for (int i = 0 ; i < DECK_LIST_COUNT; i++) {
				printf("%s (%d)\n", DECK_LISTS[i].name, i);
			}
			
			printf("\nIndex (-1 to cancel): ");
			fgets(buffer, BUFFER_SIZE, stdin);
			int index;
			sscanf(buffer, "%d", &index);
			
			if (0 <= index && index < DECK_LIST_COUNT) {
				for (int i = 0; i < DECK_SIZE; i++) {
					for (int j = 0; j < DECK_WIDTH; j++) {
						deck[i][j] = DECK_LISTS[index].list[i][j];
					}
				}
			
				// Deck 0 is reserved for the deck reset
				size = index ? DECK_SIZE : 0;
				
				printf("\n%s\n\nDeck generated.\n", DECK_LISTS[index].description);
			}
		}
		
		// Invalid input
		else {
			printf("\nInvalid input\n");
		}
	}
	
	// The deck code is copied to the buffer
	for (int i = 0; i < DECK_SIZE; i++) {
		for (int j = 0; j < DECK_WIDTH; j++) {
			buffer[i * DECK_WIDTH + j] = deck[i][j] ==  '\0' ? ' ' : deck[i][j];
		}
	}
	
	buffer[BUFFER_SIZE - 1] = '\0';
}

/* Initialises a player and their deck
 */
void player_init(Player* player, const char* deck_code) {
	deck_init(&player->deck, deck_code);
	card_store_init(&player->hand);
	card_store_init(&player->trash);
	card_store_init(&player->prizes);
	player->active = NULL;
	
	for (int i = 0; i < BENCH_SIZE; i++) {
		player->bench[i] = NULL;
	}
	
	player->effects = NULL;
	player->effect_count = 0;
}

/* Returns 1 for heads and 0 for tails
 */
int coin_flip() {
	return rand_int(0, 1);
}

/* Adds an effect to the player
 */
void affect_player(Player* player, const char* effect) {
	int length = strlen(effect) + 1;
	
	// The array of effects is extended
	player->effects = realloc(
		player->effects,
		(player->effect_count + 1) * sizeof(const char*)
	);
	
	// The effect is appended to the array
	player->effects[player->effect_count] = malloc(
		length * sizeof(char)
	);
	
	for (int i = 0; i < length; i++) {
		player->effects[player->effect_count][i] = effect[i];
	}
		
	// The effect count is incremented
	player->effect_count++;
}

/* Removes an effect from the player
   All effects that match the effect given are removed
   A matching effect will have the effect given at the front of the string
 */
void unaffect_player(Player* player, const char* effect) {
	int length = strlen(effect);
	
	// All effects are checked
	for (int i = 0; i < player->effect_count; i++) {
		// If the effect in question is shorter, it cannot match
		if (strlen(player->effects[i]) >= length) {
			bool match = true;
			
			// Check for match
			for (int j = 0; j < length; j++) {
				if (player->effects[i][j] != effect[j]) {
					match = false;
					break;
				}
			}
			
			// If it matches, the effect is removed
			// The currently checked index is decremented, as the next
			//   effect's index was also decremented
			if (match) {
				// The effect is removed
				free(player->effects[i]);
				
				for (int j = i; j < player->effect_count - 1; j++) {
					player->effects[j] = player->effects[j + 1];
				}
				
				// The array's size and count is decremented
				player->effects = realloc(
					player->effects,
					(--player->effect_count) * sizeof(const char*)
				);
				
				i--;
			}
		}
	}
}

/* Prints out all of the effects on a player
 */
void display_effects_p(Player player) {
	printf("\n");
	
	for (int i = 0; i < player.effect_count; i++) {
		printf("%s\n", player.effects[i]);
	}
}

/* Returns true if the player, one of the player's fighters,
     or one of the player's fighter's abilities has the effect
   Uses the same matching rules as unaffect_player()
 */
bool effect_search(Player player, const char* effect) {
	int length = strlen(effect);
	
	// Empty string should always return true
	if (!strcmp(effect, "")) {
		return true;
	}
	
	// All effects on the player are checked
	for (int i = 0; i < player.effect_count; i++) {
		// If the effect in question is shorter, it cannot match
		if (strlen(player.effects[i]) >= length) {
			bool match = true;
			
			// Check for match
			for (int j = 0; j < length; j++) {
				if (player.effects[i][j] != effect[j]) {
					match = false;
					break;
				}
			}
			
			// If it matches, true is returned
			if (match) {
				return true;
			}
		}
	}
	
	// All effects on the player's active fighter are checked
	for (int i = 0; i < player.active->effect_count; i++) {
		// If the effect in question is shorter, it cannot match
		if (strlen(player.active->effects[i]) >= length) {
			bool match = true;
			
			// Check for match
			for (int j = 0; j < length; j++) {
				if (player.active->effects[i][j] != effect[j]) {
					match = false;
					break;
				}
			}
			
			// If it matches, true is returned
			if (match) {
				return true;
			}
		}
	}
	
	// The effects of the player's active fighter's ability are checked
	if (
		player.active->ability
		&& player.active->ability->passive
		&& player.active->uses != player.active->ability->uses
	) {
		int length2 = strlen(player.active->ability->effects);
		
		// A match cannot occur if the effects of the ability are
		//   shorter than the checked effect
		if (length2 >= length) {
			// A match is assumed until proven otherwise
			bool match = true;
			
			for (int i = 0, index = 0; i < length2; i++, index++) {
				// New effect to check
				if (player.active->ability->effects[i] == '\n') {
					// If the old effect matched, true is returned
					if (match) {
						return true;
					}
					
					// The match variable and the effect's index are reset
					match = true;
					index = -1;
				}
				
				else if (
					index < length
					&& match
					&& player.active->ability->effects[i] != effect[index]
				) {
					match = false;
				}
			}
			
			// If the last effect matches, true is returned
			if (match) {
				return true;
			}
		}
	}
	
	// All effects on the player's benched fighters are checked
	for (int i = 0; i < BENCH_SIZE; i++) {
		if (player.bench[i]) {
			for (int j = 0; j < player.bench[i]->effect_count; j++) {
				// If the effect in question is shorter, it cannot match
				if (strlen(player.bench[i]->effects[j]) >= length) {
					bool match = true;
					
					// Check for match
					for (int k = 0; k < length; k++) {
						if (player.bench[i]->effects[j][k] != effect[k]) {
							match = false;
							break;
						}
					}
					
					// If it matches, true is returned
					if (match) {
						return true;
					}
				}
			}
		
			// The effects of the player's benched fighter's ability are checked
			if (
				player.bench[i]->ability
				&& player.bench[i]->ability->passive
				&& player.bench[i]->uses != player.bench[i]->ability->uses
			) {
				int length2 = strlen(player.bench[i]->ability->effects);
				
				// A match cannot occur if the effects of the ability are
				//   shorter than the checked effect
				if (length2 >= length) {
					// A match is assumed until proven otherwise
					bool match = true;
					
					for (int j = 0, index = 0; j < length2; j++, index++) {
						// New effect to check
						if (player.bench[i]->ability->effects[j] == '\n') {
							// If the old effect matched, true is returned
							if (match) {
								return true;
							}
							
							// The match variable and the effect's index are reset
							match = true;
							index = -1;
						}
						
						else if (
							index < length
							&& match
							&& player.bench[i]->ability->effects[j] != effect[index]
						) {
							match = false;
						}
					}
					
					// If the last effect matches, true is returned
					if (match) {
						return true;
					}
				}
			}
		}
	}
	
	return false;
}

/* Calculates the total effect value of an effect type on a player.
   Only effects of the form [effect_type] [effect_value] are considered.
 */
int effect_count_p(Player player, const char* effect) {
	int count = 0;
	
	for (int i = 0; i < player.effect_count; i++) {
		char type[20];
		sscanf(player.effects[i], "%s", type);
		
		// The types match
		if (!strcmp(type, effect)) {
			int value;
			sscanf(player.effects[i], "%*s%d", &value);
			count += value;
		}
	}
	
	return count;
}

/* Calculates the total effect value of an effect type on a player
     and all of the player's fighters.
   Only effects of the form [effect_type] [effect_value] are considered.
 */
int effect_count(Player player, const char* effect) {
	int count =
		effect_count_p(player, effect)
		+ effect_count_f(player.active, effect)
	;
	
	for (int i = 0; i < BENCH_SIZE; i++) {
		if (player.bench[i]) {
			count += effect_count_f(player.bench[i], effect);
		}
	}
	
	return count;
}

/* Plays a fighter card and returns an integer to represent
     the outcome
   -1 means that the operation was cancelled
   0 means that the active fighter was ranked up
   1 - 5 means that the fighter was played onto the bench
 */
int play_fighter(Player* players, Fighter* fighter, CardStore* the_void) {
	// The fighter is displayed
	print_fighter(fighter);
	printf("\nPlay this fighter (y/n)? ");
	char buffer[20];
	fgets(buffer, 20, stdin);
	char answer;
	sscanf(buffer, "%c", &answer);
	
	if (answer == 'y') {
		// Rank 1 and 2 fighters are ranked up into
		if (fighter->rank) {
			printf("\n%s (0)\n", players[0].active->name);
			
			for (int i = 0; i < BENCH_SIZE; i++) {
				if (players[0].bench[i]) {
					printf("%s (%d)\n", players[0].bench[i]->name, i + 1);
				}
			}
			
			printf("\nIndex (-1 to cancel): ");
			char buffer[20];
			fgets(buffer, 20, stdin);
			int index;
			sscanf(buffer, "%d", &index);
			
			// Active
			if (!index) {
				print_fighter(players[0].active);
				printf("\nRank up this fighter (y/n)? ");
				fgets(buffer, 20, stdin);
				char answer;
				sscanf(buffer, "%c", &answer);
				
				if (answer == 'y') {
					if (players[0].active->name == fighter->old_rank) {
						if (players[0].active->first_turn) {
							printf(
								"\nFighters can't rank up on the turn that they were played.\n"
							);
						}
						
						// The base form is discarded and is replaced by the higher rank
						else {
							// The "Banish" ability banishes discarded cards
							CardStore* store =
								effect_search(players[0], "banish")
								? the_void
								: &players[0].trash
							;
							
							store_fighter(
								store,
								rank_up(
									&players[0].active,
									fighter
								)
							);
							
							printf("\nYour fighter ranked up.\n");
							
							return 0;
						}
					}
					
					else {
						printf(
							"\n%s can't rank up into %s.\n",
							players[0].active->name,
							fighter->name
						);
					}
				}
			}
			
			// Bench
			else if (0 < index && index <= BENCH_SIZE && players[0].bench[index - 1]) {
				print_fighter(players[0].bench[index - 1]);
				printf("\nRank up this fighter (y/n)? ");
				fgets(buffer, 20, stdin);
				char answer;
				sscanf(buffer, "%c", &answer);
				
				if (answer == 'y') {
					if (players[0].bench[index - 1]->name == fighter->old_rank) {
						if (players[0].bench[index - 1]->first_turn) {
							printf(
								"\nFighters can't rank up on the turn that they were played.\n"
							);
						}
						
						// The base form is discarded and is replaced by the higher rank
						else {
							store_fighter(
								&players[0].trash,
								rank_up(
									players[0].bench + index - 1,
									fighter
								)
							);
							
							printf("\nYour fighter ranked up.\n");
							
							return index;
						}
					}
					
					else {
						printf(
							"\n%s can't rank up into %s.\n",
							players[0].bench[index - 1]->name,
							fighter->name
						);
					}
				}
			}
		}
		
		// Rank 0 fighters are placed onto the bench directly
		else {
		int index = -1;
		
		for (int i = 0; i < BENCH_SIZE; i++) {
			if (!players[0].bench[i]) {
				index = i;
				break;
			}
		}
		
		if (index < 0) {
			printf("The bench is full.\n");
		}
		
		else {
			printf("Place this fighter on the bench (y/n)? ");
			char buffer[20];
			fgets(buffer, 20, stdin);
			char answer;
			sscanf(buffer, "%c", &answer);
			
			if (answer == 'y') {
				players[0].bench[index] = fighter;
				printf("\nYour played your fighter onto the bench.\n");
				
				return index + 1;
			}
		}
	}
	}
	
	// The operation is cancelled and the fighter is returned to hand
	store_fighter(&players[0].hand, fighter);
	
	return -1;
}

/* Plays a supporter card and returns an integer to represent
     the outcome
   -1 means that the operation was cancelled
   0 means that the supporter was played normally
   1+ is supporter specific and represents choices
 */
int play_supporter(Player* players, Supporter* supporter, CardStore* the_void) {
	// The supporter is displayed
	print_supporter(supporter);
	
	printf("\nPlay this support card (y/n)? ");
	char buffer[50];
	fgets(buffer, 50, stdin);
	char answer;
	sscanf(buffer, "%c", &answer);
	
	if (answer == 'y') {
		printf("\n");
		
		int index;
		int effect_index = 0;
		bool end = false;
		int choices = 0;
		int choice_count = 0;
		CardStore* stores[2] = {
			effect_search(players[0], "banish")
			? the_void
			: &players[0].trash,
			effect_search(players[1], "banish")
			? the_void
			: &players[1].trash
		};
		char flips[10];
		CardStore payment;
		card_store_init(&payment);
		Fighter* payer = NULL;
		bool cancel = false;
		
		while (!end) {
			for (int i = 0;; i++, effect_index++) {
				buffer[i] = supporter->effects[effect_index];
				
				if (buffer[i] == '\n') {
					effect_index++;
					break;
				}
				
				else if (buffer[i] == '\0') {
					end = true;
					break;
				}
			}
			
			char word[20];
			sscanf(buffer, "%s", word);
			
			// Draws cards
			if (!strcmp(word, "draw")) {
				int draw;
				sscanf(buffer, "%*s%d", &draw);
				
				for (int i = 0; i < draw; i++) {
					// The players can't draw cards if the deck is empty
					if (!card_count(players[0].deck)) {
						break;
					}
					
					int counts[3] = {
						players[0].hand.fighter_count,
						players[0].hand.supporter_count,
						players[0].hand.energy_count
					};
					
					random_draw(&players[0].deck, &players[0].hand);
					
					// The card drawn is displayed
					if (counts[0] < players[0].hand.fighter_count) {
						printf(
							"You drew %s.\n",
							players[0].hand.fighters[counts[0]]->name
						);
					}
					
					else if (counts[1] < players[0].hand.supporter_count) {
						printf(
							"You drew %s.\n",
							players[0].hand.supporters[counts[1]]->name
						);
					}
					
					else {
						printf(
							"You drew %s.\n",
							players[0].hand.energy[counts[2]]->name
						);
					}
				}
			}
			
			// Discards cards
			else if (!strcmp(word, "discard")) {
				sscanf(buffer, "%*s%s", word);
				
				// Discards all of the cards in hand
				if (!strcmp(word, "all")) {
					while (card_count(players[0].hand)) {
						rigged_draw(
							&players[0].hand,
							stores[0],
							card_count(players[0].hand) - 1
						);
					}
					
					printf("You discarded your hand.\n");
				}
				
				else {
					int count;
					sscanf(word, "%d", &count);
					CardStore temp_trash;
					card_store_init(&temp_trash);
					
					// Cards are chosen to be discarded from hand
					while (count) {
						printf("Choose a card to discard:");
						print_store(players[0].hand);
						printf("\nIndex (-1 to cancel): ");
						fgets(buffer, 50, stdin);
						sscanf(buffer, "%d", &index);
						
						// Checks that the card is to be discarded
						if (0 <= index && index < players[0].hand.fighter_count) {
							print_fighter(players[0].hand.fighters[index]);
							printf("\nDiscard this card (y/n)? ");
							fgets(buffer, 50, stdin);
							sscanf(buffer, "%c", &answer);
							
							if (answer != 'y') {
								index = ERROR_INDEX;
							}
						}
						
						else if (
							players[0].hand.fighter_count
							<= index
							&& index
							< players[0].hand.fighter_count
							+ players[0].hand.supporter_count
						) {
							print_supporter(
								players[0].hand.supporters[
									index - players[0].hand.fighter_count
								]
							);
							printf("\nDiscard this card (y/n)? ");
							fgets(buffer, 50, stdin);
							sscanf(buffer, "%c", &answer);
							
							if (answer != 'y') {
								index = ERROR_INDEX;
							}
						}
						
						else if (
							players[0].hand.fighter_count
							+ players[0].hand.supporter_count
							<= index
							&& index < card_count(players[0].hand)
						) {
							print_energy(
								players[0].hand.energy[
									index
									- players[0].hand.fighter_count
									- players[0].hand.supporter_count
								]
							);
							printf("\nDiscard this card (y/n)? ");
							fgets(buffer, 50, stdin);
							sscanf(buffer, "%c", &answer);
							
							if (answer != 'y') {
								index = ERROR_INDEX;
							}
						}
						
						// Valid index
						if (0 <= index && index < card_count(players[0].hand)) {
							rigged_draw(&players[0].hand, &temp_trash, index);
							choices |=
								index << choice_count++ * CHOICE_SHIFT
							;
							count--;
						}
						
						// Invalid index returns cards to hand and returns the error index
						else {
							while (card_count(temp_trash)) {
								rigged_draw(
									&temp_trash,
									&players[0].hand,
									card_count(temp_trash) - 1
								);
							}
							
							store_supporter(&players[0].hand, supporter);
							
							return choices | ERROR_INDEX << choice_count * CHOICE_SHIFT;
						}
					}
					
					// Cards are discarded
					while (card_count(temp_trash)) {
						rigged_draw(
							&temp_trash,
							stores[0],
							card_count(temp_trash) - 1
						);
					}
				}
			}
			
			// Forces a benched fighter out
			else if (!strcmp(word, "gust")) {
				if (effect_check(players[1].active, "immune")) {
					printf(
						"%s is immune to all effects and cannot be switched out.\n",
						players[1].active->name
					);
					
					store_supporter(&players[0].hand, supporter);
					
					return -1;
				}
				
				printf("\n");
				
				for (int i = 0; i < BENCH_SIZE; i++) {
					if (players[1].bench[i]) {
						printf("%s (%d)\n", players[1].bench[i]->name, i);
					}
				}
					
				printf("\nIndex (-1 to cancel): ");
				fgets(buffer, 50, stdin);
				sscanf(buffer, "%d", &index);
				
				if (0 <= index && index < BENCH_SIZE) {
					// Valid index
					if (players[1].bench[index]) {
						print_fighter(players[1].bench[index]);
						display_energy(players[1].bench[index]);
						printf("\nForce out this fighter (y/n)? ");
						fgets(buffer, 50, stdin);
						sscanf(buffer, "%c", &answer);
						
						if (answer == 'y') {
							Fighter* temp = players[1].active;
							players[1].active = players[1].bench[index];
							players[1].bench[index] = temp;
							
							choices = index;
						}
						
						else {
							store_supporter(&players[0].hand, supporter);
							
							return -1;
						}
					}
					
					// Invalid index cancels the operation
					else {
						store_supporter(&players[0].hand, supporter);
						
						return -1;
					}
				}
				
				// Invalid operation cancels the operation
				else {
					store_supporter(&players[0].hand, supporter);
					
					return -1;
				}
			}
			
			// Heals a fighter
			else if (!strcmp(word, "heal")) {
				int value;
				sscanf(buffer, "%*s%d", &value);
				
				printf("\n%s (0)\n", players[0].active->name);
				
				for (int i = 0; i < BENCH_SIZE; i++) {
					if (players[0].bench[i]) {
						printf("%s (%d)\n", players[0].bench[i]->name, i + 1);
					}
				}
				
				printf("\nIndex (-1 to cancel): ");
				fgets(buffer, 50, stdin);
				sscanf(buffer, "%d", &index);
				
				// Active fighter
				if (!index) {
					print_fighter(players[0].active);
					display_energy(players[0].active);
					printf("\nHeal this fighter (y/n)? ");
					fgets(buffer, 50, stdin);
					sscanf(buffer, "%c", &answer);
					
					if (answer == 'y') {
						players[0].active->health[0] += value;
						
						if (players[0].active->health[0] > players[0].active->health[1]) {
							value -=
								players[0].active->health[0]
								- players[0].active->health[1]
							;
							players[0].active->health[0] = players[0].active->health[1];
						}
						
						printf(
							"\n%s was healed %d damage.\n",
							players[0].active->name,
							value
						);
					}
					
					else {
						store_supporter(&players[0].hand, supporter);
						
						return -1;
					}
				}
				
				// Benched fighter
				else if (0 < index && index <= BENCH_SIZE) {
					if (players[0].bench[index - 1]) {
						print_fighter(players[0].bench[index - 1]);
						display_energy(players[0].bench[index - 1]);
						printf("\nHeal this fighter (y/n)? ");
						fgets(buffer, 50, stdin);
						sscanf(buffer, "%c", &answer);
						
						if (answer == 'y') {
							players[0].bench[index - 1]->health[0] += value;
						
							if (
								players[0].bench[index - 1]->health[0]
								> players[0].bench[index - 1]->health[1]
							) {
								value -=
									players[0].bench[index - 1]->health[0]
									- players[0].bench[index - 1]->health[1]
								;
								players[0].bench[index - 1]->health[0] =
									players[0].bench[index - 1]->health[1]
								;
							}
							
							choices = index;
						
							printf(
								"\n%s was healed %d damage.\n",
								players[0].bench[index - 1]->name,
								value
							);
						}
						
						else {
							store_supporter(&players[0].hand, supporter);
							
							return -1;
						}
					}
					
					// Invalid index cancels the operation
					else {
						store_supporter(&players[0].hand, supporter);
						
						return -1;
					}
				}
				
				// Invalid operation cancels the operation
				else {
					store_supporter(&players[0].hand, supporter);
					
					return -1;
				}
			}
			
			// Heals a fighter
			// Heals more if energy is dicarded from the fighter
			else if (!strcmp(word, "pay_heal")) {
				int values[2];
				sscanf(buffer, "%*s%d%d", values, values + 1);
				
				printf("\n%s (0)\n", players[0].active->name);
				
				for (int i = 0; i < BENCH_SIZE; i++) {
					if (players[0].bench[i]) {
						printf("%s (%d)\n", players[0].bench[i]->name, i + 1);
					}
				}
				
				printf("\nIndex (-1 to cancel): ");
				fgets(buffer, 50, stdin);
				sscanf(buffer, "%d", &index);
				
				// Active fighter
				if (!index) {
					print_fighter(players[0].active);
					display_energy(players[0].active);
					printf("\nHeal this fighter (y/n)? ");
					fgets(buffer, 50, stdin);
					sscanf(buffer, "%c", &answer);
					
					if (answer == 'y') {
						printf("Energy index (-1 for less healing): ");
						fgets(buffer, 50, stdin);
						int index2;
						sscanf(buffer, "%d", &index2);
						
						// No energy discarded and less healing
						if (index2 == -1) {
							players[0].active->health[0] += values[0];
							
							if (players[0].active->health[0] > players[0].active->health[1]) {
								values[0] -=
									players[0].active->health[0]
									- players[0].active->health[1]
								;
								players[0].active->health[0] = players[0].active->health[1];
							}
							
							choices = ERROR_INDEX << CHOICE_SHIFT;
							
							printf(
								"\n%s was healed %d damage.\n",
								players[0].active->name,
								values[0]
							);
						}
						
						// Energy discarded and more healing
						else if (0 <= index2 && index2 < players[0].active->energy_count) {
							print_energy(players[0].active->energy[index2]);
							printf("\nDiscard this energy card (y/n)? ");
							fgets(buffer, 50, stdin);
							sscanf(buffer, "%c", &answer);
							
							if (answer == 'y') {
								store_energy(
									stores[0],
									pay(
										players[0].active,
										index2
									)
								);
								
								printf(
									"\nYou discarded %s.\n",
									stores[0]->energy[
										stores[0]->energy_count - 1
									]->name
								);
								
								players[0].active->health[0] += values[1];
								
								if (players[0].active->health[0] > players[0].active->health[1]) {
									values[1] -=
										players[0].active->health[0]
										- players[0].active->health[1]
									;
									players[0].active->health[0] = players[0].active->health[1];
								}
								
								choices = index2 << CHOICE_SHIFT;
								
								printf(
									"%s was healed %d damage.\n",
									players[0].active->name,
									values[1]
								);
							}
							
							// Cancel
							else {
								store_supporter(&players[0].hand, supporter);
								
								return -1;
							}
						}
						
						// Cancel
						else {
							store_supporter(&players[0].hand, supporter);
							
							return -1;
						}
					}
					
					else {
						store_supporter(&players[0].hand, supporter);
						
						return -1;
					}
				}
				
				// Benched fighter
				else if (0 < index && index <= BENCH_SIZE) {
					if (players[0].bench[index - 1]) {
						print_fighter(players[0].bench[index - 1]);
						display_energy(players[0].bench[index - 1]);
						printf("\nHeal this fighter (y/n)? ");
						fgets(buffer, 50, stdin);
						sscanf(buffer, "%c", &answer);
						
						if (answer == 'y') {
							printf("Energy index (-1 for less healing): ");
							fgets(buffer, 50, stdin);
							int index2;
							sscanf(buffer, "%d", &index2);
							
							// No energy discarded and less healing
							if (index2 == -1) {
								players[0].bench[index - 1]->health[0] += values[0];
								
								if (players[0].bench[index - 1]->health[0] > players[0].bench[index - 1]->health[1]) {
									values[0] -=
										players[0].bench[index - 1]->health[0]
										- players[0].bench[index - 1]->health[1]
									;
									players[0].bench[index - 1]->health[0] = players[0].bench[index - 1]->health[1];
								}
								
								choices = (ERROR_INDEX << CHOICE_SHIFT) + index;
								
								printf(
									"\n%s was healed %d damage.\n",
									players[0].bench[index - 1]->name,
									values[0]
								);
							}
							
							// Energy discarded and more healing
							else if (0 <= index2 && index2 < players[0].bench[index - 1]->energy_count) {
								print_energy(players[0].bench[index - 1]->energy[index2]);
								printf("\nDiscard this energy card (y/n)? ");
								fgets(buffer, 50, stdin);
								sscanf(buffer, "%c", &answer);
								
								if (answer == 'y') {
									store_energy(
										stores[0],
										pay(
											players[0].active,
											index2
										)
									);
									
									printf(
										"\nYou discarded %s.\n",
										stores[0]->energy[
											stores[0]->energy_count - 1
										]->name
									);
									
									players[0].bench[index - 1]->health[0] += values[1];
									
									if (players[0].bench[index - 1]->health[0] > players[0].bench[index - 1]->health[1]) {
										values[1] -=
											players[0].bench[index - 1]->health[0]
											- players[0].bench[index - 1]->health[1]
										;
										players[0].bench[index - 1]->health[0] = players[0].bench[index - 1]->health[1];
									}
									
									choices = (index2 << CHOICE_SHIFT) + index;
									
									printf(
										"%s was healed %d damage.\n",
										players[0].bench[index - 1]->name,
										values[1]
									);
								}
								
								// Cancel
								else {
									store_supporter(&players[0].hand, supporter);
									
									return -1;
								}
							}
							
							// Cancel
							else {
								store_supporter(&players[0].hand, supporter);
								
								return -1;
							}
						}
						
						else {
							store_supporter(&players[0].hand, supporter);
							
							return -1;
						}
					}
					
					// Invalid index cancels the operation
					else {
						store_supporter(&players[0].hand, supporter);
						
						return -1;
					}
				}
				
				// Invalid operation cancels the operation
				else {
					store_supporter(&players[0].hand, supporter);
					
					return -1;
				}
			}
		
			// Increases a player's attack power
			else if (!strcmp(word, "power")) {
				// The string containing the effect is created
				int power;
				sscanf(buffer, "%*s%d", &power);
				
				int power_length = strlen("power ");
				
				char* power_effect = malloc(
					(power_length + 2) * sizeof(char)
				);
				
				for (int i = 0; i < power_length; i++) {
					power_effect[i] = "power "[i];
				}
				
				sprintf(power_effect + power_length, "%d", power);
				
				// The effect is applied to the player
				affect_player(players, power_effect);
				
				// The string is freed after use
				free(power_effect);
				
				printf(
					"Your attacks deal %d more damage this turn.\n",
					power
				);
			}
		
			// Fully heals all fighters and removes all effects and energy from them
			else if (!strcmp(word, "hospital")) {
				// Effects are removed from the active fighter
				unaffect_fighter(players[0].active, "");
				
				// Energy is removed from the active fighter
				while (players[0].active->energy_count) {
					store_energy(
						stores[0],
						pay(
							players[0].active,
							players[0].active->energy_count - 1
						)
					);
				}
				
				// The active fighter is fully healed
				players[0].active->health[0] = players[0].active->health[1];
				
				// Benched fighters
				for (int i = 0; i < BENCH_SIZE; i++) {
					if (players[0].bench[i]) {
						// Energy is removed from the fighter
						while (players[0].bench[i]->energy_count) {
							store_energy(
								stores[0],
								pay(
									players[0].bench[i],
									players[0].bench[i]->energy_count - 1
								)
							);
						}
						
						// The fighter is fully healed
						players[0].bench[i]->health[0] = players[0].bench[i]->health[1];
					}
				}
				
				printf(
					"Your fighters were fully healed and "
					"had all energy discarded from "
					"them. Your active fighter's "
					"effects were cleared.\n"
				);
			}

			// Flips coins to determine the effect
			else if (!strcmp(word, "flip")) {
				int flip_count;
				sscanf(buffer, "%*s%d", &flip_count);
				
				for (int i = 0; i < flip_count; i++) {
					flips[i] =
						coin_flip()
						? 'h'
						: 't'
					;
				}
				
				flips[flip_count] = '\0';
				
				printf("Coin flips: %s\n", flips);
			}
			
			// Heads
			else if (!strcmp(word, "h") && !strncmp(flips, "h", 1)) {
				sscanf(buffer, "%*s%s", word);
				
				if (!strcmp(word, "draw")) {
					int draw;
					sscanf(buffer, "%*s%*s%d", &draw);
					
					for (int i = 0; i < draw; i++) {
						// The players can't draw cards if the deck is empty
						if (!card_count(players[0].deck)) {
							break;
						}
						
						int counts[3] = {
							players[0].hand.fighter_count,
							players[0].hand.supporter_count,
							players[0].hand.energy_count
						};
						
						random_draw(&players[0].deck, &players[0].hand);
						
						// The card drawn is displayed
						if (counts[0] < players[0].hand.fighter_count) {
							printf(
								"You drew %s.\n",
								players[0].hand.fighters[counts[0]]->name
							);
						}
						
						else if (counts[1] < players[0].hand.supporter_count) {
							printf(
								"You drew %s.\n",
								players[0].hand.supporters[counts[1]]->name
							);
						}
						
						else {
							printf(
								"You drew %s.\n",
								players[0].hand.energy[counts[2]]->name
							);
						}
					}
				}
			}
			
			// Tails
			else if (!strcmp(word, "t") && !strncmp(flips, "t", 1)) {
				sscanf(buffer, "%*s%s", word);
				
				if (!strcmp(word, "draw")) {
					int draw;
					sscanf(buffer, "%*s%*s%d", &draw);
					
					for (int i = 0; i < draw; i++) {
						// The players can't draw cards if the deck is empty
						if (!card_count(players[0].deck)) {
							break;
						}
						
						int counts[3] = {
							players[0].hand.fighter_count,
							players[0].hand.supporter_count,
							players[0].hand.energy_count
						};
						
						random_draw(&players[0].deck, &players[0].hand);
						
						// The card drawn is displayed
						if (counts[0] < players[0].hand.fighter_count) {
							printf(
								"You drew %s.\n",
								players[0].hand.fighters[counts[0]]->name
							);
						}
						
						else if (counts[1] < players[0].hand.supporter_count) {
							printf(
								"You drew %s.\n",
								players[0].hand.supporters[counts[1]]->name
							);
						}
						
						else {
							printf(
								"You drew %s.\n",
								players[0].hand.energy[counts[2]]->name
							);
						}
					}
				}
			}

			// Shuffles cards into the deck
			else if (!strcmp(word, "shuffle")) {
				sscanf(buffer, "%*s%s", word);
				
				// Shuffles the entire hand into the deck
				if (!strcmp(word, "all")) {
					while (card_count(players[0].hand)) {
						rigged_draw(
							&players[0].hand,
							&players[0].deck,
							card_count(players[0].hand) - 1
						);
					}
					
					printf("You shuffled your hand into your deck.\n");
				}
			}
		
			// Discards energy attached to an enemy fighter
			else if (!strcmp(word, "drain")) {
				int drain;
				sscanf(buffer, "%*s%d", &drain);
				
				printf(
					"\nChoose a fighter to remove energy from:\n%s (0)\n",
					players[1].active->name
				);
				
				for (int i = 0; i < BENCH_SIZE; i++) {
					if (players[1].bench[i]) {
						printf(
							"%s (%d)\n",
							players[1].bench[i]->name,
							i + 1
						);
					}
				}
				
				printf("\nIndex (-1 to cancel): ");
				fgets(buffer, 50, stdin);
				sscanf(buffer, "%d", &index);
				
				Fighter* fighter = NULL;
				
				if (!index) {
					fighter = players[1].active;
					choice_count++;
				}
				
				else if (0 < index && index <= BENCH_SIZE && players[1].bench[index - 1]) {
					fighter = players[1].bench[index - 1];
					choices |= index << choice_count++ * CHOICE_SHIFT;
				}
				
				if (fighter && effect_check(fighter, "immune")) {
					printf(
						"\n%s is immune to all effects and cannot be drained of energy.\n",
						fighter->name
					);
					
					fighter = NULL;
				}
				
				if (fighter) {
					print_fighter(fighter);
					
					if (fighter == players[1].active) {
						printf(
							"\nStamina: %d\n",
							effect_count_f(fighter, "stamina")
						);
					}
					
					display_energy(fighter);
					
					printf("\nChoose this fighter (y/n)? ");
					fgets(buffer, 50, stdin);
					char answer;
					sscanf(buffer, "%c", &answer);
					
					if (answer == 'y') {
						CardStore temp_trash;
						card_store_init(&temp_trash);
						
						for (int i = 0; i < drain && fighter->energy_count; i++) {
							display_energy(fighter);
							printf("\nEnergy index (-1 to cancel): ");
							fgets(buffer, 50, stdin);
							sscanf(buffer, "%d", &index);
							
							if (0 <= index && index < fighter->energy_count) {
								store_energy(
									&temp_trash,
									pay(
										fighter,
										index
									)
								);
								
								choices |= index << choice_count++ * CHOICE_SHIFT;
							}
							
							else {
								cancel = true;
								break;
							}
						}
						
						// Energy is refunded to both fighters if cancelled and applicable
						if (cancel) {
							// Energy is returned to the opponent's fighter
							while (temp_trash.energy_count) {
								attach(
									fighter,
									get_energy(
										&temp_trash,
										temp_trash.energy_count - 1
									)
								);
							}
							
							// The supporter card is returned to hand
							store_supporter(&players[0].hand, supporter);
							
							// If the energy drainage was paid for, the payer is refunded
							if (payer) {
								// Energy is refunded
								while (payment.energy_count) {
									attach(
										payer,
										get_energy(
											&payment,
											payment.energy_count - 1
										)
									);
								}
								
								// The cancellation is reported
								return choices | ERROR_INDEX << choice_count * CHOICE_SHIFT;
							}
							
							// Else, the function simply returns
							else {
								return choices | ERROR_INDEX << choice_count * CHOICE_SHIFT;
							}
						}
						
						else {
							while (temp_trash.energy_count) {
								rigged_draw(
									&temp_trash,
									stores[1],
									temp_trash.energy_count - 1
								);
							}
							
							printf(
								"\nEnergy was drained from %s.\n",
								fighter->name
							);
						}
					}
					
					// Cancel
					else {
						// The supporter card is returned to hand
						store_supporter(&players[0].hand, supporter);
							
						// If the energy drainage was paid for, the payer is refunded
						if (payer) {
							// Energy is refunded
							while (payment.energy_count) {
								attach(
									payer,
									get_energy(
										&payment,
										payment.energy_count - 1
									)
								);
							}
									
							// The cancellation is reported
							return choices | ERROR_INDEX << choice_count * CHOICE_SHIFT;
						}
						
						// Else, the function simply returns
						else {
							return -1;
						}
					}
				}
				
				// Cancel
				else {
					// The supporter card is returned to hand
					store_supporter(&players[0].hand, supporter);
					
					// If the energy drainage was paid for, the payer is refunded
					if (payer) {
						// Energy is refunded
						while (payment.energy_count) {
							attach(
								payer,
								get_energy(
									&payment,
									payment.energy_count - 1
								)
							);
						}
								
						// The cancellation is reported
						return choices | ERROR_INDEX << choice_count * CHOICE_SHIFT;
					}
					
					// Else, the function simply returns
					else {
						return -1;
					}
				}
			}
		
			// Discards energy cards attached to the player's fighter
			else if (!strcmp(word, "pay")) {
				int price;
				sscanf(buffer, "%*s%d", &price);
				
				printf(
					"\nChoose a fighter to pay with:\n%s (0)\n",
					players[0].active->name
				);
				
				for (int i = 0; i < BENCH_SIZE; i++) {
					if (players[0].bench[i]) {
						printf(
							"&s (%d)\n",
							players[0].bench[i]->name,
							i + 1
						);
					}
				}
				
				printf("\nIndex (-1 to cancel): ");
				fgets(buffer, 50, stdin);
				sscanf(buffer, "%d", &index);
				
				if (!index) {
					payer = players[0].active;
					choice_count++;
				}
				
				else if (0 < index && index <= BENCH_SIZE && players[0].bench[index - 1]) {
					payer = players[0].bench[index - 1];
					choices |= index << choice_count++ * CHOICE_SHIFT;
				}
				
				// Valid index
				if (payer) {
					print_fighter(payer);
					
					if (payer == players[0].active) {
						printf(
							"\nPower: %d\nStamina: %d\n",
							effect_count_f(
								payer,
								"power"
							),
							effect_count_f(
								payer,
								"stamina"
							)
						);
					}
					
					display_energy(payer);
					
					printf("\nChoose this fighter (y/n)? ");
					fgets(buffer, 50, stdin);
					char answer;
					sscanf(buffer, "%c", &answer);
					
					if (answer == 'y') {
						if (payer->energy_count < price) {
							printf("\nInsufficient energy.\n");
							
							// The operation is cancelled and the supporter is returned to hand
							store_supporter(&players[0].hand, supporter);
							
							return -1;
						}
						
						else {
							CardStore temp_trash;
							card_store_init(&temp_trash);
							
							for (int i = 0; i < price; i++) {
								display_energy(payer);
								printf("\nEnergy index (-1 to cancel): ");
								fgets(buffer, 50, stdin);
								sscanf(buffer, "%d", &index);
								
								if (0 <= index && index < payer->energy_count) {
									store_energy(
										&temp_trash,
										pay(
											payer,
											index
										)
									);
									
									choices |= index << choice_count++ * CHOICE_SHIFT;
								}
								
								else {
									cancel = true;
									break;
								}
							}
							
							// Energy is refunded
							if (cancel) {
								while (temp_trash.energy_count) {
									attach(
										payer,
										get_energy(
											&temp_trash,
											temp_trash.energy_count - 1
										)
									);
								}
								
								// The operation is cancelled and the supporter is returned to hand
								store_supporter(&players[0].hand, supporter);
								
								return choices | ERROR_INDEX << choice_count * CHOICE_SHIFT;
							}
							
							// Energy is stored for removal
							else {
								while (temp_trash.energy_count) {
									rigged_draw(
										&temp_trash,
										&payment,
										temp_trash.energy_count - 1
									);
								}
							}
						}
					}
					
					else {
						// The operation is cancelled and the supporter is returned to hand
						store_supporter(&players[0].hand, supporter);
						
						return -1;
					}
				}
				
				else {
					// The operation is cancelled and the supporter is returned to hand
					store_supporter(&players[0].hand, supporter);
					
					return -1;
				}
			}
			
			// Searches a card store for cards and adds them to hand
			else if (!strcmp(word, "search")) {
				sscanf(buffer, "%*s%s", word);
				
				// Searches the trash
				if (!strcmp(word, "trash")) {
					sscanf(buffer, "%*s%*s%s", word);
					
					if (!strcmp(word, "energy")) {
						int count;
						sscanf(buffer, "%*s%*s%*s%d", &count);
						int i;
					
						// Searches the trash for energy as there is energy in the trash
						for (i = 0; i < count && players[0].trash.energy_count; i++) {
							// Loop to get an answer
							while (true) {
								printf("\nChoose a card to draw:\n");
								
								for (int j = 0; j < players[0].trash.energy_count; j++) {
									printf(
										"%s (%d)\n",
										players[0].trash.energy[j]->name,
										j
									);
								}
								
								printf("\nIndex: ");
								fgets(buffer, 50, stdin);
								sscanf(buffer, "%d", &index);
								
								// Energy
								if (0 <= index && index < players[0].trash.energy_count) {
									print_energy(players[0].trash.energy[index]);
									printf("\nChoose this card (y/n)? ");
									fgets(buffer, 50, stdin);
									char answer;
									sscanf(buffer, "%c", &answer);
									
									if (answer == 'y') {
										break;
									}
								}
								
								// Invalid index
								else {
									printf("\nInvalid index.\n");
								}
							}
							
							index +=
								players[0].trash.fighter_count
								+ players[0].trash.supporter_count
							;
							
							rigged_draw(
								&players[0].trash,
								&players[0].hand,
								index
							);
							
							choices |= index << choice_count++ * CHOICE_SHIFT;
						}
						
						if (i != count) {
							printf("\nYour trash is depleted of energy cards.\n");
						}
						
						printf(
							"\nYou drew %d card%s.\n",
							i,
							i == 1 ? "" : "s"
						);
					}
				}
				
				// Searches the deck for a fighter
				else if (!strcmp(word, "fighter")) {
					int count;
					sscanf(buffer, "%*s%*s%d", &count);
					int i;
					
					// Searches the deck for cards as long as there are fighters
					for (i = 0; i < count && players[0].deck.fighter_count; i++) {
						// Loop to get an answer
						while (true) {
							printf("\nChoose a card to draw:\n");
							
							for (int j = 0; j < players[0].deck.fighter_count; j++) {
								printf(
									"%s (%d)\n",
									players[0].deck.fighters[j]->name,
									j
								);
							}
							
							printf("\nIndex: ");
							fgets(buffer, 50, stdin);
							sscanf(buffer, "%d", &index);
							
							// Fighter
							if (0 <= index && index < players[0].deck.fighter_count) {
								print_fighter(players[0].deck.fighters[index]);
								printf("\nChoose this card (y/n)? ");
								fgets(buffer, 50, stdin);
								char answer;
								sscanf(buffer, "%c", &answer);
								
								if (answer == 'y') {
									break;
								}
							}
							
							// Invalid index
							else {
								printf("\nInvalid index.\n");
							}
						}
						
						rigged_draw(
							&players[0].deck,
							&players[0].hand,
							index
						);
						
						choices |= index << choice_count++ * CHOICE_SHIFT;
					}
					
					if (i != count) {
						printf("\nYour deck is depleted of fighter cards.\n");
					}
					
					printf(
						"\nYou drew %d card%s.\n",
						i,
						i == 1 ? "" : "s"
					);
				}
				
				// Searches the deck for a fighter that ranks up from
				//   one of the player's fighters and ranks it up
				else if (!strcmp(word, "rank_up")) {
					int count;
					sscanf(buffer, "%*s%*s%d", &count);
					int i;
					
					bool playable = false;
					
					for (int i = 0; i < players[0].deck.fighter_count && !playable; i++) {
						if (players[0].deck.fighters[i]->rank) {
							if (
								!strcmp(
									players[0].deck.fighters[i]->old_rank,
									players[0].active->name
								)
							) {
								playable = true;
							}
							
							for (int j = 0; j < BENCH_SIZE && !playable; j++) {
								if (players[0].bench[j]) {
									if (
										!strcmp(
											players[0].deck.fighters[i]->old_rank,
											players[0].bench[j]->name
										)
									) {
										playable = true;
									}
								}
							}
						}
					}
					
					// If the card can be played, it is played
					if (playable) {
						// Searches the deck for cards as long as there are fighters
						for (i = 0; i < count && players[0].deck.fighter_count; i++) {
							// Loop to get an answer
							while (true) {
								printf("\nChoose a card to draw:\n");
								
								for (int j = 0; j < players[0].deck.fighter_count; j++) {
									printf(
										"%s (%d)\n",
										players[0].deck.fighters[j]->name,
										j
									);
								}
								
								printf("\nIndex: ");
								fgets(buffer, 50, stdin);
								sscanf(buffer, "%d", &index);
								
								// Fighter
								if (0 <= index && index < players[0].deck.fighter_count) {
									print_fighter(players[0].deck.fighters[index]);
									printf("\nChoose this card (y/n)? ");
									fgets(buffer, 50, stdin);
									char answer;
									sscanf(buffer, "%c", &answer);
									
									// Checks to see if the fighter can be used for a rank up
									if (answer == 'y') {
										playable = false;
										
										if (
											!strcmp(
												players[0].deck.fighters[index]->old_rank,
												players[0].active->name
											)
										) {
											playable = true;
										}
										
										for (int j = 0; j < BENCH_SIZE && !playable; j++) {
											if (players[0].bench[j]) {
												if (
													!strcmp(
														players[0].deck.fighters[index]->old_rank,
														players[0].bench[j]->name
													)
												) {
													playable = true;
												}
											}
										}
										
										if (playable) {
											break;
										}
										
										else {
											printf(
												"\nThe fighter must rank up from one of "
												"your fighters:\n%s (0)\n",
												players[0].active->name
											);
											
											for (int j = 0; j < BENCH_SIZE; j++) {
												if (players[0].bench[j]) {
													printf(
														"%s (%d)\n",
														players[0].bench[j]->name,
														j + 1
													);
												}
											}
										}
									}
								}
								
								// Invalid index
								else {
									printf("\nInvalid index.\n");
								}
							}
							
							Fighter* new_form = get_fighter(
								&players[0].deck,
								index
							);
							
							choices |= index << choice_count++ * CHOICE_SHIFT;
							
							// Loop to get input
							while (true) {
								printf(
									"\nChoose a fighter to rank up:\n%s (0)\n",
									players[0].active->name
								);
								
								for (int j = 0; j < BENCH_SIZE; j++) {
									if (players[0].bench[j]) {
										printf(
											"%s (%d)\n",
											players[0].bench[j]->name,
											j + 1
										);
									}
								}
								
								printf("\nIndex: ");
								fgets(buffer, 50, stdin);
								sscanf(buffer, "%d", &index);
								
								Fighter** old_form = NULL;
								
								if (!index) {
									old_form = &players[0].active;
								}
								
								else if (
									0 < index && index <= BENCH_SIZE && players[0].bench[index - 1]
								) {
									old_form = &players[0].bench[index - 1];
								}
								
								// Valid index
								if (old_form) {
									// The rank up is valid
									if (!strcmp(new_form->old_rank, (*old_form)->name)) {
										print_fighter(*old_form);
										
										if (*old_form == players[0].active) {
											printf(
												"\nPower: %d\nStamina: %d\n",
												effect_count_f(
													*old_form,
													"power"
												),
												effect_count_f(
													*old_form,
													"stamina"
												)
											);
										}
										
										display_energy(*old_form);
										printf("\nRank up this fighter (y/n)? ");
										fgets(buffer, 50, stdin);
										char answer;
										sscanf(buffer, "%c", &answer);
										
										if (answer == 'y') {
											printf(
												"\n%s was ranked up into %s.\n",
												(*old_form)->name,
												new_form->name
											);
												
											store_fighter(
												stores[0],
												rank_up(
													old_form,
													new_form
												)
											);
											
											choices |= index << choice_count++ * CHOICE_SHIFT;
											
											break;
										}
									}
									
									// Invalid rank up
									else {
										printf(
											"\n%s can't rank up into %s.\n",
											(*old_form)->name,
											new_form->name
										);
										
										print_fighter(*old_form);
										print_fighter(new_form);
									}
								}
								
								// Invalid index
								else {
									printf("\nInvalid index.\n");
								}
							}
						}
					}
					
					// Else, no fighter is gotten and the card's functionality ceases
					// The player is not refunded
					else {
						end = true;
						printf("\nYour deck:");
						print_store(players[0].deck);
						printf("\nThere are no valid cards to draw...\n");
					}
				}
				
				// Searches the deck for any card
				else {
					int count;
					sscanf(buffer, "%*s%d", &count);
					int i;
					
					// Searches the deck for cards as long as it is not empty
					for (i = 0; i < count && card_count(players[0].deck); i++) {
						// Loop to get an answer
						while (true) {
							printf("\nChoose a card to draw:");
							print_store(players[0].deck);
							printf("\nIndex: ");
							fgets(buffer, 50, stdin);
							sscanf(buffer, "%d", &index);
							
							// Valid index
							if (0 <= index && index < card_count(players[0].deck)) {
								// Fighter
								if (index < players[0].deck.fighter_count) {
									print_fighter(players[0].deck.fighters[index]);
									printf("\nChoose this card (y/n)? ");
									fgets(buffer, 50, stdin);
									char answer;
									sscanf(buffer, "%c", &answer);
									
									if (answer == 'y') {
										break;
									}
								}
								
								else {
									index -= players[0].deck.fighter_count;
									
									// Supporter
									if (index < players[0].deck.supporter_count) {
										print_supporter(players[0].deck.supporters[index]);
										printf("\nChoose this card (y/n)? ");
										fgets(buffer, 50, stdin);
										char answer;
										sscanf(buffer, "%c", &answer);
										
										if (answer == 'y') {
											index += players[0].deck.fighter_count;
											break;
										}
									}
									
									// Energy
									else {
										index -= players[0].deck.supporter_count;
										
										print_energy(players[0].deck.energy[index]);
										printf("\nChoose this card (y/n)? ");
										fgets(buffer, 50, stdin);
										char answer;
										sscanf(buffer, "%c", &answer);
										
										if (answer == 'y') {
											index +=
												players[0].deck.fighter_count
												+ players[0].deck.supporter_count
											;
											
											break;
										}
									}
								}
							}
							
							// Invalid index
							else {
								printf("\nInvalid index.\n");
							}
						}
						
						rigged_draw(
							&players[0].deck,
							&players[0].hand,
							index
						);
						
						choices |= index << choice_count++ * CHOICE_SHIFT;
					}
					
					if (i != count) {
						printf("\nYour deck is empty.\n");
					}
					
					printf(
						"\nYou drew %d card%s.\n",
						i,
						i == 1 ? "" : "s"
					);
				}
			}
		}
		
		// The payment is taken
		while (payment.energy_count) {
			rigged_draw(
				&payment,
				stores[0],
				payment.energy_count - 1
			);
		}
		
		// The supporter is discarded
		store_supporter(stores[0], supporter);
		
		return choices;
	}
	
	// The operation is cancelled and the supporter is returned to hand
	store_supporter(&players[0].hand, supporter);
	
	return -1;
}

/* Plays an energy card and returns an integer to represent
     the outcome
   -1 means that the operation was cancelled
   0 means that the energy was attached to the active fighter
   1-5 means that the energy was attached to a benched fighter
 */
int play_energy(Player* players, Energy* energy) {
	// The energy is displayed
	print_energy(energy);
	printf("\nAttach this energy card to one of your fighters (y/n)? ");
	char buffer[20];
	fgets(buffer, 20, stdin);
	char answer;
	sscanf(buffer, "%c", &answer);
	
	// A fighter to attach the energy to is selected
	if (answer == 'y') {
		printf("\n%s (0)\n", players[0].active->name);
		
		for (int i = 0; i < BENCH_SIZE; i++) {
			if (players[0].bench[i]) {
				printf("%s (%d)\n", players[0].bench[i]->name, i + 1);
			}
		}
		
		printf("\nIndex (-1 to cancel): ");
		fgets(buffer, 20, stdin);
		int index;
		sscanf(buffer, "%d", &index);
		
		if (!index) {
			print_fighter(players[0].active);
			display_energy(players[0].active);
			printf("\nAttach energy to this fighter (y/n)? ");
			fgets(buffer, 20, stdin);
			sscanf(buffer, "%c", &answer);
			
			if (answer == 'y') {
				attach(players[0].active, energy);
				printf("\nYou attached energy to your fighter.\n");
				
				return 0;
			}
		}
		
		else if (0 < index && index <= BENCH_SIZE) {
			if (players[0].bench[index - 1]) {
				print_fighter(players[0].bench[index - 1]);
				display_energy(players[0].bench[index - 1]);
				printf("\nAttach energy to this fighter (y/n)? ");
				fgets(buffer, 20, stdin);
				sscanf(buffer, "%c", &answer);
				
				if (answer == 'y') {
					attach(players[0].bench[index - 1], energy);
					printf("\nYou attached energy to your fighter.\n");
					
					return index;
				}
			}
		}
	}
	
	// The operation is cancelled and the energy is returned to hand
	store_energy(&players[0].hand, energy);
	
	return -1;
}

/* Frees all memory dynamically allocated to the players
 */
void free_players(Player* players) {
	for (int i = 0; i < 2; i++) {
		// The stores are freed
		free_store(&players[i].deck);
		free_store(&players[i].hand);
		free_store(&players[i].trash);
		free_store(&players[i].prizes);
		
		// The active fighter is freed
		if (players[i].active) {
			// The energy is freed
			while (players[i].active->energy_count) {
				free(pay(players[i].active, players[i].active->energy_count - 1));
			}
			
			// The effect array is freed
			reset_fighter(players[i].active);
			
			// The fighter, itself, is freed
			free(players[i].active);
			
			// The fighter is set to NULL
			players[i].active = NULL;
		}
		
		// The benched fighters are freed
		for (int j = 0; j < BENCH_SIZE; j++) {
			if (players[i].bench[j]) {
				// The energy is freed
				while (players[i].bench[j]->energy_count) {
					free(pay(players[i].bench[j], players[i].bench[j]->energy_count - 1));
				}
				
				// The effect array is freed
				reset_fighter(players[i].bench[j]);
				
				// The fighter, itself, is freed
				free(players[i].bench[j]);
				
				// The fighter is set to NULL
				players[i].bench[j] = NULL;
			}
		}
		
		unaffect_player(players + i, "");
	}
}

/* Returns true if the operation was successful
 */
bool play_test(int bitmask, const char* name) {
	for (int i = 0; i < EXCEPTION_COUNT; i++) {
		if (!strcmp(EXCEPTIONS[i], name)) {
			return true;
		}
	}
	
	bool played = true;
	
	for (int i = 0; i < sizeof(int) * 8 / CHOICE_SHIFT; i++) {
		if ((bitmask >> i * CHOICE_SHIFT & ERROR_INDEX) == ERROR_INDEX) {
			played = false;
		}
	}
	
	return played;
}