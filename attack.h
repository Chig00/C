#include <stdio.h>

// The number of attacks currently in the game
const int ATTACK_COUNT = 29;

/* A structure that defines a fighter's attack
 */
typedef struct Attack {
	const char* name;        // The name of the attack
	const char* description; // The description of the attack
	const char* effects;     // The effects of the attack
	int damage;              // The damage dealt to the defending fighter
	int cost;                // The energy cost of the attack
} Attack;

// All of the attacks in the game
const Attack ALL_ATTACKS[] = {
	{
		.name = "Accelerate",
		.description =
			"Deal 1 damage to the defending fighter. "
			"Flip a coin. If heads, this fighter cannot "
			"be damaged during your opponent's next turn."
		,
		.effects =
			"flip 1\n"
			"h invincible"
		,
		.damage = 1,
		.cost = 1
	},
	{
		.name = "Drift",
		.description = "Deal 3 damage to the defending fighter.",
		.effects = "",
		.damage = 3,
		.cost = 2
	},
	{
		.name = "Nitro",
		.description =
			"Deal 2 damage to the defending fighter. "
			"Flip a coin. If heads, this fighter cannot "
			"be damaged during your opponent's next turn."
		,
		.effects =
			"flip 1\n"
			"h invincible"
		,
		.damage = 2,
		.cost = 1
	},
	{
		.name = "Hyperthrust",
		.description =
			"Deal 5 damage to the defending fighter. "
			"Discard one energy card from this fighter."
		,
		.effects = "pay 1",
		.damage = 5,
		.cost = 2
	},
	{
		.name = "Super Nitro",
		.description =
			"Deal 4 damage to the defending fighter. "
			"Flip a coin. If heads, this fighter cannot "
			"be damaged during your opponent's next turn. "
			"If tails, move an energy card from this "
			"fighter to a fighter on your bench. If your "
			"bench is empty, discard that energy card."
		,
		.effects =
			"flip 1\n"
			"h invincible\n"
			"t backpedal 1"
		,
		.damage = 4,
		.cost = 2
	},
	{
		.name = "Sea Fist",
		.description =
			"Deal 2 damage to the defending fighter. "
			"Flip a coin. If heads, deal 1 more damage."
		,
		.effects =
			"flip 1\n"
			"h power 1"
		,
		.damage = 2,
		.cost = 1
	},
	{
		.name = "Torpedo",
		.description =
			"Deal 3 damage to one of your opponent's fighters."
		,
		.effects = "snipe 3",
		.damage = 0,
		.cost = 2
	},
	{
		.name = "Plank Walk",
		.description =
			"Deal 3 damage to the defending fighter. "
			"If it isn't of the water element, deal 2 more "
			"damage to it.",
		.effects =
			"plankwalk 2"
		,
		.damage = 3,
		.cost = 1
	},
	{
		.name = "Fancy Flying",
		.description =
			"Deal 1 damage to the defending fighter. "
			"Flip a coin. If heads, this fighter cannot "
			"be damaged during your opponent's next turn."
		,
		.effects =
			"flip 1\n"
			"h invincible"
		,
		.damage = 1,
		.cost = 0
	},
	{
		.name = "Divebomb",
		.description =
			"Deal 5 damage to the defending fighter. "
			"Deal 2 damage to this fighter."
		,
		.effects = "recoil 2",
		.damage = 5,
		.cost = 1
	},
	{
		.name = "G Flip",
		.description =
			"Deal 3 damage to the defending fighter. "
			"Double its retreat cost."
		,
		.effects = "gravity",
		.damage = 3,
		.cost = 0
	},
	{
		.name = "Pickaxe Slap",
		.description = "Deal 1 damage to the defending fighter.",
		.effects = "",
		.damage = 1,
		.cost = 1
	},
	{
		.name = "Undermine",
		.description =
			"Deal 2 damage to the defending fighter. "
			"Discard an energy card from the "
			"defending fighter."
		,
		.effects = "drain 1",
		.damage = 2,
		.cost = 1
	},
	{
		.name = "Aqua Rush",
		.description = "Deal 1 damage to the defending fighter.",
		.effects = "",
		.damage = 1,
		.cost = 1
	},
	{
		.name = "Dive",
		.description =
			"Deal 2 damage to the defending fighter. "
			"Flip a coin. If heads, prevents all effects "
			"of cards on this fighter during your "
			"opponent's next turn."
		,
		.effects =
			"flip 1\n"
			"h immune"
		,
		.damage = 2,
		.cost = 1
	},
	{
		.name = "Surf",
		.description =
			"Deal 1 damage to all of your opponent's "
			"fighters. Switch this fighter out."
		,
		.effects =
			"splash 1\n"
			"switch_out"
		,
		.damage = 1,
		.cost = 0
	},
	{
		.name = "Magic Missiles",
		.description =
			"Deal 2 damage to the defending fighter. "
			"Deal 1 damage to a fighter on your "
			"opponent's bench."
		,
		.effects = "snipe bench 1",
		.damage = 2,
		.cost = 1
	},
	{
		.name = "Greater Magic Missiles",
		.description =
			"Deal 2 damage to the defending fighter. "
			"Deal 1 damage to 2 different fighters "
			"on your opponent's bench."
		,
		.effects = "snipe bench double 1",
		.damage = 2,
		.cost = 1
	},
	{
		.name = "Incinerate",
		.description =
			"Deal 3 damage to all of your opponent's "
			"fighters. Deal 1 damage to all of your "
			"benched fighters."
		,
		.effects =
			"splash 3\n"
			"splash self 1"
		,
		.damage = 3,
		.cost = 2
	},
	{
		.name = "Shadow Pulse",
		.description =
			"Deal 4 damage to the defending fighter. "
			"Send one of your opponent's prizes to the void. "
			"If the defending fighter is defeated, send it "
			"and all attached cards to the void."
		,
		.effects =
			"deprize void 1\n"
			"redirect void"
		,
		.damage = 4,
		.cost = 0
	},
	{
		.name = "Lost Communication",
		.description =
			"Choose an attack of a fighter in the void. "
			"Use it as this attack."
		,
		.effects = "talk void",
		.damage = 0,
		.cost = 1
	},
	{
		.name = "Heal Pulse",
		.description =
			"Deal 2 damage to the defending fighter. "
			"Heal 1 damage from this fighter."
		,
		.effects = "recover 1",
		.damage = 2,
		.cost = 2
	},
	{
		.name = "Haymaker",
		.description = "Deal 2 damage to the defending fighter.",
		.effects = "",
		.damage = 2,
		.cost = 1
	},
	{
		.name = "Ancient Power",
		.description =
			"Choose an attack of a fighter in your trash. "
			"Use it as this attack."
		,
		.effects = "talk trash",
		.damage = 0,
		.cost = 1
	},
	{
		.name = "Body Slam",
		.description = "Deal 3 damage to the defending fighter.",
		.effects = "",
		.damage = 3,
		.cost = 3
	},
	{
		.name = "Wander",
		.description = "Draw a card.",
		.effects = "draw 1",
		.damage = 0,
		.cost = 0
	},
	{
		.name = "Dark Flame",
		.description =
			"Deal 3 damage to the defending fighter. "
			"Discard all cards attached to this fighter."
		,
		.effects = "pay all",
		.damage = 3,
		.cost = 1
	},
	{
		.name = "Lost Wail",
		.description =
			"Deal 1 damage to the defending fighter for "
			"each fighter in the void."
		,
		.effects = "lost_wail 1",
		.damage = 0,
		.cost = 1,
	},
	{
		.name = "Void Pact",
		.description =
			"Banish both active fighters and all attached "
			"cards. Neither player draws a prize."
		,
		.effects = "void_pact",
		.damage = 0,
		.cost = 3
	}
};

/* Prints the attack to standard output
 */
void print_attack(const Attack* attack) {
	if (attack) {
		printf(
			"%s (%d)\n"
			"%s\n",
			attack->name,
			attack->cost,
			attack->description
		);
	}
}