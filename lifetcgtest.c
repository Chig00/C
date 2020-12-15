#include "lifetcg.h"

int main() {
	srand(time(NULL));
	
	Player player = {
		.active = NULL,
		.bench = {
			NULL,
			NULL,
			NULL,
			NULL,
			NULL
		},
		.effects = NULL,
		.effect_count = 0
	};
	
	Fighter fighters[6];
	
	for (int i = 0; i < 6; i++) {
		fighters[i] = ALL_FIGHTERS[i + 2];
	}
	
	player.active = fighters;
	
	for (int i = 0; i < BENCH_SIZE; i++) {
		player.bench[i] = fighters + i + 1;
	}
	
	affect_player(&player, "power 1");
	affect_player(&player, "power 2");
	affect_player(&player, "power 3");
	unaffect_player(&player, "power 1");
	
	printf(
		"%d\n",
		effect_count(player, "power")
	);
	
	return 0;
}