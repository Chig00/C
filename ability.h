#include <stdio.h>
#include <stdbool.h>

// The number of abilities currently in the game
const int ABILITY_COUNT = 8;

/* A structure that defines a fighter's ability
 */
typedef struct Ability {
	const char* name;        // The ability's name
	const char* description; // The ability's description
	const char* effects;     // The ability's effects
	bool passive;            // True if the ability is passive
	int uses;                // The number of ability uses per turn (-1 if infinite)
} Ability;

// All of the abilities in the game
const Ability ALL_ABILITIES[] = {
	{
		.name = "Charged Thruster",
		.description =
			"If this fighter is on the bench, you may switch it "
			"with your active fighter. If you do, this fighter "
			"deals 2 more damage this turn."
		,
		.effects =
			"switch_in\n"
			"power 2"
		,
		.passive = false,
		.uses = -1
	},
	{
		.name = "Plundered",
		.description =
			"Once a turn, you may draw an extra card or make "
			"your opponent draw 3 cards."
		,
		.effects = "draw either 1 3",
		.passive = false,
		.uses = 1
	},
	{
		.name = "Zero Gravity",
		.description = "Your fighters can retreat for free.",
		.effects = "retreat_aid",
		.passive = true,
		.uses = -1
	},
	{
		.name = "Healing Aura",
		.description =
			"At the end of your turn, heal 1 damage from each "
			"of your fighters."
		,
		.effects = "end_heal 1",
		.passive = true,
		.uses = -1,
	},
	{
		.name = "Aggressive",
		.description =
			"If your active fighter doesn't have an ability "
			"with the same effect, switch this fighter with "
			"your active one."
		,
		.effects = "aggro",
		.passive = true,
		.uses = 1
	},
	{
		.name = "Sacrifice",
		.description =
			"You may increase this fighter's health by the "
			"health of one of your fighters. If you do, "
			"defeat that fighter."
		,
		.effects = "sacrifice",
		.passive = false,
		.uses = -1
	},
	{
		.name = "Lost",
		.description =
			"If this fighter is defeated, your opponent "
			"doesn't get a prize. Banish this fighter and "
			"all of its attached cards."
		,
		.effects = "lost",
		.passive = true,
		.uses = -1
	},
	{
		.name = "Banish",
		.description =
			"Whenever you discard one of your cards, "
			"banish it instead."
		,
		.effects = "banish",
		.passive = true,
		.uses = -1
	}
};

/* Prints the ability to standard ouput
 */
void print_ability(const Ability* ability) {
	if (ability) {
		printf(
			"Ability: %s\n"
			"%s\n",
			ability->name,
			ability->description
		);
	}
}