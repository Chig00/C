#include <stdio.h>
#include <string.h>

// The number of energy cards currently in the game
const int ENERGY_COUNT = 4;

/* A structure to define energy cards
 */
typedef struct Energy {
	const char* name;        // The name of the energy card
	const char* description; // The description of the energy card
	const char* element;     // The element of the energy card
	const char* effects;     // The effects of the energy card
	int value;               // The amount of energy this card provides
} Energy;

// All of the energy cards in the game
const Energy ALL_ENERGY[] = {
	{
		.name = "Water Energy",
		.description =
			"Provides 1 energy for the attacks of fighters of this element "
			"and 1 energy for the retreat of fighters of any element."
		,
		.element = "Water",
		.effects = "",
		.value = 1
	},
	{
		.name = "Earth Energy",
		.description =
			"Provides 1 energy for the attacks of fighters of this element "
			"and 1 energy for the retreat of fighters of any element."
		,
		.element = "Earth",
		.effects = "",
		.value = 1
	},
	{
		.name = "Fire Energy",
		.description =
			"Provides 1 energy for the attacks of fighters of this element "
			"and 1 energy for the retreat of fighters of any element."
		,
		.element = "Fire",
		.effects = "",
		.value = 1
	},
	{
		.name = "Air Energy",
		.description =
			"Provides 1 energy for the attacks of fighters of this element "
			"and 1 energy for the retreat of fighters of any element."
		,
		.element = "Air",
		.effects = "",
		.value = 1
	}
};

/* Prints the energy to standard output
 */
void print_energy(const Energy* energy) {
	if (energy) {
		printf(
			"\nEnergy (%s)\n"
			"%s (%s)\n"
			"%s\n",
			energy->element,
			energy->name,
			strlen(energy->effects) ? "Special" : "Basic",
			energy->description
		);
	}
}