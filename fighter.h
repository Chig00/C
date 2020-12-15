#include "ability.h"
#include "attack.h"
#include "energy.h"

// The number of fighters currently in the game
const int FIGHTER_COUNT = 22;

/* A structure that defines a fighter card
 */
typedef struct Fighter {
	const char* name;         // The fighter's name
	const char* element;      // The fighter's element
	int health[2];            // The fighter's health and maximum health
	int retreat;              // The energy cost of a retreat
	int rank;                 // The numerical rank of the fighter
	const char* old_rank;     // The previous named rank of the fighter
	const Attack* attacks[2]; // The fighter's attacks (NULL if no attack)
	const Ability* ability;   // The fighter's ability (NULL if no ability)
	int uses;                 // The number of ability uses this turn
	Energy** energy;          // The fighter's energy
	int energy_count;         // The number of attached energy cards
	char** effects;           // The effects on the fighter
	int effect_count;         // The number of effects on the fighter
	bool first_turn;          // True for the first turn that the fighter is played on
} Fighter;

// All of the fighter cards in the game
const Fighter ALL_FIGHTERS[] = {
	{
		.name = "Driver",
		.element = "Fire",
		.health = {4, 4},
		.retreat = 1,
		.rank = 0,
		.old_rank = "",
		.attacks = {
			ALL_ATTACKS + 0,
			ALL_ATTACKS + 1
		},
		.ability = NULL,
		.uses = 0,
		.energy = NULL,
		.energy_count = 0,
		.effects = NULL,
		.effect_count = 0,
		.first_turn = true
	},
	{
		.name = "Racer",
		.element = "Fire",
		.health = {7, 7},
		.retreat = 1,
		.rank = 1,
		.old_rank = "Driver",
		.attacks = {
			ALL_ATTACKS + 2,
			ALL_ATTACKS + 3
		},
		.ability = NULL,
		.uses = 0,
		.energy = NULL,
		.energy_count = 0,
		.effects = NULL,
		.effect_count = 0,
		.first_turn = true
	},
	{
		.name = "Hotrod",
		.element = "Fire",
		.health = {10, 10},
		.retreat = 1,
		.rank = 2,
		.old_rank = "Racer",
		.attacks = {
			ALL_ATTACKS + 4,
			NULL
		},
		.ability = ALL_ABILITIES + 0,
		.uses = 0,
		.energy = NULL,
		.energy_count = 0,
		.effects = NULL,
		.effect_count = 0,
		.first_turn = true
	},
	{
		.name = "Sailor",
		.element = "Water",
		.health = {8, 8},
		.retreat = 1,
		.rank = 1,
		.old_rank = "Driver",
		.attacks = {
			ALL_ATTACKS + 5,
			ALL_ATTACKS + 6
		},
		.ability = NULL,
		.uses = 0,
		.energy = NULL,
		.energy_count = 0,
		.effects = NULL,
		.effect_count = 0,
		.first_turn = true
	},
	{
		.name = "Pirate",
		.element = "Water",
		.health = {11, 11},
		.retreat = 2,
		.rank = 2,
		.old_rank = "Sailor",
		.attacks = {
			ALL_ATTACKS + 7,
			NULL
		},
		.ability = ALL_ABILITIES + 1,
		.uses = 0,
		.energy = NULL,
		.energy_count = 0,
		.effects = NULL,
		.effect_count = 0,
		.first_turn = true
	},
	{
		.name = "Pilot",
		.element = "Air",
		.health = {7, 7},
		.retreat = 0,
		.rank = 1,
		.old_rank = "Driver",
		.attacks = {
			ALL_ATTACKS + 8,
			ALL_ATTACKS + 9
		},
		.ability = NULL,
		.uses = 0,
		.energy = NULL,
		.energy_count = 0,
		.effects = NULL,
		.effect_count = 0,
		.first_turn = true
	},
	{
		.name = "Astronaut",
		.element = "Air",
		.health = {9, 9},
		.retreat = 3,
		.rank = 2,
		.old_rank = "Pilot",
		.attacks = {
			ALL_ATTACKS + 10,
			NULL
		},
		.ability = ALL_ABILITIES + 2,
		.uses = 0,
		.energy = NULL,
		.energy_count = 0,
		.effects = NULL,
		.effect_count = 0,
		.first_turn = true
	},
	{
		.name = "Miner",
		.element = "Earth",
		.health = {5, 5},
		.retreat = 1,
		.rank = 0,
		.old_rank = "",
		.attacks =  {
			ALL_ATTACKS + 11,
			NULL
		},
		.ability = NULL,
		.uses = 0,
		.energy = NULL,
		.energy_count = 0,
		.effects = NULL,
		.effect_count = 0,
		.first_turn = true
	},
	{
		.name = "Excavator",
		.element = "Earth",
		.health = {10, 10},
		.retreat = 2,
		.rank = 1,
		.old_rank = "Miner",
		.attacks =  {
			ALL_ATTACKS + 12,
			NULL
		},
		.ability = NULL,
		.uses = 0,
		.energy = NULL,
		.energy_count = 0,
		.effects = NULL,
		.effect_count = 0,
		.first_turn = true
	},
	{
		.name = "Swimmer",
		.element = "Water",
		.health = {5, 5},
		.retreat = 1,
		.rank = 0,
		.old_rank = "",
		.attacks =  {
			ALL_ATTACKS + 13,
			NULL
		},
		.ability = NULL,
		.uses = 0,
		.energy = NULL,
		.energy_count = 0,
		.effects = NULL,
		.effect_count = 0,
		.first_turn = true
	},
	{
		.name = "Diver",
		.element = "Water",
		.health = {10, 10},
		.retreat = 1,
		.rank = 1,
		.old_rank = "Swimmer",
		.attacks =  {
			ALL_ATTACKS + 14,
			NULL
		},
		.ability = NULL,
		.uses = 0,
		.energy = NULL,
		.energy_count = 0,
		.effects = NULL,
		.effect_count = 0,
		.first_turn = true
	},
	{
		.name = "Surfer",
		.element = "Water",
		.health = {6, 6},
		.retreat = 1,
		.rank = 1,
		.old_rank = "Swimmer",
		.attacks =  {
			ALL_ATTACKS + 15,
			NULL
		},
		.ability = NULL,
		.uses = 0,
		.energy = NULL,
		.energy_count = 0,
		.effects = NULL,
		.effect_count = 0,
		.first_turn = true
	},
	{
		.name = "Mage",
		.element = "Air",
		.health = {3, 3},
		.retreat = 1,
		.rank = 0,
		.old_rank = "",
		.attacks =  {
			ALL_ATTACKS + 16,
			NULL
		},
		.ability = NULL,
		.uses = 0,
		.energy = NULL,
		.energy_count = 0,
		.effects = NULL,
		.effect_count = 0,
		.first_turn = true
	},
	{
		.name = "Wizard",
		.element = "Fire",
		.health = {6, 6},
		.retreat = 2,
		.rank = 1,
		.old_rank = "Mage",
		.attacks =  {
			ALL_ATTACKS + 17,
			ALL_ATTACKS + 18
		},
		.ability = NULL,
		.uses = 0,
		.energy = NULL,
		.energy_count = 0,
		.effects = NULL,
		.effect_count = 0,
		.first_turn = true
	},
	{
		.name = "Warlock",
		.element = "Earth",
		.health = {8, 8},
		.retreat = 2,
		.rank = 1,
		.old_rank = "Mage",
		.attacks =  {
			ALL_ATTACKS + 19,
			ALL_ATTACKS + 20
		},
		.ability = NULL,
		.uses = 0,
		.energy = NULL,
		.energy_count = 0,
		.effects = NULL,
		.effect_count = 0,
		.first_turn = true
	},
	{
		.name = "Cleric",
		.element = "Water",
		.health = {6, 6},
		.retreat = 2,
		.rank = 1,
		.old_rank = "Mage",
		.attacks =  {
			ALL_ATTACKS + 21,
			NULL
		},
		.ability = ALL_ABILITIES + 3,
		.uses = 0,
		.energy = NULL,
		.energy_count = 0,
		.effects = NULL,
		.effect_count = 0,
		.first_turn = true
	},
	{
		.name = "Boxer",
		.element = "Earth",
		.health = {7, 7},
		.retreat = 2,
		.rank = 0,
		.old_rank = "",
		.attacks =  {
			ALL_ATTACKS + 22,
			NULL
		},
		.ability = ALL_ABILITIES + 4,
		.uses = 0,
		.energy = NULL,
		.energy_count = 0,
		.effects = NULL,
		.effect_count = 0,
		.first_turn = true
	},
	{
		.name = "Cultist",
		.element = "Earth",
		.health = {3, 3},
		.retreat = 1,
		.rank = 0,
		.old_rank = "",
		.attacks =  {
			ALL_ATTACKS + 23,
			NULL
		},
		.ability = ALL_ABILITIES + 5,
		.uses = 0,
		.energy = NULL,
		.energy_count = 0,
		.effects = NULL,
		.effect_count = 0,
		.first_turn = true
	},
	{
		.name = "Glutton",
		.element = "Earth",
		.health = {9, 9},
		.retreat = 4,
		.rank = 0,
		.old_rank = "",
		.attacks =  {
			ALL_ATTACKS + 24,
			NULL
		},
		.ability = NULL,
		.uses = 0,
		.energy = NULL,
		.energy_count = 0,
		.effects = NULL,
		.effect_count = 0,
		.first_turn = true
	},
	{
		.name = "Lost Soul",
		.element = "Air",
		.health = {1, 1},
		.retreat = 0,
		.rank = 0,
		.old_rank = "",
		.attacks =  {
			ALL_ATTACKS + 25,
			NULL
		},
		.ability = ALL_ABILITIES + 6,
		.uses = 0,
		.energy = NULL,
		.energy_count = 0,
		.effects = NULL,
		.effect_count = 0,
		.first_turn = true
	},
	{
		.name = "Banisher",
		.element = "Fire",
		.health = {8, 8},
		.retreat = 2,
		.rank = 0,
		.old_rank = "",
		.attacks =  {
			ALL_ATTACKS + 26,
			NULL
		},
		.ability = ALL_ABILITIES + 7,
		.uses = 0,
		.energy = NULL,
		.energy_count = 0,
		.effects = NULL,
		.effect_count = 0,
		.first_turn = true
	},
	{
		.name = "Banshee",
		.element = "Water",
		.health = {6, 6},
		.retreat = 1,
		.rank = 0,
		.old_rank = "",
		.attacks =  {
			ALL_ATTACKS + 27,
			ALL_ATTACKS + 28
		},
		.ability = NULL,
		.uses = 0,
		.energy = NULL,
		.energy_count = 0,
		.effects = NULL,
		.effect_count = 0,
		.first_turn = true
	}
};

