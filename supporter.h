#include <stdio.h>

// The number of supporters currently in the game
const int SUPPORTER_COUNT = 14;

/* A structure to define supporter cards
 */
typedef struct Supporter {
	const char* name;        // The name of the supporter card
	const char* description; // The description of the supporter card
	const char* effects;     // The effects of the supporter card
} Supporter;

// All of the supporter cards in the game
const Supporter ALL_SUPPORTERS[] = {
	{
		.name = "Nurse",
		.description = "Heal 4 damage from 1 of your fighters.",
		.effects = "heal 4"
	},
	{
		.name = "Doctor",
		.description =
			"Choose one of your fighters. You may discard 1 energy "
			"card from it. If you do, heal 8 damage from it. If "
			"you don't, heal 2 damage from it."
		,
		.effects = "pay_heal 2 8"
	},
	{
		.name = "Hospital",
		.description =
			"Remove all effects and cards from all of your fighters. "
			"Restore all of them to full health."
		,
		.effects = "hospital"
	},
	{
		.name = "Professor",
		.description = "Discard your hand. Draw 7 cards.",
		.effects =
			"discard all\n"
			"draw 7"
	},
	{
		.name = "Researcher",
		.description = "Draw 2 cards.",
		.effects = "draw 2"
	},
	{
		.name = "Plumber",
		.description =
			"Discard an energy card from one of your opponent's fighters."
		,
		.effects = "drain 1"
	},
	{
		.name = "Powered-Up Plumber",
		.description =
			"Discard an energy card from one of your fighters. "
			"Discard 2 energy cards from one of your opponent's fighters."
		,
		.effects =
			"pay 1\n"
			"drain 2"
	},
	{
		.name = "Casino",
		.description =
			"Shuffle your hand into your deck. Flip a coin. "
			"If heads, draw 8 cards. If tails, draw 4 cards."
		,
		.effects =
			"shuffle all\n"
			"flip 1\n"
			"h draw 8\n"
			"t draw 4"
	},
	{
		.name = "Supercomputer",
		.description =
			"Discard 2 cards from your hand. Search your "
			"deck for a card and put it into your hand."
		,
		.effects =
			"discard 2\n"
			"search 1"
	},
	{
		.name = "Bounty Hunter",
		.description =
			"Switch in one of your opponent's benched fighters."
		,
		.effects = "gust"
	},
	{
		.name = "Recruiter",
		.description =
			"Discard 1 card from your hand. Search your deck "
			"for a fighter card and put it into your hand."
		,
		.effects =
			"discard 1\n"
			"search fighter 1"
	},
	{
		.name = "Personal Trainer",
		.description =
			"Discard 2 cards from your hand. Search your deck "
			"for a fighter card that ranks up from one of your "
			"fighters in play. Rank up that fighter (even if "
			"it was put into play this turn."
		,
		.effects =
			"discard 2\n"
			"search rank_up 1"
	},
	{
		.name = "Cheerleader",
		.description = "Your attacks deal 1 more damage this turn.",
		.effects = "power 1"
	},
	{
		.name = "Dumpster Diver",
		.description =
			"Search your trash for 2 energy cards "
			"and put them in your hand."
		,
		.effects = "search trash energy 2"
	}
};

/* Prints the supporter card to standard output
 */
void print_supporter(const Supporter* supporter) {
	if (supporter) {
		printf(
			"\nSupporter\n"
			"%s\n"
			"%s\n",
			supporter->name,
			supporter->description
		);
	}
}