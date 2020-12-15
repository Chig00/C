#include <stdlib.h>
#include "fighter.h"
#include "supporter.h"

#define DECK_SIZE 60
#define DECK_WIDTH 10

/* A structure that defines card storages
 */
typedef struct CardStore {
	Fighter** fighters;
	Supporter** supporters;
	Energy** energy;
	int fighter_count;
	int supporter_count;
	int energy_count;
} CardStore;

/* Returns the total number of cards in the store
 */
int card_count(CardStore card_store) {
	return card_store.fighter_count + card_store.supporter_count + card_store.energy_count;
}

/* Initialises an empty card store
 */
void card_store_init(CardStore* card_store) {
	card_store->fighters = NULL;
	card_store->supporters = NULL;
	card_store->energy = NULL;
	card_store->fighter_count = 0;
	card_store->supporter_count = 0;
	card_store->energy_count = 0;
}

/* Initialises a deck with a deck code
 */
void deck_init(CardStore* deck, const char* deck_code) {
	card_store_init(deck);
	
	// The deck is loaded
	for (int i = 0; i < DECK_SIZE; i++) {
		// The type of card and its index are obtained
		char type;
		int index;
		sscanf(deck_code + i * DECK_WIDTH, "%c%d", &type, &index);
		
		// Fighter card
		if (type == 'f') {
			// The fighter array is extended
			deck->fighters = realloc(
				deck->fighters,
				(deck->fighter_count + 1) * sizeof(Fighter*)
			);
			
			// The new fighter card has memory allocated to it
			deck->fighters[deck->fighter_count] = malloc(sizeof(Fighter));
			
			// The fighter is initialised
			*deck->fighters[deck->fighter_count] = ALL_FIGHTERS[index];
			
			// The number of fighter cards is incremented
			deck->fighter_count++;
		}
		
		// Supporter card
		else if (type == 's') {
			// The supporter array is extended
			deck->supporters = realloc(
				deck->supporters,
				(deck->supporter_count + 1) * sizeof(Supporter*)
			);
			
			// The new supporter card has memory allocated to it
			deck->supporters[deck->supporter_count] = malloc(sizeof(Supporter));
			
			// The supporter is initialised
			*deck->supporters[deck->supporter_count] = ALL_SUPPORTERS[index];
			
			// The number of energy cards is incremented
			deck->supporter_count++;
		}
		
		// Energy card
		else {
			// The energy array is extended
			deck->energy = realloc(
				deck->energy,
				(deck->energy_count + 1) * sizeof(Energy*)
			);
			
			// The new energy card has memory allocated to it
			deck->energy[deck->energy_count] = malloc(sizeof(Energy));
			
			// The energy is initialised
			*deck->energy[deck->energy_count] = ALL_ENERGY[index];
			
			// The number of energy cards is incremented
			deck->energy_count++;
		}
	}
}

/* Prints out a card storage's contents to standard output
 */
void print_store(CardStore card_store) {
	printf("\n");
	
	for (int i = 0; i < card_store.fighter_count; i++) {
		printf(
			"%s (%d)\n",
			card_store.fighters[i]->name,
			i
		);
	}
	
	for (int i = 0; i < card_store.supporter_count; i++) {
		printf(
			"%s (%d)\n",
			card_store.supporters[i]->name,
			i + card_store.fighter_count
		);
	}
	
	for (int i = 0; i < card_store.energy_count; i++) {
		printf(
			"%s (%d)\n",
			card_store.energy[i]->name,
			i + card_store.fighter_count + card_store.supporter_count
		);
	}
}

/* Returns a random number in the given range (inclusively)
 */
int rand_int(double min, double max) {
	return min + (1 + max - min) * rand() / (RAND_MAX + 1);
}

/* Moves a card from the source to the destination
 */
void rigged_draw(CardStore* source, CardStore* destination, int index) {
	// If a fighter is to be moved
	if (index < source->fighter_count) {
		// The destination's fighter array's size is incremented
		destination->fighters = realloc(
			destination->fighters,
			(destination->fighter_count + 1) * sizeof(Fighter*)
		);
		
		// The pointer to the fighter is copied
		destination->fighters[destination->fighter_count] =
			source->fighters[index]
		;
		
		// The destination's fighter count is incremented
		destination->fighter_count++;
		
		// The fighter is removed from the source
		for (int i = index; i < source->fighter_count - 1; i++) {
			source->fighters[i] = source->fighters[i + 1];
		}
		
		// The source's fighter array's size is decremented
		source->fighters = realloc(
			source->fighters,
			(source->fighter_count - 1) * sizeof(Fighter*)
		);
		
		// The source's fighter count is decremented
		source->fighter_count--;
	}
	
	// Else, the index disregards fighters
	else {
		index -= source->fighter_count;
		
		// If a supporter is to be moved
		if (index < source->supporter_count) {
			// The destination's supporter array's size is incremented
			destination->supporters = realloc(
				destination->supporters,
				(destination->supporter_count + 1) * sizeof(Supporter*)
			);
			
			// The pointer to the supporter is copied
			destination->supporters[destination->supporter_count] =
				source->supporters[index]
			;
			
			// The destination's supporter count is incremented
			destination->supporter_count++;
			
			// The supporter is removed from the source
			for (int i = index; i < source->supporter_count - 1; i++) {
				source->supporters[i] = source->supporters[i + 1];
			}
			
			// The source's supporter array's size is decremented
			source->supporters = realloc(
				source->supporters,
				(source->supporter_count - 1) * sizeof(Supporter*)
			);
			
			// The source's supporter count is decremented
			source->supporter_count--;
		}
		
		// Else, the index disregards supporters
		else {
			index -= source->supporter_count;
			
			// The destination's energy array's size is incremented
			destination->energy = realloc(
				destination->energy,
				(destination->energy_count + 1) * sizeof(Energy*)
			);
			
			// The pointer to the energy is copied
			destination->energy[destination->energy_count] =
				source->energy[index]
			;
			
			// The destination's energy count is incremented
			destination->energy_count++;
			
			// The energy is removed from the source
			for (int i = index; i < source->energy_count - 1; i++) {
				source->energy[i] = source->energy[i + 1];
			}
			
			// The source's energy array's size is decremented
			source->energy = realloc(
				source->energy,
				(source->energy_count - 1) * sizeof(Energy*)
			);
			
			// The source's energy count is decremented
			source->energy_count--;
		}
	}
}