/* Prints the fighter card to standard output
 */
void print_fighter(const Fighter* fighter) {
	if (fighter) {
		printf(
			"\nFighter (%s)\n"
			"Rank %d",
			fighter->element,
			fighter->rank
		);
		
		if (fighter->rank) {
			printf(
				" from %s",
				fighter->old_rank
			);
		}
		
		printf(
			"\n%s %d/%d\n",
			fighter->name,
			fighter->health[0],
			fighter->health[1]
		);
		
		if (fighter->ability) {
			print_ability(fighter->ability);
		}
		
		if (fighter->attacks[0]) {
			print_attack(fighter->attacks[0]);
		}
		
		if (fighter->attacks[1]) {
			print_attack(fighter->attacks[1]);
		}
		
		printf(
			"Retreat: %d\n",
			fighter->retreat
		);
	}
}

/* Returns the value of energy attached to the fighter
     that matches the fighter's element
 */
int energy_count(const Fighter* fighter) {
	int value = 0;
	
	for (int i = 0; i < fighter->energy_count; i++) {
		if (!strcmp(fighter->energy[i]->element, fighter->element)) {
			value += fighter->energy[i]->value;
		}
	}
	
	return value;
}

/* Returns the value of energy attached to the fighter
 */
int energy_any_count(const Fighter* fighter) {
	int value = 0;
	
	for (int i = 0; i < fighter->energy_count; i++) {
		value += fighter->energy[i]->value;
	}
	
	return value;
}

/* Attaches an energy card to the fighter
 */
void attach(Fighter* fighter, Energy* energy) {
	// The array of energy cards is extended
	fighter->energy = realloc(
		fighter->energy,
		(fighter->energy_count + 1) * sizeof(Energy*)
	);
	
	// The energy is stored
	fighter->energy[fighter->energy_count] = energy;
	
	// The energy count is incremented
	fighter->energy_count++;
}

/* Returns and removes the energy at the given index
 */
Energy* pay(Fighter* fighter, int index) {
	// The energy is found
	Energy* energy = fighter->energy[index];
	
	// The energy is removed from the array
	for (int i = index; i < fighter->energy_count - 1; i++) {
		fighter->energy[i] = fighter->energy[i + 1];
	}
	
	// The array is shortened
	fighter->energy = realloc(
		fighter->energy,
		(fighter->energy_count - 1) * sizeof(Energy*)
	);
	
	// The energy count is decremented
	fighter->energy_count--;
	
	// The energy is returned
	return energy;
}

/* Prints all of the energy attached to the fighter
 */
void display_energy(const Fighter* fighter) {
	for (int i = 0; i < fighter->energy_count; i++) {
		printf("\n%d:", i);
		print_energy(fighter->energy[i]);
	}
}

/* Converts all energy that matches the fighter's current
     element to the given element
 */