/* Moves a random card from the source to the destination
 */
void random_draw(CardStore* source, CardStore* destination) {
	rigged_draw(source, destination, rand_int(0, card_count(*source) - 1));
}

/* Returns the pointer to the fighter removed from the card storage
 */
Fighter* get_fighter(CardStore* card_store, int index) {
	// The fighter is found
	Fighter* fighter = card_store->fighters[index];
	
	// The fighter is removed from the array
	for (int i = index; i < card_store->fighter_count - 1; i++) {
		card_store->fighters[i] = card_store->fighters[i + 1];
	}
	
	// The array's size is decremented
	card_store->fighters = realloc(
		card_store->fighters,
		(card_store->fighter_count - 1) * sizeof(Fighter*)
	);
	
	// The number of fighters is decremented
	card_store->fighter_count--;
	
	// The fighter is returned
	return fighter;
}

/* Stores the fighter card in the card storage
 */
void store_fighter(CardStore* card_store, Fighter* fighter) {
	// The array's size is incremented
	card_store->fighters = realloc(
		card_store->fighters,
		(card_store->fighter_count + 1) * sizeof(Fighter*)
	);
	
	// The fighter is stored in the array
	card_store->fighters[card_store->fighter_count] = fighter;
	
	// The number of fighter cards is incremented
	card_store->fighter_count++;
}

/* Returns the pointer to the supporter removed from the card storage
 */
Supporter* get_supporter(CardStore* card_store, int index) {
	// The supporter is found
	Supporter* supporter = card_store->supporters[index];
	
	// The supporter is removed from the array
	for (int i = index; i < card_store->supporter_count - 1; i++) {
		card_store->supporters[i] = card_store->supporters[i + 1];
	}
	
	// The array's size is decremented
	card_store->supporters = realloc(
		card_store->supporters,
		(card_store->supporter_count - 1) * sizeof(Supporter*)
	);
	
	// The number of supporters is decremented
	card_store->supporter_count--;
	
	// The supporter is returned
	return supporter;
}

/* Stores the supporter card in the card storage
 */
void store_supporter(CardStore* card_store, Supporter* supporter) {
	// The array's size is incremented
	card_store->supporters = realloc(
		card_store->supporters,
		(card_store->supporter_count + 1) * sizeof(Supporter*)
	);
	
	// The supporter is stored in the array
	card_store->supporters[card_store->supporter_count] = supporter;
	
	// The number of supporter cards is incremented
	card_store->supporter_count++;
}

/* Returns the pointer to the energy removed from the card storage
 */
Energy* get_energy(CardStore* card_store, int index) {
	// The energy is found
	Energy* energy = card_store->energy[index];
	
	// The energy is removed from the array
	for (int i = index; i < card_store->energy_count - 1; i++) {
		card_store->energy[i] = card_store->energy[i + 1];
	}
	
	// The array's size is decremented
	card_store->energy = realloc(
		card_store->energy,
		(card_store->energy_count - 1) * sizeof(Energy*)
	);
	
	// The number of energy cards is decremented
	card_store->energy_count--;
	
	// The energy is returned
	return energy;
}

/* Stores the energy card in the card storage
 */
void store_energy(CardStore* card_store, Energy* energy) {
	// The array's size is incremented
	card_store->energy = realloc(
		card_store->energy,
		(card_store->energy_count + 1) * sizeof(Energy*)
	);
	
	// The energy is stored in the array
	card_store->energy[card_store->energy_count] = energy;
	
	// The number of energy cards is incremented
	card_store->energy_count++;
}

/* Frees all of the memory that was dynamically allocated to the card store
 */
void free_store(CardStore* card_store) {
	// Frees the fighters
	while (card_store->fighter_count) {
		free(get_fighter(card_store, card_store->fighter_count - 1));
	}
	
	// Frees the supporters
	while (card_store->supporter_count) {
		free(get_supporter(card_store, card_store->supporter_count - 1));
	}
	
	// Frees the energy
	while (card_store->energy_count) {
		free(get_energy(card_store, card_store->energy_count - 1));
	}
}