void convert_energy(Fighter* fighter, const char* element) {
	for (int i = 0; i < fighter->energy_count; i++) {
		if (!strcmp(fighter->energy[i]->element, fighter->element)) {
			fighter->energy[i]->element = element;
		}
	}
}

/* Adds an effect to the fighter
   The effect is a dynamically allocated copy of the given argument
 */
void affect_fighter(Fighter* fighter, const char* effect) {
	int length = strlen(effect) + 1;
	
	// The array of effects is extended
	fighter->effects = realloc(
		fighter->effects,
		(fighter->effect_count + 1) * sizeof(const char*)
	);
	
	// The effect is appended to the array
	fighter->effects[fighter->effect_count] = malloc(
		length * sizeof(char)
	);
	
	for (int i = 0; i < length; i++) {
		fighter->effects[fighter->effect_count][i] = effect[i];
	}
		
	// The effect count is incremented
	fighter->effect_count++;
}

/* Removes an effect from the fighter
   All effects that match the effect given are removed
   A matching effect will have the effect given at the front of the string
 */
void unaffect_fighter(Fighter* fighter, const char* effect) {
	// All effects are checked
	for (int i = 0; i < fighter->effect_count; i++) {
		int length = strlen(effect);
		
		// If the effect in question is shorter, it cannot match
		if (strlen(fighter->effects[i]) >= length) {
			bool match = true;
			
			// Check for match
			for (int j = 0; j < length; j++) {
				if (fighter->effects[i][j] != effect[j]) {
					match = false;
					break;
				}
			}
			
			// If it matches, the effect is removed
			// The currently checked index is decremented, as the next
			//   effect's index was also decremented
			if (match) {
				// The effect is removed
				free(fighter->effects[i]);
				
				for (int j = i; j < fighter->effect_count - 1; j++) {
					fighter->effects[j] = fighter->effects[j + 1];
				}
				
				// The array's size and count is decremented
				fighter->effects = realloc(
					fighter->effects,
					(--fighter->effect_count) * sizeof(const char*)
				);
				
				i--;
			}
		}
	}
}

/* Resets a fighter to their initial state
   Does not reset energy as energy should be dealt with elsewhere
 */
void reset_fighter(Fighter* fighter) {
	fighter->health[0] = fighter->health[1];
	unaffect_fighter(fighter, "");
	fighter->first_turn = true;
}

/* Ranks the fighter up and returns the base rank
 */
Fighter* rank_up(Fighter** old_form, Fighter* new_form) {
	// The new rank inherits the old rank's energy
	// The energy that matched the old rank's element
	//   is converted to the new rank's element
	convert_energy(*old_form, new_form->element);
	
	while ((*old_form)->energy_count) {
		attach(new_form, pay(*old_form, (*old_form)->energy_count - 1));
	}
	
	// The new rank inherits the old rank's damage
	new_form->health[0] =
		new_form->health[1]
		- ((*old_form)->health[1] - (*old_form)->health[0])
	;
	
	// The old rank is reset
	reset_fighter(*old_form);
	
	// The old rank is stored and replaced
	Fighter* old_rank = *old_form;
	*old_form = new_form;
	
	// The old rank is returned
	return old_rank;
}

/* Prints out all of the effects on a fighter
 */
void display_effects_f(const Fighter* fighter) {
	printf("\n");
	
	for (int i = 0; i < fighter->effect_count; i++) {
		printf("%s\n", fighter->effects[i]);
	}
}

/* Returns true if the fighter or the fighter's abilities has the effect.
   Uses the same matching rules as unaffect_fighter().
 */
bool effect_check(const Fighter* fighter, const char* effect) {
	int length = strlen(effect);
	
	// Empty string should always return true
	if (!strcmp(effect, "")) {
		return true;
	}
	
	// All effects on the player's active fighter are checked
	for (int i = 0; i < fighter->effect_count; i++) {
		// If the effect in question is shorter, it cannot match
		if (strlen(fighter->effects[i]) >= length) {
			bool match = true;
			
			// Check for match
			for (int j = 0; j < length; j++) {
				if (fighter->effects[i][j] != effect[j]) {
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
		fighter->ability
		&& fighter->ability->passive
		&& fighter->uses != fighter->ability->uses
	) {
		int length2 = strlen(fighter->ability->effects);
		
		// A match cannot occur if the effects of the ability are
		//   shorter than the checked effect
		if (length2 >= length) {
			// A match is assumed until proven otherwise
			bool match = true;
			
			for (int i = 0, index = 0; i < length2; i++, index++) {
				// New effect to check
				if (fighter->ability->effects[i] == '\n') {
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
					&& fighter->ability->effects[i] != effect[index]
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
	
	return false;
}

/* Calculates the total effect value of an effect type on a fighter.
   Checks both the effect array and the ability's effects.
   Only effects of the form [effect_type] [effect_value] are considered.
 */
int effect_count_f(const Fighter* fighter, const char* effect) {
	int count = 0;
	
	for (int i = 0; i < fighter->effect_count; i++) {
		char type[20];
		sscanf(fighter->effects[i], "%s", type);
		
		// The types match
		if (!strcmp(type, effect)) {
			int value;
			sscanf(fighter->effects[i], "%*s%d", &value);
			count += value;
		}
	}
	
	if (
		fighter->ability
		&& fighter->ability->passive
		&& fighter->uses != fighter->ability->uses
	) {
		bool end = false;
		int index = 0;
		
		while (!end) {
			char buffer[50];
			
			for (int i = 0;; i++, index++) {
				buffer[i] = fighter->ability->effects[index];
				
				if (buffer[i] == '\n') {
					index++;
					break;
				}
				
				if (buffer[i] == '\0') {
					end = true;
					break;
				}
			}
			
			char word[20];
			sscanf(buffer, "%s", word);
			
			if (!strcmp(word, effect)) {
				int value;
				sscanf(buffer, "%*s%d", &value);
				count += value;
			}
		}
	}
	
	return count;
}