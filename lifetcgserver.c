#include "server.h"
#include "lifetcg.h"

// Macros that allow for easy version management
#define Messenger Server
#define messenger_send server_send
#define messenger_read server_read
#define messenger server
#define flip coin_flip()

const char* const VERSION = "0.1.0.0";

/* Performs a fighter's attack
 */
void attack(
	Player* attacker, Player* defender, int index,
	CardStore* the_void, Messenger* messenger
) {
	// The attack is set (this allows for the attack to change
	const Attack* attack = attacker->active->attacks[index];
	int power = effect_count(*attacker, "power");
	
	// Attack announcement
	printf(
		"\n%s used %s.\n",
		attacker->active->name,
		attack->name
	);
	
	// The boost in damage from the attack's effect
	int boost = 0;
	
	// Attack effects are managedc
	bool end = false;
	int effect_index = 0;
	char buffer[50];
	char flips[10];
	CardStore* stores[2] = {
		effect_search(*attacker, "banish")
		? the_void
		: &attacker->trash,
		effect_search(*defender, "banish")
		? the_void
		: &defender->trash
	};
	
	while (!end) {
		for (int i = 0;; i++, effect_index++) {
			buffer[i] = attack->effects[effect_index];
			
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
				if (!card_count(attacker->deck)) {
					draw = i;
					break;
				}
				
				int counts[3] = {
					attacker->hand.fighter_count,
					attacker->hand.supporter_count,
					attacker->hand.energy_count
				};
				
				random_draw(&attacker->deck, &attacker->hand);
				
				// The card drawn is displayed
				if (attacker < defender) {
					if (counts[0] < attacker->hand.fighter_count) {
						printf(
							"You drew %s.\n",
							attacker->hand.fighters[counts[0]]->name
						);
					}
					
					else if (counts[1] < attacker->hand.supporter_count) {
						printf(
							"You drew %s.\n",
							attacker->hand.supporters[counts[1]]->name
						);
					}
					
					else {
						printf(
							"You drew %s.\n",
							attacker->hand.energy[counts[2]]->name
						);
					}
				}
			}
			
			// The number of cards drawn is displayed
			if (defender < attacker) {
				printf("Your opponent drew %d cards.\n", draw);
			}
		}
		
		// Flips coins
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
			
			// Invincibility prevents damage
			if (!strcmp(word, "invincible")) {
				affect_fighter(attacker->active, "invincible");
				
				printf(
					"%s is invincible during your%s next turn.\n",
					attacker->active->name,
					attacker < defender ? " opponent's" : ""
				);
			}
			
			// Immunity prevents effects (including damage)
			else if (!strcmp(word, "immune")) {
				affect_fighter(attacker->active, "immune");
				
				printf(
					"%s is immune to all effects during your%s next turn.\n",
					attacker->active->name,
					attacker < defender ? " opponent's" : ""
				);
			}

			// Increases damage
			else if (!strcmp(word, "power")) {
				int value;
				sscanf(buffer, "%*s%*s%d", &value);
				boost += value;
			}
		}
		
		// Tails
		else if (!strcmp(word, "t") && !strncmp(flips, "t", 1)) {
			sscanf(buffer, "%*s%s", word);
			
			// Moves an energy to the bench (no benched fighters discarded the energy)
			if (!strcmp(word, "backpedal")) {
				int count;
				sscanf(buffer, "%*s%*s%d", &count);
				
				bool bench_empty = true;
				
				for (int i = 0; i < BENCH_SIZE; i++) {
					if (attacker->bench[i]) {
						bench_empty = false;
						break;
					}
				}
				
				// Energy is discarded if the bench is empty
				if (bench_empty) {
					// Energy is discarded
					for (int i = 0; i < count && attacker->active->energy_count; i++) {
						Energy* energy;
						
						if (attacker < defender) {
							while (true) {
								printf("\nEnergy to discard from the active fighter:");
								display_energy(attacker->active);
								printf("\nIndex: ");
								fgets(buffer, 50, stdin);
								int index;
								sscanf(buffer, "%d", &index);
								
								if (0 <= index && index < attacker->active->energy_count) {
									print_energy(attacker->active->energy[index]);
									printf("\nDiscard this energy card (y/n)? ");
									fgets(buffer, 50, stdin);
									char answer;
									sscanf(buffer, "%c", &answer);
									
									if (answer == 'y') {
										energy = pay(attacker->active, index);
										sprintf(buffer, "%d", index);
										messenger_send(messenger, buffer);
										break;
									}
								}
								
								else {
									printf("\nInvalid index.\n");
								}
							}
							
							printf("\n");
						}
						
						else {
							messenger_read(messenger, buffer, 50);
							int index;
							sscanf(buffer, "%d", &index);
							energy = pay(attacker->active, index);
						}
						
						store_energy(stores[0], energy);
						printf(
							"%s was discarded.\n",
							energy->name
						);
					}
				}
				
				// Energy is moved if the bench isn't empty
				else{
					Fighter* destination;
				
					if (attacker < defender) {
						// Loop to get input
						while (true) {
							printf("\nEnergy recipient:\n");
							
							for (int i = 0; i < BENCH_SIZE; i++) {
								if (attacker->bench[i]) {
									printf(
										"%s (%d)\n",
										attacker->bench[i]->name,
										i
									);
								}
							}
							
							printf("\nIndex: ");
							fgets(buffer, 50, stdin);
							int index;
							sscanf(buffer, "%d", &index);
							
							if (0 <= index && index < BENCH_SIZE && attacker->bench[index]) {
								print_fighter(attacker->bench[index]);
								display_energy(attacker->bench[index]);
								printf("\nChoose this fighter (y/n)? ");
								fgets(buffer, 50, stdin);
								char answer;
								sscanf(buffer, "%c", &answer);
								
								if (answer == 'y') {
									sprintf(buffer, "%d", index);
									messenger_send(messenger, buffer);
									destination = attacker->bench[index];
									break;
								}
							}
							
							else {
								printf("\nInvalid index.\n");
							}
						}
					}
					
					else {
						messenger_read(messenger, buffer, 50);
						int index;
						sscanf(buffer, "%d", &index);
						destination = attacker->bench[index];
					}
					
					// Energy is moved to the benched fighter
					for (int i = 0; i < count && attacker->active->energy_count; i++) {
						Energy* energy;
						
						if (attacker < defender) {
							while (true) {
								printf("\nEnergy to move:");
								display_energy(attacker->active);
								printf("\nIndex: ");
								fgets(buffer, 50, stdin);
								int index;
								sscanf(buffer, "%d", &index);
								
								if (0 <= index && index < attacker->active->energy_count) {
									print_energy(attacker->active->energy[index]);
									printf("\nMove this energy card (y/n)? ");
									fgets(buffer, 50, stdin);
									char answer;
									sscanf(buffer, "%c", &answer);
									
									if (answer == 'y') {
										energy = pay(attacker->active, index);
										sprintf(buffer, "%d", index);
										messenger_send(messenger, buffer);
										break;
									}
								}
								
								else {
									printf("\nInvalid index.\n");
								}
							}
							
							printf("\n");
						}
						
						else {
							messenger_read(messenger, buffer, 50);
							int index;
							sscanf(buffer, "%d", &index);
							energy = pay(attacker->active, index);
						}
						
						attach(destination, energy);
						
						printf(
							"%s was moved to %s.\n",
							energy->name,
							destination->name
						);
					}
				}
			}
		}

		// Recovers health
		else if (!strcmp(word, "recover")) {
			int recovery;
			sscanf(buffer, "%*s%d", &recovery);
			
			attacker->active->health[0] += recovery;
			
			// If the fighter's health
			if (attacker->active->health[0] > attacker->active->health[1]) {
				recovery -=
					attacker->active->health[0]
					- attacker->active->health[1]
				;
				
				attacker->active->health[0] = attacker->active->health[1];
			}
			
			printf(
				"%s recovered %d health.\n",
				attacker->active->name,
				recovery
			);
		}
	
		// Attacks a fighter
		else if (!strcmp(word, "snipe")) {
			sscanf(buffer, "%*s%s", word);
			
			// Attacks a fighter on the bench
			if (!strcmp(word, "bench")) {
				bool bench_empty = true;
				
				for (int i = 0; i < BENCH_SIZE; i++) {
					if (defender->bench[i]) {
						bench_empty = false;
						break;
					}
				}
				
				if (!bench_empty) {
					sscanf(buffer, "%*s%*s%s", word);
					
					// Attacks two different fighters on the bench
					if (!strcmp(word, "double")) {
						int no_pair = -2;
						
						for (int i = 0; i < BENCH_SIZE && no_pair; i++) {
							if (defender->bench[i]) {
								no_pair++;
							}
						}
						
						int damage;
						sscanf(buffer, "%*s%*s%*s%d", &damage);
						damage += power + boost;
						Fighter* target = NULL;
						int index;
						
						// If ther bench only has one fighter, only one is targeted
						if (no_pair) {
							// The player chooses a fighter to attack
							if (attacker < defender) {
								while (true) {
									printf("\nChoose a benched fighter to damage:\n");
									
									for (int i = 0; i < BENCH_SIZE; i++) {
										if (defender->bench[i]) {
											printf(
												"%s (%d)\n",
												defender->bench[i]->name,
												i
											);
										}
									}
									
									printf("\nIndex: ");
									fgets(buffer, 50, stdin);
									sscanf(buffer, "%d", &index);
									
									if (0 <= index && index < BENCH_SIZE) {
										target = defender->bench[index];
									}
									
									if (target) {
										print_fighter(target);
										display_energy(target);
										printf(
											"\nDeal %d damage to this fighter (y/n)? ",
											damage
										);
										
										fgets(buffer, 50, stdin);
										char answer;
										sscanf(buffer, "%c", &answer);
										
										if (answer == 'y') {
											sprintf(buffer, "%d", index);
											messenger_send(messenger, buffer);
											break;
										}
									}
									
									else {
										printf("\nInvalid index.\n");
									}
								}
								
								printf("\n");
							}
							
							else {
								messenger_read(messenger, buffer, 50);
								sscanf(buffer, "%d", &index);
								target = defender->bench[index];
							}
							
							target->health[0] -= damage;
							
							printf(
								"%s took %d damage.\n",
								target->name,
								damage
							);
						}
					
						// Else, two fighters are chosen
						else {
							Fighter* old_target = NULL;
							
							for (int h = 0; h < 2; h++) {
								// The player chooses a fighter to attack
								if (attacker < defender) {
									while (true) {
										printf("\nChoose a benched fighter to damage:\n");
										
										for (int i = 0; i < BENCH_SIZE; i++) {
											if (defender->bench[i]) {
												printf(
													"%s (%d)\n",
													defender->bench[i]->name,
													i
												);
											}
										}
										
										printf("\nIndex: ");
										fgets(buffer, 50, stdin);
										sscanf(buffer, "%d", &index);
										
										if (0 <= index && index < BENCH_SIZE) {
											target = defender->bench[index];
										}
										
										if (target) {
											print_fighter(target);
											display_energy(target);
											printf(
												"\nDeal %d damage to this fighter (y/n)? ",
												damage
											);
											
											fgets(buffer, 50, stdin);
											char answer;
											sscanf(buffer, "%c", &answer);
											
											if (answer == 'y') {
												if (target != old_target) {
													sprintf(buffer, "%d", index);
													messenger_send(messenger, buffer);
													break;
												}
												
												else {
													printf(
														"\nThe same fighter can't be targeted twice.\n"
													);
												}
											}
										}
										
										else {
											printf("\nInvalid index.\n");
										}
									}
									
									printf("\n");
								}
								
								else {
									messenger_read(messenger, buffer, 50);
									sscanf(buffer, "%d", &index);
									target = defender->bench[index];
								}
								
								target->health[0] -= damage;
								
								printf(
									"%s took %d damage.\n",
									target->name,
									damage
								);
								
								old_target = target;
							}
						}
					}
					
					else {
						int damage;
						sscanf(buffer, "%*s%*s%d", &damage);
						damage += power + boost;
						Fighter* target = NULL;
						int index;
								
						// The player chooses a fighter to attack
						if (attacker < defender) {
							while (true) {
								printf("\nChoose a benched fighter to damage:\n");
								
								for (int i = 0; i < BENCH_SIZE; i++) {
									if (defender->bench[i]) {
										printf(
											"%s (%d)\n",
											defender->bench[i]->name,
											i
										);
									}
								}
								
								printf("\nIndex: ");
								fgets(buffer, 50, stdin);
								sscanf(buffer, "%d", &index);
								
								if (0 <= index && index < BENCH_SIZE) {
									target = defender->bench[index];
								}
								
								if (target) {
									print_fighter(target);
									display_energy(target);
									printf(
										"\nDeal %d damage to this fighter (y/n)? ",
										damage
									);
									
									fgets(buffer, 50, stdin);
									char answer;
									sscanf(buffer, "%c", &answer);
									
									if (answer == 'y') {
										sprintf(buffer, "%d", index);
										messenger_send(messenger, buffer);
										break;
									}
								}
								
								else {
									printf("\nInvalid index.\n");
								}
							}
							
							printf("\n");
						}
						
						else {
							messenger_read(messenger, buffer, 50);
							sscanf(buffer, "%d", &index);
							target = defender->bench[index];
						}
						
						target->health[0] -= damage;
						
						printf(
							"%s took %d damage.\n",
							target->name,
							damage
						);
					}
				}
			}
			
			else {
				int damage;
				sscanf(buffer, "%*s%d", &damage);
				damage += power + boost;
				Fighter* target = NULL;
				int index;
						
				// The player chooses a fighter to attack
				if (attacker < defender) {
					while (true) {
						printf(
							"\nChoose a fighter to damage:\n%s (0)\n",
							defender->active->name
						);
						
						for (int i = 0; i < BENCH_SIZE; i++) {
							if (defender->bench[i]) {
								printf(
									"%s (%d)\n",
									defender->bench[i]->name,
									i + 1
								);
							}
						}
						
						printf("\nIndex: ");
						fgets(buffer, 50, stdin);
						sscanf(buffer, "%d", &index);
						
						if (!index) {
							target = defender->active;
						}
						
						else if (0 < index && index <= BENCH_SIZE) {
							target = defender->bench[index - 1];
						}
						
						if (target) {
							print_fighter(target);
							
							if (target == defender->active) {
								printf(
									"\nStamina: %d\n%s%s",
									effect_count_f(
										target,
										"stamina"
									),
									effect_check(
										target,
										"invincible"
									) ? "Invincible\n"
									: "",
									effect_check(
										target,
										"immune"
									) ? "Immune\n"
									: ""
								);
							}
							
							display_energy(target);
							printf(
								"\nDeal %d damage to this fighter (y/n)? ",
								damage
							);
							
							fgets(buffer, 50, stdin);
							char answer;
							sscanf(buffer, "%c", &answer);
							
							if (answer == 'y') {
								sprintf(buffer, "%d", index);
								messenger_send(messenger, buffer);
								break;
							}
						}
						
						else {
							printf("\nInvalid index.\n");
						}
					}
					
					printf("\n");
				}
				
				else {
					messenger_read(messenger, buffer, 50);
					sscanf(buffer, "%d", &index);
					
					if (!index) {
						target = defender->active;
					}
					
					else if (0 < index && index <= BENCH_SIZE) {
						target = defender->bench[index - 1];
					}
				}
				
				if (effect_check(target, "invincible") || effect_check(target, "immune")) {
					printf(
						"%s is invincible and took no damage.\n",
						target->name
					);
				}
				
				else {
					target->health[0] -= damage;
					
					printf(
						"%s took %d damage.\n",
						target->name,
						damage
					);
				}
			}
		}

		// Discards attached energy
		else if (!strcmp(word, "pay")) {
			sscanf(buffer, "%*s%s", word);
			
			if (!strcmp(word, "all")) {
				while (attacker->active->energy_count) {
					store_energy(
						stores[0],
						pay(
							attacker->active,
							attacker->active->energy_count - 1
						)
					);
				}
				
				printf(
					"%s discarded all of its energy.\n",
					attacker->active->name
				);
			}
			
			else {
				int count;
				sscanf(buffer, "%*s%d", &count);
				
				// Energy is discarded
				for (int i = 0; i < count && attacker->active->energy_count; i++) {
					Energy* energy;
					
					if (attacker < defender) {
						while (true) {
							printf("\nEnergy to discard from the active fighter:");
							display_energy(attacker->active);
							printf("\nIndex: ");
							fgets(buffer, 50, stdin);
							int index;
							sscanf(buffer, "%d", &index);
							
							if (0 <= index && index < attacker->active->energy_count) {
								print_energy(attacker->active->energy[index]);
								printf("\nDiscard this energy card (y/n)? ");
								fgets(buffer, 50, stdin);
								char answer;
								sscanf(buffer, "%c", &answer);
								
								if (answer == 'y') {
									energy = pay(attacker->active, index);
									sprintf(buffer, "%d", index);
									messenger_send(messenger, buffer);
									break;
								}
							}
							
							else {
								printf("\nInvalid index.\n");
							}
						}
						
						printf("\n");
					}
					
					else {
						messenger_read(messenger, buffer, 50);
						int index;
						sscanf(buffer, "%d", &index);
						energy = pay(attacker->active, index);
					}
					
					store_energy(stores[0], energy);
					printf(
						"%s was discarded.\n",
						energy->name
					);
				}
			}
		}

		// Deals extra damage to non-water element fighters
		else if (!strcmp(word, "plankwalk")) {
			int plankwalk;
			sscanf(buffer, "%*s%d", &plankwalk);
			
			// Not of the water element (increased damage)
			if (strcmp(defender->active->element, "Water")) {
				boost += plankwalk;
			}
		}

		// Doubles the defending fighter's retreat cost
		else if (!strcmp(word, "gravity")) {
			if (effect_check(defender->active, "immune")) {
				printf(
					"%s is immune to all effects and cannot have "
					"its retreat cost doubled.\n",
					defender->active->name
				);
			}
			
			else {
				affect_fighter(defender->active, "gravity");
				
				printf(
					"The retreat cost of %s is doubled during your%s next turn.\n",
					defender->active->name,
					attacker < defender ? " opponent's" : ""
				);
			}
		}

		// Deals damage to the attacking fighter
		else if (!strcmp(word, "recoil")) {
			int damage;
			sscanf(buffer, "%*s%d", &damage);
			damage += power;
			attacker->active->health[0] -= damage;
			
			printf(
				"%s dealt %d damage to itself.\n",
				attacker->active->name,
				damage
			);
		}
	
		// Discards energy cards attached the the defending fighter
		else if (!strcmp(word, "drain")) {
			int count;
			sscanf(buffer, "%*s%d", &count);
			
			if (effect_check(defender->active, "immune")) {
				printf(
					"%s is immune to all effects and can't have its energy drained.\n",
					defender->active->name
				);
			}
			
			else {
				for (int i = 0; i < count && defender->active->energy_count; i++) {
					Energy* drained;
					
					if (attacker < defender) {
						while (true) {
							print_fighter(defender->active);
							printf(
								"\nStamina: %d\n%s%s",
								effect_count_f(
									defender->active,
									"stamina"
								),
								effect_check(
									defender->active,
									"invincible"
								) ? "Invincible\n"
								: "",
								effect_check(
									defender->active,
									"immune"
								) ? "Immune\n"
								: ""
							);
							printf("\nChoose the energy card to discard:");
							display_energy(defender->active);
							printf("\nIndex: ");
							fgets(buffer, 50, stdin);
							int index;
							sscanf(buffer, "%d", &index);
							
							if (0 <= index && index < defender->active->energy_count) {
								print_energy(defender->active->energy[index]);
								printf("\nDiscard this energy card (y/n)? ");
								fgets(buffer, 50, stdin);
								char answer;
								sscanf(buffer, "%c", &answer);
								
								if (answer == 'y') {
									sprintf(buffer, "%d", index);
									messenger_send(messenger, buffer);
									drained = pay(defender->active, index);
									break;
								}
							}
							
							else {
								printf("\nInvalid index.\n");
							}
						}
						
						printf("\n");
					}
					
					else {
						messenger_read(messenger, buffer, 50);
						int index;
						sscanf(buffer, "%d", &index);
						drained = pay(defender->active, index);
					}
					
					store_energy(stores[1], drained);
					
					printf(
						"%s was discarded from %s.\n",
						drained->name,
						defender->active->name
					);
				}
			}
		}
		
		// Deals damage to all of a player's benched fighters
		else if (!strcmp(word, "splash")) {
			sscanf(buffer, "%*s%s", word);
			
			// Own bench
			if (!strcmp(word, "self")) {
				bool bench_empty = true;
				
				for (int i = 0; i < BENCH_SIZE; i++) {
					if (attacker->bench[i]) {
						bench_empty = false;
						break;
					}
				}
			
				if (!bench_empty) {
					int damage;
					sscanf(buffer, "%*s%*s%d", &damage);
					damage += power + boost;
					
					for (int i = 0; i < BENCH_SIZE; i++) {
						if (attacker->bench[i]) {
							attacker->bench[i]->health[0] -= damage;
						}
					}
					
					printf(
						"%s dealt %d damage to your%s benched fighters.\n",
						attacker->active->name,
						damage,
						attacker > defender ? " opponent's" : ""
					);
				}
			}
			
			// Enemy bench
			else {
				bool bench_empty = true;
				
				for (int i = 0; i < BENCH_SIZE; i++) {
					if (defender->bench[i]) {
						bench_empty = false;
						break;
					}
				}
			
				if (!bench_empty) {
					int damage;
					sscanf(buffer, "%*s%d", &damage);
					damage += power + boost;
					
					for (int i = 0; i < BENCH_SIZE; i++) {
						if (defender->bench[i]) {
							defender->bench[i]->health[0] -= damage;
						}
					}
					
					printf(
						"%s dealt %d damage to your%s benched fighters.\n",
						attacker->active->name,
						damage,
						attacker < defender ? " opponent's" : ""
					);
				}
			}
		}
	
		// Switches the fighter out
		else if (!strcmp(word, "switch_out")) {
			bool bench_empty = true;
				
			for (int i = 0; i < BENCH_SIZE; i++) {
				if (attacker->bench[i]) {
					bench_empty = false;
					break;
				}
			}
				
			if (!bench_empty) {
				int index;
			
				if (attacker < defender) {
					while (true) {
						printf("\nChoose a fighter to switch in:\n");
						
						for (int i = 0; i < BENCH_SIZE; i++) {
							if (attacker->bench[i]) {
								printf(
									"%s (%d)\n",
									attacker->bench[i]->name,
									i
								);
							}
						}
						
						printf("Index: ");
						fgets(buffer, 50, stdin);
						sscanf(buffer, "%d", &index);
						
						if (0 <= index && index < BENCH_SIZE && attacker->bench[index]) {
							print_fighter(attacker->bench[index]);
							display_energy(attacker->bench[index]);
							printf("\nSwitch in this fighter (y/n)? ");
							fgets(buffer, 50, stdin);
							char answer;
							sscanf(buffer, "%c", &answer);
							
							if (answer == 'y') {
								sprintf(buffer, "%d", index);
								messenger_send(messenger, buffer);
								break;
							}
						}
						
						else {
							printf("\nInvalid index.\n");
						}
					}
					
					printf("\n");
				}
				
				else {
					messenger_read(messenger, buffer, 50);
					sscanf(buffer, "%d", &index);
				}
				
				Fighter* temp = attacker->active;
				attacker->active = attacker->bench[index];
				attacker->bench[index] = temp;
				
				printf(
					"%s switched out.\n"
					"%s switched in.\n",
					temp->name,
					attacker->active->name
				);
			}
		}

		// Removes the opponent's prizes
		else if(!strcmp(word, "deprize")) {
			sscanf(buffer, "%*s%s", word);
			
			// Banishes the prizes
			if (!strcmp(word, "void")) {
				int count;
				sscanf(buffer, "%*s%*s%d", &count);
				
				for (int i = 0; i < count && card_count(defender->prizes); i++) {
					int counts[3] = {
						the_void->fighter_count,
						the_void->supporter_count,
						the_void->energy_count
					};
					
					random_draw(&defender->prizes, the_void);
					
					if (counts[0] < the_void->fighter_count) {
						printf(
							"%s was banished from your%s prizes.\n",
							the_void->fighters[counts[0]]->name,
							attacker < defender ? " opponent's" : ""
						);
					}
					
					else if (counts[1] < the_void->supporter_count) {
						printf(
							"%s was banished from your%s prizes.\n",
							the_void->supporters[counts[1]]->name,
							attacker < defender ? " opponent's" : ""
						);
					}
					
					else {
						printf(
							"%s was banished from your%s prizes.\n",
							the_void->energy[counts[2]]->name,
							attacker < defender ? " opponent's" : ""
						);
					}
				}
			}
		}
		
		// Banishes the defending fighter if it is defeated
		else if (!strcmp(word, "redirect")) {
			sscanf(buffer, "%*s%s", word);
			int lengths[2] = {strlen("redirect"), strlen(word)};
			char* redirection = malloc(lengths[0] + lengths[1] + 2);
			
			for (int i = 0; i < lengths[0]; i++) {
				redirection[i] = "redirect"[i];
			}
			
			redirection[lengths[0]] = ' ';
			
			for (int i = 0; i < lengths[1]; i++) {
				redirection[i + lengths[0] + 1] = word[i];
			}
			
			redirection[lengths[0] + lengths[1] + 1] = '\0';
			affect_fighter(defender->active, redirection);
			free(redirection);
			
			printf(
				"%s was marked for banishment.\n",
				defender->active->name
			);
		}

		// Uses another fighter's attack
		else if (!strcmp(word, "talk")) {
			sscanf(buffer, "%*s%s", word);
			
			// Trash
			if (!strcmp(word, "trash")) {
				// A fighter's attack is chosen
				if (attacker->trash.fighter_count) {
					// Attacker
					if (attacker < defender) {
						while (true) {
							printf("\nFighter to copy:\n");
							
							for (int i = 0; i < attacker->trash.fighter_count; i++) {
								printf(
									"%s (%d)\n",
									attacker->trash.fighters[i]->name,
									i
								);
							}
							
							printf("\nIndex: ");
							fgets(buffer, 50, stdin);
							int index;
							sscanf(buffer, "%d", &index);
							
							if (0 <= index && index < attacker->trash.fighter_count) {
								printf("\nChoose an attack to copy:\n");
								
								for (int i = 0; i < 2; i++) {
									if (attacker->trash.fighters[index]->attacks[i]) {
										printf("\n%d:\n", i);
										print_attack(
											attacker->trash.fighters[index]->attacks[i]
										);
									}
								}
								
								printf("\nAttack index (-1 to cancel): ");
								fgets(buffer, 50, stdin);
								int index2;
								sscanf(buffer, "%d", &index2);
								
								if (
									0 <= index2 && index2 < 2
									&& attacker->trash.fighters[index]->attacks[index2]
								) {
									printf("\n");
									print_attack(attacker->trash.fighters[index]->attacks[index2]);
									printf("\nUse this attack (y/n)? ");
									fgets(buffer, 50, stdin);
									char answer;
									sscanf(buffer, "%c", &answer);
									
									// The attack is sent to the defender
									if (answer == 'y') {
										sprintf(buffer, "%d %d", index, index2);
										messenger_send(messenger, buffer);
										attack = attacker->trash.fighters[index]->attacks[index2];
										break;
									}
								}
							}
							
							else {
								printf("\nInvalid index.\n");
							}
						}
						
						printf("\n");
					}
					
					// Defender
					else {
						messenger_read(messenger, buffer, 50);
						int index;
						int index2;
						sscanf(buffer, "%d%d", &index, &index2);
						attack = attacker->trash.fighters[index]->attacks[index2];
					}
					
					effect_index = 0;
					end = false;
					
					printf(
						"%s used %s.\n",
						attacker->active->name,
						attack->name
					);
				}
			
				// No fighters to choose from
				else {
					printf("There are no fighters in your trash...\n");
				}
			}
			
			// Void
			else if (!strcmp(word, "void")) {
				// A fighter's attack is chosen
				if (the_void->fighter_count) {
					// Attacker
					if (attacker < defender) {
						while (true) {
							printf("\nFighter to copy:\n");
							
							for (int i = 0; i < the_void->fighter_count; i++) {
								printf(
									"%s (%d)\n",
									the_void->fighters[i]->name,
									i
								);
							}
							
							printf("\nIndex: ");
							fgets(buffer, 50, stdin);
							int index;
							sscanf(buffer, "%d", &index);
							
							if (0 <= index && index < the_void->fighter_count) {
								printf("\nChoose an attack to copy:\n");
								
								for (int i = 0; i < 2; i++) {
									if (the_void->fighters[index]->attacks[i]) {
										printf("\n%d:\n", i);
										print_attack(
											the_void->fighters[index]->attacks[i]
										);
									}
								}
								
								printf("\nAttack index (-1 to cancel): ");
								fgets(buffer, 50, stdin);
								int index2;
								sscanf(buffer, "%d", &index2);
								
								if (
									0 <= index2 && index2 < 2
									&& the_void->fighters[index]->attacks[index2]
								) {
									printf("\n");
									print_attack(the_void->fighters[index]->attacks[index2]);
									printf("\nUse this attack (y/n)? ");
									fgets(buffer, 50, stdin);
									char answer;
									sscanf(buffer, "%c", &answer);
									
									// The attack is sent to the defender
									if (answer == 'y') {
										sprintf(buffer, "%d %d", index, index2);
										messenger_send(messenger, buffer);
										attack = the_void->fighters[index]->attacks[index2];
										break;
									}
								}
							}
							
							else {
								printf("\nInvalid index.\n");
							}
						}
						
						printf("\n");
					}
					
					// Defender
					else {
						messenger_read(messenger, buffer, 50);
						int index;
						int index2;
						sscanf(buffer, "%d%d", &index, &index2);
						attack = the_void->fighters[index]->attacks[index2];
					}
					
					effect_index = 0;
					end = false;
					
					printf(
						"%s used %s.\n",
						attacker->active->name,
						attack->name
					);
				}
			
				// No fighters to choose from
				else {
					printf("There are no fighters in the void...\n");
				}
			}
		}

		// Deals more damage for each fighter in the void
		else if (!strcmp(word, "lost_wail")) {
			double value;
			sscanf(buffer, "%*s%lf", &value);
			
			boost += value * the_void->fighter_count;
		}
		
		// Banishes both fighters
		else if (!strcmp(word, "void_pact")) {
			unaffect_fighter(attacker->active, "");
			affect_fighter(attacker->active, "lost");
			attacker->active->health[0] = 0;
			
			printf(
				"%s was banished!\n",
				attacker->active->name
			);
			
			// Immunity prevents banishment
			if (effect_check(defender->active, "immune")) {
				printf(
					"%s is immune to all effects and can't be banished.\n",
					defender->active->name
				);
			}
			
			else {
				unaffect_fighter(defender->active, "");
				affect_fighter(defender->active, "lost");
				defender->active->health[0] = 0;
				
				printf(
					"%s was banished!\n",
					defender->active->name
				);
			}
		}
	}
	
	// Damage is calculated
	int damage = attack->damage + boost;
	
	// Attacks only get a power boost if they already deal damage
	if (damage) {
		damage += power;
		
		// Invincibility and immunity prevent all damage
		if (
			effect_check(defender->active, "invincible")
			|| effect_check(defender->active, "immune")
		) {
			damage = 0;
			
			printf(
				"%s is invincible and took no damage.\n",
				defender->active->name
			);
		}
	}
	
	// Damage is dealt
	defender->active->health[0] -= damage;
	
	if (damage) {
		printf(
			"%s took %d damage.\n",
			defender->active->name,
			damage
		);
	}
}

/* Uses a fighter's ability
 */
void ability(
	Player* attacker, Player* defender, int index,
	CardStore* the_void, Messenger* messenger
) {
	int effect_index = 0;
	bool end = false;
	char buffer[50];
	char word[20];
	
	CardStore* stores[2] = {
		effect_search(*attacker, "banish")
		? the_void
		: &attacker->trash,
		effect_search(*defender, "banish")
		? the_void
		: &defender->trash
	};
	
	Fighter* fighter =
		index
		? attacker->bench[index - 1]
		: attacker->active
	;
	
	printf(
		"\n%s used %s.\n",
		fighter->name,
		fighter->ability->name
	);
	
	while (!end) {
		for (int i = 0;; i++, effect_index++) {
			buffer[i] =
				fighter->ability->effects[effect_index]
			;
			
			if (buffer[i] == '\n') {
				effect_index++;
				break;
			}
			
			else if (buffer[i] == '\0') {
				end = true;
				break;
			}
		}
		
		sscanf(buffer, "%s", word);
		
		// Abilities that switch the fighter in can only be used on the bench
		if (!strcmp(word, "switch_in")) {
			if (fighter == attacker->active) {
				printf("This ability can only be used on the bench...\n");
				end = true;
			}
			
			else {
				Fighter* temp = attacker->active;
				attacker->active = attacker->bench[index - 1];
				attacker->bench[index - 1] = temp;
				
				printf(
					"%s switched in.\n%s was switched out.\n",
					fighter->name,
					temp->name
				);
			}
		}
		
		// Increases a fighter's power
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
			affect_fighter(fighter, power_effect);
			
			// The string is freed after use
			free(power_effect);
			
			printf(
				"%s attacks deal %d more damage this turn.\n",
				fighter->name,
				power
			);
		}
		
		// Draws cards
		else if (!strcmp(word, "draw")) {
			sscanf(buffer, "%*s%s", word);
			
			// Either player can draw a card at the player's discretion
			if (!strcmp(word, "either")) {
				int answer;
				int draw;
				
				// The attacker chooses who draws
				if (attacker < defender) {
					while (true) {
						printf("\nShould you (0) or your opponent (1) draw? ");
						fgets(buffer, 50, stdin);
						sscanf(buffer, "%d", &answer);
						
						if (answer == 0 || answer == 1) {
							break;
						}
						
						else {
							printf("\nInvalid input.\n");
						}
					}
					
					messenger_send(messenger, buffer);
				}
				
				// The defender listens for who will draw
				else {
					messenger_read(messenger, buffer, 50);
					sscanf(buffer, "%d", &answer);
				}
				
				// The defender is to draw
				if (answer) {
					 sscanf(buffer, "%*s%*s%*d%d", &draw);
					 
					// Cards are drawn
					for (int i = 0; i < draw; i++) {
						// Cards cannot be drawn from an empty deck
						if (!card_count(defender->deck)) {
							draw = i;
							break;
						}
						
						int counts[3] = {
							defender->hand.fighter_count,
							defender->hand.supporter_count,
							defender->hand.energy_count
						};
						
						random_draw(&defender->deck, &defender->hand);
						
						// The card drawn is displayed to the owner
						if (defender < attacker) {
							if (counts[0] < defender->hand.fighter_count) {
								printf(
									"You drew %s.\n",
									defender->hand.fighters[counts[0]]->name
								);
							}
							
							else if (counts[1] < defender->hand.supporter_count) {
								printf(
									"You drew %s.\n",
									defender->hand.supporters[counts[1]]->name
								);
							}
							
							else {
								printf(
									"You drew %s.\n",
									defender->hand.energy[counts[2]]->name
								);
							}
						}
					}
					
					// The number of cards drawn is displayed to the other player
					if (attacker < defender) {
						printf("Your opponent drew %d cards.\n", draw);
					}
				}
				
				// The attacker is to draw
				else {
					 sscanf(buffer, "%*s%*s%d", &draw);
					 
					// Cards are drawn
					for (int i = 0; i < draw; i++) {
						// Cards cannot be drawn from an empty deck
						if (!card_count(attacker->deck)) {
							draw = i;
							break;
						}
						
						int counts[3] = {
							attacker->hand.fighter_count,
							attacker->hand.supporter_count,
							attacker->hand.energy_count
						};
						
						random_draw(&attacker->deck, &attacker->hand);
						
						// The card drawn is displayed to the owner
						if (attacker < defender) {
							if (counts[0] < attacker->hand.fighter_count) {
								printf(
									"\nYou drew %s.\n",
									attacker->hand.fighters[counts[0]]->name
								);
							}
							
							else if (counts[1] < attacker->hand.supporter_count) {
								printf(
									"\nYou drew %s.\n",
									attacker->hand.supporters[counts[1]]->name
								);
							}
							
							else {
								printf(
									"\nYou drew %s.\n",
									attacker->hand.energy[counts[2]]->name
								);
							}
						}
					}
					
					// The number of cards drawn is displayed to the other player
					if (defender < attacker) {
						printf("Your opponent drew %d cards.\n", draw);
					}
				}
			}
		}

		// Sacrifices a friendly fighter for health gain
		else if (!strcmp(word, "sacrifice")) {
			bool bench_empty = true;
			
			for (int i = 0; i < BENCH_SIZE; i++) {
				if (attacker->bench[i]) {
					bench_empty = false;
					break;
				}
			}
			
			if (bench_empty) {
				printf("There is nothing to sacrifice...\n");
			}
			
			else {
				// The player chooses their sacrifice
				if (attacker < defender) {
					while (true) {
						printf("\n%s (0)\n", attacker->active->name);
					
						for (int i = 0; i < BENCH_SIZE; i++) {
							if (attacker->bench[i]) {
								printf(
									"%s (%d)\n",
									attacker->bench[i]->name,
									i + 1
								);
							}
						}
						
						printf("\nIndex: ");
						fgets(buffer, 50, stdin);
						sscanf(buffer, "%d", &index);
						
						Fighter* sacrifice = NULL;
						
						if (!index) {
							sacrifice = attacker->active;
						}
						
						else if (0 < index && index <= BENCH_SIZE) {
							if (attacker->bench[index - 1]) {
								sacrifice = attacker->bench[index - 1];
							}
						}
						
						else {
							printf("\nInvalid index.\n");
						}
						
						// A fighter cannot sacrifice itself
						if (sacrifice == fighter) {
							printf("\nA fighter cannot sacrifice itself.\n");
						}
						
						else {
							break;
						}
					}
					
					// The chosen sacrifice is relayed to the other player
					sprintf(buffer, "%d", index);
					messenger_send(messenger, buffer);
					printf("\n");
				}
				
				else {
					messenger_read(messenger, buffer, 50);
					sscanf(buffer, "%d", &index);
				}
				
				Fighter* sacrifice =
					index
					? attacker->bench[index - 1]
					: attacker->active
				;
				
				// The chosen fighter is defeated and the ability user
				//   gains stamina equal to the remaining health and
				//   stamina of the sacrifice
				int stamina =
					fighter == attacker->active
					? sacrifice->health[0]
					: 0
				;
				
				unaffect_fighter(sacrifice, "stamina");
				sacrifice->health[0] = 0;
				
				int stamina_length = strlen("stamina ");
			
				char* stamina_effect = malloc(
					(stamina_length + 3) * sizeof(char)
				);
				
				for (int i = 0; i < stamina_length; i++) {
					stamina_effect[i] = "stamina "[i];
				}
				
				sprintf(stamina_effect + stamina_length, "%d", stamina);
				
				// The effect is applied to the player
				affect_fighter(fighter, stamina_effect);
				
				// The string is freed after use
				free(stamina_effect);
				
				printf(
					"%s was sacrificed.\n"
					"%s gained %d health.\n",
					sacrifice->name,
					fighter->name,
					stamina
				);
			}
		}
	}
	
	// The number of ability uses is incremented
	fighter->uses++;
}

/* A card game based on the PTCG, but using
     cards based on things in real life
   This is the server-side version of the program
 */
int main(int argc, char* argv[]) {
	int port;
	char message[BUFFER_SIZE];
	
	// SDL and SDL Net are initialised
	sdlnet_init(0);
	
	// If the port was given through the command line, it is used
	if (argc > 1) {
		sscanf(argv[1], "%d", &port);
	}
	
	// Else, it is prompted for
	else {
		printf("Port: ");
		fgets(message, BUFFER_SIZE, stdin);
		sscanf(message, "%d", &port);
	}
	
	// The server is initialised
	printf("\nWaiting for the client...\n");
	Server server;
	server_init(&server, port);
	
	// The game begins once a connection is established
	printf(
		"\nLife TCG by Chigozie Agomo\nServer Version: %s\n",
		VERSION
	);
	
	// The RNG seed is sent to the client
	int seed = time(NULL);
	sprintf(message, "%d", seed);
	server_send(&server, message);
	
	// The RNG is seeded
	srand(seed);
	
	while (true) {
		// The deck is built and its code is stored
		build_deck(message);
		
		// The players' are initialised with the deck codes
		Player players[2];
		player_init(players + 0, message);
		
		printf("\nYour deck:");
		print_store(players[0].deck);
		
		messenger_send(&messenger, message);
		messenger_read(&messenger, message, BUFFER_SIZE);
		
		player_init(players + 1, message);
		
		// The player to go first is determined
		int turn = flip;
		printf("\n%s go%s first.\n", turn ? "Your opponent" : "You", turn ? "es" : "");
		
		// Players can draw an extra card for each time when their opponent
		//   fails to draw an rank 0 fighter (if the player has a valid hand)
		int mulligan[2] = {0, 0};
		
		// Each player draws their starting hand
		for (int i = 0; i < HAND_SIZE; i++) {
			random_draw(&players[turn].deck, &players[turn].hand);
			random_draw(&players[1 - turn].deck, &players[1 - turn].hand);
		}
		
		// Check for rank 0 fighters in hand
		while (true) {
			bool basics[2] = {false, false};
			
			for (int i = 0; i < players[0].hand.fighter_count; i++) {
				if (!players[0].hand.fighters[i]->rank) {
					basics[0] = true;
					break;
				}
			}
			
			for (int i = 0; i < players[1].hand.fighter_count; i++) {
				if (!players[1].hand.fighters[i]->rank) {
					basics[1] = true;
					break;
				}
			}
			
			// If a player drew 0 rank 0 fighters, they reveal their hand and redraw
			if (!basics[0]) {
				printf("\nYou didn't draw any rank 0 fighters:");
				print_store(players[0].hand);
			}
			
			if (!basics[1]) {
				printf("\nYour opponent didn't draw any rank 0 fighters:");
				print_store(players[1].hand);
			}
			
			if (basics[0] && basics[1]) {
				break;
			}
			
			else if (!basics[0] && basics[1]) {
				printf("\nYou will shuffle your hand into your deck and draw again.\n\n");
				for (int i = HAND_SIZE - 1; i > -1; i--) {
					rigged_draw(&players[0].hand, &players[0].deck, i);
				}
				
				for (int i = 0; i < HAND_SIZE; i++) {
					random_draw(&players[0].deck, &players[0].hand);
				}
				
				mulligan[1]++;
			}
			
			else if (basics[0] && !basics[1]) {
				printf(
					"\nYour opponent will shuffle their hand into their deck and draw again.\n\n"
				);
				
				for (int i = HAND_SIZE - 1; i > -1; i--) {
					rigged_draw(&players[1].hand, &players[1].deck, i);
				}
				
				for (int i = 0; i < HAND_SIZE; i++) {
					random_draw(&players[1].deck, &players[1].hand);
				}
				
				mulligan[0]++;
			}
			
			else {
				printf(
					"\nBoth players will shuffle their hands into their decks and draw again.\n\n"
				);
				
				for (int i = HAND_SIZE - 1; i > -1; i--) {
					rigged_draw(&players[0].hand, &players[0].deck, i);
					rigged_draw(&players[1].hand, &players[1].deck, i);
				}
				
				for (int i = 0; i < HAND_SIZE; i++) {
					random_draw(&players[turn].deck, &players[turn].hand);
					random_draw(&players[1 - turn].deck, &players[1 - turn].hand);
				}
			}
		}
		
		printf("\nYour hand:");
		print_store(players[0].hand);
		
		// Players can draw extra cards off of the mulligan
		if (mulligan[0]) {
			printf(
				"\nYou may draw up to %d extra cards.\n"
				"How many cards would you like to draw? ",
				mulligan[0]
			);
			fgets(message, BUFFER_SIZE, stdin);
			int draw;
			sscanf(message, "%d", &draw);
			
			if (0 < draw && draw <= mulligan[0]) {
				for (int i = 0; i < draw; i++) {
					random_draw(&players[0].deck, &players[0].hand);
				}
			}
			
			else {
				sprintf(message, "%d", 0);
			}
				
			messenger_send(&messenger, message);
		}
		
		else if (mulligan[1]) {
			printf("\nYour opponent may draw up to %d extra cards.\n", mulligan[1]);
			messenger_read(&messenger, message, BUFFER_SIZE);
			int draw;
			sscanf(message, "%d", &draw);
			
			for (int i = 0; i < draw; i++) {
				random_draw(&players[1].deck, &players[1].hand);
			}
			
			printf("Your opponent drew %d extra cards.\n", draw);
		}
		
		// The indices for choice are stored
		int choice[6] = {-1, -1, -1, -1, -1, -1};
		
		// The active fighter is chosen
		while (true) {
			printf("\nChoose your active fighter: ");
			print_store(players[0].hand);
			printf("\nIndex: ");
			fgets(message, BUFFER_SIZE, stdin);
			int index;
			sscanf(message, "%d", &index);
			
			// The request is rejected, if an invalid index is given
			if (index < 0 || index >= card_count(players[0].hand)) {
				printf("\nInvalid input.\n");
			}
			
			// The request is rejected if a supporter or energy card is chosen
			else if (index >= players[0].hand.fighter_count) {
				printf("\nOnly fighters can be chosen.\n");
				
				index -= players[0].hand.fighter_count;
				
				// Supporter
				if (index < players[0].hand.supporter_count) {
					print_supporter(players[0].hand.supporters[index]);
				}
				
				// Energy
				else {
					index -= players[0].hand.supporter_count;
					print_energy(players[0].hand.energy[index]);
				}
			}
			
			else {
				// The request is rejected if a rank 1 or 2 fighter is chosen
				if (players[0].hand.fighters[index]->rank) {
					printf("\nOnly rank 0 fighters can be chosen.\n");
					print_fighter(players[0].hand.fighters[index]);
				}
				
				// The rank 0 fighter is chosen as the active fighter
				else {
					print_fighter(players[0].hand.fighters[index]);
					printf("\nPlay this fighter (y/n)? ");
					fgets(message, BUFFER_SIZE, stdin);
					char answer;
					sscanf(message, "%c", &answer);
					
					if (answer == 'y') {
						players[0].active = get_fighter(&players[0].hand, index);
						choice[0] = index;
						break;
					}
				}
			}
		}
		
		// The benched fighters are chosen
		while (true) {
			printf("\nChoose your benched fighters: ");
			print_store(players[0].hand);
			printf("\nIndex (-1 to stop): ");
			fgets(message, BUFFER_SIZE, stdin);
			int index;
			sscanf(message, "%d", &index);
			
			// The request is rejected, if an invalid index is given
			if (index < -1 || index >= card_count(players[0].hand)) {
				printf("\n\nInvalid input.\n");
			}
			
			// The request is rejected if a supporter or energy card is chosen
			else if (index >= players[0].hand.fighter_count) {
				printf("\nOnly fighters can be chosen.\n");
				
				index -= players[0].hand.fighter_count;
				
				// Supporter
				if (index < players[0].hand.supporter_count) {
					print_supporter(players[0].hand.supporters[index]);
				}
				
				// Energy
				else {
					index -= players[0].hand.supporter_count;
					print_energy(players[0].hand.energy[index]);
				}
			}
			
			else if (index == -1) {
				break;
			}
			
			else {
				// The request is rejected if a rank 1 or 2 fighter is chosen
				if (players[0].hand.fighters[index]->rank) {
					printf("\nOnly rank 0 fighters can be chosen.\n");
					print_fighter(players[0].hand.fighters[index]);
				}
				
				else {
					bool bench_full = true;
					
					for (int i = 0; i < BENCH_SIZE; i++) {
						if (!players[0].bench[i]) {
							print_fighter(players[0].hand.fighters[index]);
							printf("\nPlay this fighter (y/n)? ");
							fgets(message, BUFFER_SIZE, stdin);
							char answer;
							sscanf(message, "%c", &answer);
							
							if (answer == 'y') {
								players[0].bench[i] = get_fighter(&players[0].hand, index);
								bench_full = false;
								choice[i + 1] = index;
							}
							
							break;
						}
					}
					
					if (bench_full) {
						printf("\nThe bench is full.\n");
					}
				}
			}
		}
		
		// The players' board states are swapped
		sprintf(
			message,
			"%d %d %d %d %d %d",
			choice[0],
			choice[1],
			choice[2],
			choice[3],
			choice[4],
			choice[5]
		);
		
		messenger_send(&messenger, message);
		messenger_read(&messenger, message, BUFFER_SIZE);
		
		sscanf(
			message,
			"%d %d %d %d %d %d",
			choice + 0,
			choice + 1,
			choice + 2,
			choice + 3,
			choice + 4,
			choice + 5
		);
		
		// The opponent's board is initialised
		players[1].active = get_fighter(&players[1].hand, choice[0]);
		
		for (int i = 0; i < BENCH_SIZE; i++) {
			if (choice[i + 1] != -1) {
				players[1].bench[i] = get_fighter(&players[1].hand, choice[i + 1]);
			}
		}
		
		// The board is displayed
		printf("\nBench: ");
		
		for (int i = 0; i < BENCH_SIZE; i++) {
			if (players[1].bench[i]) {
				printf("%s ", players[1].bench[i]->name);
			}
		}
		
		printf("\nActive: %s\n", players[1].active->name);
		
		printf("\nActive: %s\nBench: ", players[0].active->name);
		
		for (int i = 0; i < BENCH_SIZE; i++) {
			if (players[0].bench[i]) {
				printf("%s ", players[0].bench[i]->name);
			}
		}
		
		printf("\n");
		
		// Prizes are set up
		for (int i = 0; i < PRIZES; i++) {
			random_draw(&players[turn].deck, &players[turn].prizes);
			random_draw(&players[1 - turn].deck, &players[1 - turn].prizes);
		}
		
		printf("\n%d prizes have been placed.\n", PRIZES);
		
		// Cards sent to the void can never return
		CardStore the_void;
		card_store_init(&the_void);
		
		// True when the game ends
		bool end = false;
		
		// If a fighter with the "Aggressive" ability was placed on
		//   bench, it is switched in unless another fighter with
		//   the ability is in the active position
		for (int i = 0; i < 2; i++) {
			if (!effect_check(players[i].active, "aggro")) {
				for (int j = 0; j < BENCH_SIZE; j++) {
					if (players[i].bench[j]) {
						if (effect_check(players[i].bench[j], "aggro")) {
							Fighter* temp = players[i].active;
							players[i].active = players[i].bench[j];
							players[i].bench[j] = temp;
							unaffect_fighter(players[i].bench[j], "");
							
							printf(
								"\n%s forced its way into the active position!\n"
								"%s switched out.\n",
								players[i].active->name,
								players[i].bench[j]->name
							);
							
							break;
						}
					}
				}
			}
		}
		
		// The game begins
		while (!end) {
			// The player's turn
			if (!turn) {
				// If the deck is empty at the start of the turn, the player loses
				if (!card_count(players[0].deck)) {
					printf(
						"\nYour deck is empty and you cannot draw a card.\n"
						"You lose...\n"
					);
					end = true;
					break;
				}
				
				// The player begins their turn by drawing a card
				printf("\nIt is your turn.\n");
				
				int counts[3] = {
					players[0].hand.fighter_count,
					players[0].hand.supporter_count,
					players[0].hand.energy_count
				};
				
				random_draw(&players[0].deck, &players[0].hand);
				
				if (players[0].hand.fighter_count > counts[0]) {
					printf("\nYou drew %s.\n", players[0].hand.fighters[counts[0]]->name);
				}
				
				else if (players[0].hand.supporter_count > counts[1]) {
					printf("\nYou drew %s.\n", players[0].hand.supporters[counts[1]]->name);
				}
				
				else {
					printf("\nYou drew %s.\n", players[0].hand.energy[counts[2]]->name);
				}
				
				// Only one energy card may be attached from hand per turn
				bool attached = false;
				bool attacked = false;
				
				// The player makes their choices
				while (!attacked) {
					printf(
						"\nHand (h)\n"
						"Check (c)\n"
						"Attack (a)\n"
						"Ability (b)\n"
						"Retreat (r)\n"
						"Done (d)\n"
						"Quit (q)\n"
						"\nChoice: "
					);
					
					fgets(message, BUFFER_SIZE, stdin);
					char option;
					sscanf(message, "%c", &option);
					
					// Ends the player's turn
					if (option == 'd') {
						messenger_send(&messenger, "done");
						break;
					}
					
					// Concedes the game
					else if (option == 'q') {
						messenger_send(&messenger, "quit");
						printf("\n\nYou conceded.\nYou lose...\n");
						end = true;
						break;
					}
					
					// Views the hand to play cards
					else if (option == 'h') {
						printf("\nYour hand:");
						print_store(players[0].hand);
						printf("\nIndex (-1 to cancel): ");
						fgets(message, BUFFER_SIZE, stdin);
						int index;
						sscanf(message, "%d", &index);
						
						// Checks for index validity
						if (0 <= index && index < card_count(players[0].hand)) {
							// Fighter
							if (index < players[0].hand.fighter_count) {
								sprintf(
									message,
									"play fighter %d %d",
									index,
									play_fighter(
										players,
										get_fighter(&players[0].hand, index),
										&the_void
									)
								);
								
								messenger_send(&messenger, message);
							}
							
							else {
								index -= players[0].hand.fighter_count;
								
								// Supporter
								if (index < players[0].hand.supporter_count) {
									sprintf(
										message,
										"play supporter %d %d",
										index,
										play_supporter(
											players,
											get_supporter(&players[0].hand, index),
											&the_void
										)
									);
									
									messenger_send(&messenger, message);
								}
								
								// Energy
								else {
									if (attached) {
										printf("\nOnly one energy card can be played per turn.\n");
									}
									
									else {
										index -= players[0].hand.supporter_count;
										
										int code = play_energy(
											players,
											get_energy(&players[0].hand, index)
										);
										
										attached = code != -1;
										
										sprintf(
											message,
											"play energy %d %d",
											index,
											code
										);
										
										messenger_send(&messenger, message);
									}
								}
							}
						}
					}
					
					// Allow the player to check cards
					else if (option == 'c') {
						printf(
							"\nYour deck: %d\n"
							"Your opponent's deck: %d\n"
							"Your hand: %d\n"
							"Your opponent's hand: %d\n"
							"Your prizes: %d\n"
							"Your opponent's prizes: %d\n"
							"\nPower boost: %d\n"
							"\nYour fighters (0)\n"
							"Your opponent's fighters (1)\n"
							"Your trash (2)\n"
							"Your opponent's trash (3)\n"
							"The void (4)\n"
							"\nCheck (-1 to cancel): ",
							card_count(players[0].deck),
							card_count(players[1].deck),
							card_count(players[0].hand),
							card_count(players[1].hand),
							card_count(players[0].prizes),
							card_count(players[1].prizes),
							effect_count_p(players[0], "power")
						);
						
						fgets(message, BUFFER_SIZE, stdin);
						int check;
						sscanf(message, "%d", &check);
						
						// Player's fighter cards
						if (check == 0) {
							printf("\n%s (0)\n", players[0].active->name);
							
							for (int i = 0; i < BENCH_SIZE; i++) {
								if (players[0].bench[i]) {
									printf("%s (%d)\n", players[0].bench[i]->name, i + 1);
								}
							}
							
							printf("\nCheck (-1 to cancel): ");
							fgets(message, BUFFER_SIZE, stdin);
							sscanf(message, "%d", &check);
							
							// Active fighter
							if (check == 0) {
								print_fighter(players[0].active);
								printf(
									"\nPower boost: %d\n"
									"Stamina boost: %d\n"
									"%s",
									effect_count_f(
										players[0].active,
										"power"
									),
									effect_count_f(
										players[0].active,
										"stamina"
									),
									effect_check(
										players[0].active,
										"gravity"
									) ? "Gravity\n"
									: ""
								);
								display_energy(players[0].active);
							}
							
							// Benched fighter
							else if (1 <= check && check <= BENCH_SIZE) {
								print_fighter(players[0].bench[check - 1]);
								display_energy(players[0].bench[check - 1]);
							}
						}
						
						// Opponent's fighter cards
						else if (check == 1) {
							printf("\n%s (0)\n", players[1].active->name);
							
							for (int i = 0; i < BENCH_SIZE; i++) {
								if (players[1].bench[i]) {
									printf("%s (%d)\n", players[1].bench[i]->name, i + 1);
								}
							}
							
							printf("\nCheck (-1 to cancel): ");
							fgets(message, BUFFER_SIZE, stdin);
							sscanf(message, "%d", &check);
							
							// Active fighter
							if (check == 0) {
								print_fighter(players[1].active);
								printf(
									"\nStamina boost: %d\n%s%s",
									effect_count_f(
										players[1].active,
										"stamina"
									),
									effect_check(
										players[1].active,
										"invincible"
									) ? "Invincible\n"
									: "",
									effect_check(
										players[1].active,
										"immune"
									) ? "Immune\n"
									: ""
								);
								display_energy(players[1].active);
							}
							
							// Benched fighter
							else if (1 <= check && check <= BENCH_SIZE) {
								print_fighter(players[1].bench[check - 1]);
								display_energy(players[1].bench[check - 1]);
							}
						}
						
						// Player's trash
						else if (check == 2) {
							print_store(players[0].trash);
							printf("\nCheck (-1 to cancel): ");
							fgets(message, BUFFER_SIZE, stdin);
							sscanf(message, "%d", &check);
							
							// If a valid index was chosen
							if (0 <= check && check < card_count(players[0].trash)) {
								// A fighter card was chosen
								if (check < players[0].trash.fighter_count) {
									print_fighter(players[0].trash.fighters[check]);
								}
								
								else {
									check -= players[0].trash.fighter_count;
									
									// A supporter card was chosen
									if (check < players[0].trash.supporter_count) {
										print_supporter(players[0].trash.supporters[check]);
									}
									
									// An energy card was chosen
									else {
										check -= players[0].trash.supporter_count;
										print_energy(players[0].trash.energy[check]);
									}
								}
							}
						}
						
						// Opponent's trash
						else if (check == 3) {
							print_store(players[1].trash);
							printf("\nCheck (-1 to cancel): ");
							fgets(message, BUFFER_SIZE, stdin);
							sscanf(message, "%d", &check);
							
							// If a valid index was chosen
							if (0 <= check && check < card_count(players[1].trash)) {
								// A fighter card was chosen
								if (check < players[1].trash.fighter_count) {
									print_fighter(players[1].trash.fighters[check]);
								}
								
								else {
									check -= players[1].trash.fighter_count;
									
									// A supporter card was chosen
									if (check < players[1].trash.supporter_count) {
										print_supporter(players[1].trash.supporters[check]);
									}
									
									// An energy card was chosen
									else {
										check -= players[1].trash.supporter_count;
										print_energy(players[1].trash.energy[check]);
									}
								}
							}
						}
						
						// The void
						else if (check == 4) {
							print_store(the_void);
							printf("\nCheck (-1 to cancel): ");
							fgets(message, BUFFER_SIZE, stdin);
							sscanf(message, "%d", &check);
							
							// If a valid index was chosen
							if (0 <= check && check < card_count(the_void)) {
								// A fighter card was chosen
								if (check < the_void.fighter_count) {
									print_fighter(the_void.fighters[check]);
								}
								
								else {
									check -= the_void.fighter_count;
									
									// A supporter card was chosen
									if (check < the_void.supporter_count) {
										print_supporter(the_void.supporters[check]);
									}
									
									// An energy card was chosen
									else {
										check -= the_void.supporter_count;
										print_energy(the_void.energy[check]);
									}
								}
							}
						}
					}
					
					// Uses the active fighter's attack
					else if (option == 'a') {
						if (players[0].active->attacks[0]) {
							printf("\n0:\n");
							print_attack(players[0].active->attacks[0]);
						}
						
						if (players[0].active->attacks[1]) {
							printf("\n1:\n");
							print_attack(players[0].active->attacks[1]);
						}
						
						printf("\nIndex (-1 to cancel): ");
						fgets(message, BUFFER_SIZE, stdin);
						int index;
						sscanf(message, "%d", &index);
						
						// Valid index
						if (index == 0 || index == 1) {
							if (players[0].active->attacks[index]) {
								// Sufficient energy
								if (
									players[0].active->attacks[index]->cost
									<= energy_count(players[0].active)
								) {
									printf("\n");
									print_attack(players[0].active->attacks[index]);
									printf("\nUse this attack (y/n)? ");
									fgets(message, BUFFER_SIZE, stdin);
									char answer;
									sscanf(message, "%c", &answer);
									
									if (answer == 'y') {
										// Attack command sent
										sprintf(
											message,
											"attack %d",
											index
										);
										messenger_send(&messenger, message);
										
										// Attack performed
										attack(
											players + 0, players + 1, index,
											&the_void, &messenger
										);
										
										// Attacking ends the turn
										attacked = true;
									}
								}
								
								else {
									printf("\nInsufficient energy.\n");
								}
							}
						}
					}
					
					// Uses a fighter's ability
					else if (option == 'b') {
						printf(
							"\nChoose a fighter to use its ability:\n"
							"%s (0)\n",
							players[0].active->name
						);
						
						for (int i = 0; i < BENCH_SIZE; i++) {
							if (players[0].bench[i]) {
								printf(
									"%s (%d)\n",
									players[0].bench[i]->name,
									i + 1
								);
							}
						}
						
						printf("\nIndex (-1 to cancel): ");
						fgets(message, BUFFER_SIZE, stdin);
						int index;
						sscanf(message, "%d", &index);
						
						if (!index) {
							print_fighter(players[0].active);
							
							if (players[0].active->ability) {
								if (!players[0].active->ability->passive) {
									if (
										players[0].active->uses
										!= players[0].active->ability->uses
									) {
										printf("\n");
										print_ability(players[0].active->ability);
										printf("\nUse this ability (y/n)? ");
										fgets(message, BUFFER_SIZE, stdin);
										char answer;
										sscanf(message, "%c", &answer);
										
										// The other player is notified and the ability is used
										if (answer == 'y') {
											sprintf(
												message,
												"ability %d",
												index
											);
											
											messenger_send(&messenger, message);
											
											ability(
												players, players + 1, index,
												&the_void, &messenger
											);
										}
									}
									
									else {
										printf(
											"\nThis ability can only be used %d "
											"time%s per turn.\n",
											players[0].active->uses,
											players[0].active->uses == 1
											? ""
											: "s"
										);
									}
								}
								
								else {
									printf("\nThis ability is passive.\n");
								}
							}
							
							else {
								printf("\nThis fighter doesn't have an ability.\n");
							}
						}
						
						else if (
							0 < index && index <= BENCH_SIZE
							&& players[0].bench[index - 1]
						) {
							print_fighter(players[0].bench[index - 1]);
							
							if (players[0].bench[index - 1]->ability) {
								if (!players[0].bench[index - 1]->ability->passive) {
									if (
										players[0].bench[index - 1]->uses
										!= players[0].bench[index - 1]->ability->uses
									) {
										printf("\n");
										print_ability(players[0].bench[index - 1]->ability);
										printf("\nUse this ability (y/n)? ");
										fgets(message, BUFFER_SIZE, stdin);
										char answer;
										sscanf(message, "%c", &answer);
										
										// The other player is notified and the ability is used
										if (answer == 'y') {
											sprintf(
												message,
												"ability %d",
												index
											);
											
											messenger_send(&messenger, message);
											
											ability(
												players, players + 1, index,
												&the_void, &messenger
											);
										}
									}
									
									else {
										printf(
											"\nThis ability can only be used %d "
											"time%s per turn.\n",
											players[0].bench[index - 1]->uses,
											players[0].bench[index - 1]->uses == 1
											? ""
											: "s"
										);
									}
								}
								
								else {
									printf("\nThis ability is passive.\n");
								}
							}
							
							else {
								printf("\nThis fighter doesn't have an ability.\n");
							}
						}
					}
					
					// Retreats the active fighter to the bench
					else if (option == 'r') {
						bool aided = effect_search(players[0], "retreat_aid");
						bool gravity = effect_check(players[0].active, "gravity");
						
						if (
							energy_any_count(players[0].active)
							< (gravity ? 2 : 1) * players[0].active->retreat
							&& !aided
						) {
							printf("\nInsufficient energy.\n");
						}
						
						else {
							CardStore* store =
								effect_search(players[0], "banish")
								? &the_void
								: &players[0].trash
							;
							
							printf("\n");
							
							for (int i = 0; i < BENCH_SIZE; i++) {
								if (players[0].bench[i]) {
									printf("%s (%d)\n", players[0].bench[i]->name, i);
								}
							}
							
							printf("\nIndex (-1 to cancel): ");
							fgets(message, BUFFER_SIZE, stdin);
							int index;
							sscanf(message, "%d", &index);
							
							if (0 <= index && index < BENCH_SIZE) {
								// A valid benched fighter was chosen
								if (players[0].bench[index]) {
									print_fighter(players[0].bench[index]);
									printf("\nSwitch in this fighter (y/n)? ");
									fgets(message, BUFFER_SIZE, stdin);
									sscanf(message, "%c", &option);
									
									// Confirmation for the switch
									if (option == 'y') {
										CardStore temp_trash;
										card_store_init(&temp_trash);
										int payment =
											aided
											? players[0].active->retreat
											: gravity
												? -players[0].active->retreat
												: 0
										;
										
										// The opponent is notified of the retreat
										messenger_send(&messenger, "retreat");
										
										// Energy is discarded
										while (payment < players[0].active->retreat) {
											display_energy(players[0].active);
											printf("\nIndex (-1 to cancel): ");
											fgets(message, BUFFER_SIZE, stdin);
											int index2;
											sscanf(message, "%d", &index2);
											
											// A valid index trashes the energy
											if (
												0 <= index2
												&& index2 < players[0].active->energy_count
											) {
												Energy* energy = pay(
													players[0].active,
													index2
												);
												
												store_energy(
													&temp_trash,
													energy
												);
												
												payment += energy->value;
												
												// The index of the energy removed is sent
												sprintf(message, "%d", index2);
												messenger_send(&messenger, message);
											}
											
											// An invalid index cancels the operation
											else {
												while (temp_trash.energy_count) {
													attach(
														players[0].active,
														get_energy(
															&temp_trash,
															temp_trash.energy_count - 1
														)
													);
												}
												
												// The opponent is notified of the retreat cancel
												sprintf(message, "%d", -1);
												messenger_send(&messenger, message);
												
												break;
											}
										}
										
										// If the energy was paid, the fighter retreats
										if (payment >= players[0].active->retreat) {
											Fighter* temp = players[0].active;
											players[0].active = players[0].bench[index];
											players[0].bench[index] = temp;
											printf("\nYour fighter retreated.\n");
											
											// The opponent is notified of a successful switch
											sprintf(message, "%d %d", -2, index);
											messenger_send(&messenger, message);
										}
										
										// The energy is moved to the trash
										while (temp_trash.energy_count) {
											rigged_draw(
												&temp_trash,
												store,
												temp_trash.energy_count - 1
											);
										}
									}
								}
							}
						}
					}
					
					// Invalid input
					else {
						printf("\nInvalid input.\n");
					}
					
					// Effects on benched fighters are cleared
					for (int i = 0; i < 2; i++) {
						for (int j = 0; j < BENCH_SIZE; j++) {
							if (players[i].bench[j]) {
								unaffect_fighter(players[i].bench[j], "");
							}
						}
					}

					// If a fighter with the "Aggressive" ability was placed on
					//   bench, it is switched in unless another fighter with
					//   the ability is in the active position
					for (int i = 0; i < 2; i++) {
						if (!effect_check(players[i].active, "aggro")) {
							for (int j = 0; j < BENCH_SIZE; j++) {
								if (players[i].bench[j]) {
									if (effect_check(players[i].bench[j], "aggro")) {
										Fighter* temp = players[i].active;
										players[i].active = players[i].bench[j];
										players[i].bench[j] = temp;
										unaffect_fighter(players[i].bench[j], "");
							
										printf(
											"\n%s forced its way into the active position!\n"
											"%s switched out.\n",
											players[i].active->name,
											players[i].bench[j]->name
										);
							
										break;
									}
								}
							}
						}
					}
					
					// Check for defeated fighters
					for (int i = 0; i < 2; i++) {
						bool banish = effect_search(players[i], "banish");
						
						// The bench is checked first to prevent invalid switch ins
						for (int j = 0; j < BENCH_SIZE; j++) {
							// A valid fighter
							if (players[i].bench[j]) {
								// Stamina allows a fighter to take more damage before defeat
								int stamina = effect_count_f(
									players[i].bench[j], "stamina"
								);
								
								if (players[i].bench[j]->health[0] <= -stamina) {
									bool lost = effect_check(
										players[i].bench[j], "lost"
									);
									
									// The "Banish" and "Lost" abilities banish cards
									CardStore* store =
										banish || lost
										? &the_void
										: &players[i].trash
									;
									
									// Energy is discarded
									while (players[i].bench[j]->energy_count) {
										store_energy(
											store,
											pay(
												players[i].bench[j],
												players[i].bench[j]->energy_count - 1
											)
										);
									}
									
									// The fighter is reset and discarded
									reset_fighter(players[i].bench[j]);
									store_fighter(store, players[i].bench[j]);
									
									// The defeat is announced
									printf(
										"\n%s was defeated.\n",
										players[i].bench[j]->name
									);
									
									// The fighter on the bench is removed
									players[i].bench[j] = NULL;
									
									// A random prize is drawn
									// The "Lost" ability also prevents a prize draw
									if (!lost && card_count(players[!i].prizes)) {
										int counts[3] = {
											players[!i].hand.fighter_count,
											players[!i].hand.supporter_count,
											players[!i].hand.energy_count
										};
										
										random_draw(&players[!i].prizes, &players[!i].hand);
										
										printf(
											"\n%s drew a prize.\n",
											i ? "You" : "Your opponent"
										);
										
										if (i) {
											if (counts[0] < players[!i].hand.fighter_count) {
												printf(
													"You drew %s.\n",
													players[!i].hand.fighters[counts[0]]->name
												);
											}
											
											else if (counts[1] < players[!i].hand.supporter_count) {
												printf(
													"You drew %s.\n",
													players[!i].hand.supporters[counts[1]]->name
												);
											}
											
											else {
												printf(
													"You drew %s.\n",
													players[!i].hand.energy[counts[2]]->name
												);
											}
										}
									}
								}
							}
						}
						
						// The active fighter is checked
						// Stamina allows a fighter to take more damage before defeat
						int stamina = effect_count_f(
							players[i].active, "stamina"
						);
						
						if (players[i].active->health[0] <= -stamina) {
							bool lost = effect_check(
								players[i].active, "lost"
							);
							
							bool redirect_void = effect_check(
								players[i].active, "redirect void"
							);
							
							// The "Banish" and "Lost" abilities banish cards
							CardStore* store =
								banish || lost || redirect_void
								? &the_void
								: &players[i].trash
							;
							
							// Energy is discarded
							while (players[i].active->energy_count) {
								store_energy(
									store,
									pay(
										players[i].active,
										players[i].active->energy_count - 1
									)
								);
							}
							
							// The fighter is reset and discarded
							reset_fighter(players[i].active);
							store_fighter(store, players[i].active);
							
							// The defeat is announced
							printf(
								"\n%s was defeated.\n",
								players[i].active->name
							);
							
							// The fighter on the bench is removed
							players[i].active = NULL;
							
							// A random prize is drawn
							// The "Lost" ability also prevents a prize draw
							if (!lost && card_count(players[!i].prizes)) {
								int counts[3] = {
									players[!i].hand.fighter_count,
									players[!i].hand.supporter_count,
									players[!i].hand.energy_count
								};
								
								random_draw(&players[!i].prizes, &players[!i].hand);
								
								printf(
									"\n%s drew a prize.\n",
									i ? "You" : "Your opponent"
								);
								
								if (i) {
									if (counts[0] < players[!i].hand.fighter_count) {
										printf(
											"You drew %s.\n",
											players[!i].hand.fighters[counts[0]]->name
										);
									}
									
									else if (counts[1] < players[!i].hand.supporter_count) {
										printf(
											"You drew %s.\n",
											players[!i].hand.supporters[counts[1]]->name
										);
									}
									
									else {
										printf(
											"You drew %s.\n",
											players[!i].hand.energy[counts[2]]->name
										);
									}
								}
							}
						}
					}
				
					// Checks for a winner
					// The player with more win conditions is the winner
					//   Prize check
					int win[2] = {
						!card_count(players[0].prizes),
						!card_count(players[1].prizes)
					};
					
					int empty[2] = {1, 1};
					
					for (int i = 0; i < 2; i++) {
						for (int j = 0; j < BENCH_SIZE; j++) {
							if (players[i].bench[j]) {
								empty[i] = 0;
								break;
							}
						}
						
						if (players[i].active) {
							empty[i] = 0;
						}
						
						win[!i] += 2 * empty[i];
					}
					
					if (win[0] & 1) {
						printf("\nYou drew all of your prizes!\n");
					}
					
					if (win[0] & 2) {
						printf(
							"%sYou defeated all of your opponent's fighters!\n",
							win[0] & 1 ? "" : "\n"
						);
					}
					
					if (win[1] & 1) {
						printf("\nYour opponent drew all of their prizes...\n");
					}
					
					if (win[1] & 2) {
						printf(
							"%sYour opponent defeated all of your fighters...\n",
							win[1] & 1 ? "" : "\n"
						);
					}
					
					// Win condition count for tiebreaker
					if (win[0] && win[1]) {
						if (win[0] == 3 && win[1] != 3) {
							printf("\nYou win!\n");
						}
						
						else if (win[0] != 3 && win[1] == 3) {
							printf("\nYou lose...\n");
						}
						
						else {
							printf("\nIt's a tie.\n");
						}
						
						end = true;
						break;
					}
					
					else if (win[0]) {
						printf("\nYou win!\n");
						end = true;
						break;
					}
					
					else if (win[1]) {
						printf("\nYou lose...\n");
						end = true;
						break;
					}
					
					// If the game didn't end, new active fighters are sent
					//   out if they were defeated
					if (!players[0].active) {
						int index;
						
						while (true) {
							printf("\nSend out a fighter:\n");
							
							for (int i = 0; i < BENCH_SIZE; i++) {
								if (players[0].bench[i]) {
									printf(
										"%s (%d)\n",
										players[0].bench[i]->name,
										i
									);
								}
							}
						
							printf("\nIndex (-1 to cancel): ");
							fgets(message, BUFFER_SIZE, stdin);
							sscanf(message, "%d", &index);
							
							if (0 <= index && index < BENCH_SIZE) {
								if (players[0].bench[index]) {
									print_fighter(players[0].bench[index]);
									display_energy(players[0].bench[index]);
									printf("\nSend out this fighter (y/n)? ");
									fgets(message, BUFFER_SIZE, stdin);
									char answer;
									sscanf(message, "%c", &answer);
									
									if (answer == 'y') {
										break;
									}
								}
								
								else {
									printf("\nInvalid index.\n");
								}
							}
							
							else {
								printf("\nInvalid index.\n");
							}
						}
						
						players[0].active = players[0].bench[index];
						players[0].bench[index] = NULL;
						
						printf(
							"\nYou sent out %s.\n",
							players[0].active->name
						);
						
						sprintf(message, "%d", index);
						messenger_send(&messenger, message);
					}
					
					if (!players[1].active) {
						messenger_read(&messenger, message, BUFFER_SIZE);
						int index;
						sscanf(message, "%d", &index);
						players[1].active = players[1].bench[index];
						players[1].bench[index] = NULL;
						
						printf(
							"\nYour opponent sent out %s.\n",
							players[1].active->name
						);
					}
				}
			}
			
			// The opponent's turn
			else {
				// If the opponent cannot draw a card at the start of their turn, the player wins
				if (!card_count(players[1].deck)) {
					printf(
						"\nYour opponent's deck is empty and they cannot draw a card.\n"
						"You win!\n"
					);
					end = true;
					break;
				}
				
				// The opponent starts their turn off by drawing a card
				printf(
					"\nIt is your opponent's turn.\nYour opponent drew a card.\n"
				);
				random_draw(&players[1].deck, &players[1].hand);
				
				bool attacked = false;
				
				// The opponent makes their choices and they are relayed
				while (!attacked) {
					messenger_read(&messenger, message, BUFFER_SIZE);
					char word[20];
					sscanf(message, "%s", word);
					
					// If the opponent ended their turn, the player's turn begins
					if (!strcmp(word, "done")) {
						break;
					}
					
					// If the opponent conceded, the player wins
					else if (!strcmp(word, "quit")) {
						printf("\nYour opponent conceded.\nYou win!\n");
						end = true;
						break;
					}
					
					// Opponent attempted to play cards from hand
					else if (!strcmp(word, "play")) {
						int index;
						int code;
						sscanf(message, "%*s%s%d%d", word, &index, &code);
						
						// Fighter
						if (!strcmp(word, "fighter")) {
							Fighter* fighter = get_fighter(&players[1].hand, index);
							
							// Operation cancelled
							if (code == -1) {
								store_fighter(&players[1].hand, fighter);
							}
							
							// Active fighter ranked up
							else if (!code) {
								printf(
									"\n%s was ranked up into %s.\n",
									players[1].active->name,
									fighter->name
								);
								
								store_fighter(
									&players[1].trash,
									rank_up(
										&players[1].active,
										fighter
									)
								);
							}
							
							// Fighter placed on the bench
							else {
								// Rank up
								if (fighter->rank) {
									printf(
										"\n%s was ranked up into %s.\n",
										players[1].bench[code - 1]->name,
										fighter->name
									);
									
									store_fighter(
										&players[1].trash,
										rank_up(
											players[1].bench + code - 1,
											fighter
										)
									);
								}
								
								// Placed on bench
								else {
									players[1].bench[code - 1] = fighter;
									printf("\n%s was placed on the bench.\n", fighter->name);
								}
							}
						}
						
						// Supporter
						else if (!strcmp(word, "supporter")) {
							Supporter* supporter = get_supporter(&players[1].hand, index);
							
							// Operation cancelled
							if (code == -1) {
								store_supporter(&players[1].hand, supporter);
							}
							
							else {
								bool played = play_test(code, supporter->name);
								
								if (played) {
									printf("\nYour opponent played %s.\n", supporter->name);
								}
								
								char buffer[50];
								int effect_index = 0;
								bool end2 = false;
								int choice_count = 0;
								bool cancel = false;
								// Discarded cards are banished if required
								CardStore* stores[2] = {
									effect_search(players[0], "banish")
									? &the_void
									: &players[0].trash,
									effect_search(players[1], "banish")
									? &the_void
									: &players[1].trash
								};
								char flips[10];
								CardStore payment;
								card_store_init(&payment);
								Fighter* payer = NULL;
								
								// The effects of the supporter are managed
								while (!end2 && !cancel) {
									// The next effect is extracted
									for (int i = 0;; i++, effect_index++) {
										buffer[i] = supporter->effects[effect_index];
										
										// Not the last effect
										if (buffer[i] == '\n') {
											effect_index++;
											break;
										}
										
										// The last effect
										else if (buffer[i] == '\0') {
											end2 = true;
											break;
										}
									}
									
									// The effect type is found
									char word[20];
									sscanf(buffer, "%s", word);
									
									// Draws cards
									if (!strcmp(word, "draw")) {
										int draw;
										sscanf(buffer, "%*s%d", &draw);
										
										for (int i = 0; i < draw; i++) {
											// The players can't draw cards if the deck is empty
											if (!card_count(players[1].deck)) {
												draw = i;
												break;
											}
											
											random_draw(&players[1].deck, &players[1].hand);
										}
										
										printf(
											"Your opponent drew %d card%s.\n",
											draw,
											draw == 1
											? ""
											: "s"
										);
									}

									// Discards cards
									else if (!strcmp(word, "discard")) {
										sscanf(buffer, "%*s%s", word);
										
										// Discards all of the cards in hand
										if (!strcmp(word, "all")) {
											while (card_count(players[1].hand)) {
												rigged_draw(
													&players[1].hand,
													stores[1],
													card_count(players[1].hand) - 1
												);
											}
										
											printf("Your opponent discarded their hand.\n");
										}
										
										else {
											int count;
											sscanf(word, "%d", &count);
											CardStore temp_trash;
											card_store_init(&temp_trash);
											
											// Cards are chosen to be discarded from hand
											while (count && !cancel) {
												index = code >> choice_count++ * CHOICE_SHIFT & ERROR_INDEX;
												
												// Valid index
												if (0 <= index && index < card_count(players[1].hand)) {
													rigged_draw(&players[1].hand, &temp_trash, index);
													count--;
												}
												
												// Invalid index returns cards to hand and returns the error index
												else {
													while (card_count(temp_trash)) {
														rigged_draw(
															&temp_trash,
															&players[1].hand,
															card_count(temp_trash) - 1
														);
													}
													
													cancel = true;
												}
											}
											
											// Cards are discarded
											while (card_count(temp_trash)) {
												int counts[3] = {
													stores[1]->fighter_count,
													stores[1]->supporter_count,
													stores[1]->energy_count
												};
												
												rigged_draw(
													&temp_trash,
													stores[1],
													card_count(temp_trash) - 1
												);
												
												if (counts[0] < stores[1]->fighter_count) {
													printf(
														"Your opponent discarded %s.\n",
														stores[1]->fighters[counts[0]]->name
													);
												}
												
												else if (counts[1] < stores[1]->supporter_count) {
													printf(
														"Your opponent discarded %s.\n",
														stores[1]->supporters[counts[1]]->name
													);
												}
												
												else {
													printf(
														"Your opponent discarded %s.\n",
														stores[1]->energy[counts[2]]->name
													);
												}
											}
										}
									}
									
									// Forces a benched fighter out
									else if (!strcmp(word, "gust")) {
										Fighter* temp = players[0].active;
										players[0].active = players[0].bench[code];
										players[0].bench[code] = temp;
										
										printf(
											"%s was forced out from the bench.\n"
											"%s returned to the bench.\n",
											players[1].active->name,
											temp->name
										);
									}
									
									// Heals a fighter
									else if (!strcmp(word, "heal")) {
										int value;
										sscanf(buffer, "%*s%d", &value);
										
										if (!code) {
											players[1].active->health[0] += value;
											
											if (players[1].active->health[0] > players[1].active->health[1]) {
												value -=
													players[1].active->health[0]
													- players[1].active->health[1]
												;
												players[1].active->health[0] = players[1].active->health[1];
											}
											
											printf(
												"%s was healed %d damage.\n",
												players[1].active->name,
												value
											);
										}
										
										else {
											players[1].bench[code - 1]->health[0] += value;
											
											if (
												players[1].bench[code - 1]->health[0]
												> players[1].bench[code- 1]->health[1]
											) {
												value -=
													players[1].bench[code - 1]->health[0]
													- players[1].bench[code - 1]->health[1]
												;
												players[1].bench[code - 1]->health[0] =
													players[1].bench[code - 1]->health[1]
												;
											}
											
											printf(
												"%s was healed %d damage.\n",
												players[1].bench[code - 1]->name,
												value
											);
										}
									}
									
									// Heals a fighter
									// Heals more if energy is dicarded from the fighter
									else if (!strcmp(word, "pay_heal")) {
										int values[2];
										sscanf(buffer, "%*s%d%d", values, values + 1);
										index = code & ERROR_INDEX;
										int index2 = code >> CHOICE_SHIFT & ERROR_INDEX;
										index2 = index2 == ERROR_INDEX ? -1 : index2;
										
										// Active fighter
										if (!index) {
											// No energy discarded and less healing
											if (index2 == -1) {
												players[1].active->health[0] += values[0];
												
												if (players[1].active->health[0] > players[1].active->health[1]) {
													values[0] -=
														players[1].active->health[0]
														- players[1].active->health[1]
													;
													players[1].active->health[0] = players[1].active->health[1];
												}
												
												printf(
													"%s was healed %d damage.\n",
													players[1].active->name,
													values[0]
												);
											}
												
											// Energy discarded and more healing
											else if (0 <= index2 && index2 < players[1].active->energy_count) {
												store_energy(
													stores[1],
													pay(
														players[1].active,
														index2
													)
												);
												
												printf(
													"Your opponent discarded %s.\n",
													stores[1]->energy[
														stores[1]->energy_count - 1
													]->name
												);
												
												players[1].active->health[0] += values[1];
												
												if (players[1].active->health[0] > players[1].active->health[1]) {
													values[1] -=
														players[1].active->health[0]
														- players[1].active->health[1]
													;
													
													players[1].active->health[0] = players[1].active->health[1];
												}
												
												printf(
													"%s was healed %d damage.\n",
													players[1].active->name,
													values[1]
												);
											}
										}
										
										// Benched fighter
										else if (0 < index && index <= BENCH_SIZE) {
											// No energy discarded, but less energy
											if (index2 == -1) {
												players[1].bench[index - 1]->health[0] += values[0];
												
												if (players[1].bench[index - 1]->health[0] > players[1].bench[index - 1]->health[1]) {
													values[0] -=
														players[1].bench[index - 1]->health[0]
														- players[1].bench[index - 1]->health[1]
													;
													players[1].bench[index - 1]->health[0] = players[1].bench[index - 1]->health[1];
												}
												
												printf(
													"%s was healed %d damage.\n",
													players[1].bench[index - 1]->name,
													values[0]
												);
											}
											
											// Energy discarded and more healing
											else if (0 <= index2 && index2 < players[1].bench[index - 1]->energy_count) {
												store_energy(
													stores[1],
													pay(
														players[1].active,
														index2
													)
												);
												
												printf(
													"Your opponent discarded %s.\n",
													stores[1]->energy[
														stores[1]->energy_count - 1
													]->name
												);
												
												players[1].bench[index - 1]->health[0] += values[1];
												
												if (players[1].bench[index - 1]->health[0] > players[1].bench[index - 1]->health[1]) {
													values[1] -=
														players[1].bench[index - 1]->health[0]
														- players[1].bench[index - 1]->health[1]
													;
													players[1].bench[index - 1]->health[0] = players[1].bench[index - 1]->health[1];
												}
												
												printf(
													"%s was healed %d damage.\n",
													players[1].bench[index - 1]->name,
													values[1]
												);
											}
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
										affect_player(players + 1, power_effect);
										
										printf(
											"Your opponent's attacks deal %d more damage this turn.\n",
											power
										);
									}
								
									// Fully heals all fighters and removes all effects and energy from them
									else if (!strcmp(word, "hospital")) {
										// Effects are removed from the active fighter
										unaffect_fighter(players[1].active, "");
										
										// Energy is removed from the active fighter
										while (players[1].active->energy_count) {
											store_energy(
												stores[1],
												pay(
													players[1].active,
													players[1].active->energy_count - 1
												)
											);
										}
										
										// The active fighter is fully healed
										players[1].active->health[0] = players[1].active->health[1];
										
										// Benched fighters
										for (int i = 0; i < BENCH_SIZE; i++) {
											if (players[1].bench[i]) {
												// Energy is removed from the fighter
												while (players[1].bench[i]->energy_count) {
													store_energy(
														stores[1],
														pay(
															players[1].bench[i],
															players[1].bench[i]->energy_count - 1
														)
													);
												}
												
												// The fighter is fully healed
												players[1].bench[i]->health[0] = players[1].bench[i]->health[1];
											}
										}
										
										printf(
											"Your opponent's fighters were fully healed and "
											"had all energy discarded from "
											"them. Your opponent's active fighter's "
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
												if (!card_count(players[1].deck)) {
													draw = i;
													break;
												}
												
												random_draw(&players[1].deck, &players[1].hand);
											}
											
											printf(
												"Your opponent drew %d card%s.\n",
												draw,
												draw == 1
												? ""
												: "s"
											);
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
												if (!card_count(players[1].deck)) {
													draw = i;
													break;
												}
												
												random_draw(&players[1].deck, &players[1].hand);
											}
											
											printf(
												"Your opponent drew %d card%s.\n",
												draw,
												draw == 1
												? ""
												: "s"
											);
										}
									}

									// Shuffles cards into the deck
									else if (!strcmp(word, "shuffle")) {
										sscanf(buffer, "%*s%s", word);
										
										// Shuffles the entire hand into the deck
										if (!strcmp(word, "all")) {
											while (card_count(players[1].hand)) {
												rigged_draw(
													&players[1].hand,
													&players[1].deck,
													card_count(players[1].hand) - 1
												);
											}
											
											printf("Your opponent shuffled their hand into their deck.\n");
										}
									}

									// Discards energy attached to a fighter
									else if (!strcmp(word, "drain")) {
										int drain;
										sscanf(buffer, "%*s%d", &drain);
										
										Fighter* fighter = NULL;
										
										index = code >> choice_count++ * CHOICE_SHIFT & ERROR_INDEX;
										
										if (!index) {
											fighter = players[0].active;
										}
										
										else if (0 < index && index <= BENCH_SIZE) {
											fighter = players[0].bench[index - 1];
										}
										
										if (fighter) {
											CardStore temp_trash;
											card_store_init(&temp_trash);
											
											for (int i = 0; i < drain && fighter->energy_count; i++) {
												index = code >> choice_count++ * CHOICE_SHIFT & ERROR_INDEX;
												
												if (0 <= index && index < fighter->energy_count) {
													store_energy(
														&temp_trash,
														pay(
															fighter,
															index
														)
													);
												}
												
												else {
													cancel = true;
													break;
												}
											}
											
											// Energy is refunded to both fighters if cancelled and applicable
											if (cancel) {
												// Energy is returned to the player's fighter
												while (temp_trash.energy_count) {
													attach(
														fighter,
														get_energy(
															&temp_trash,
															temp_trash.energy_count - 1
														)
													);
												}
											
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
											}
											
											// Energy is drained
											else {
												while (temp_trash.energy_count) {
													rigged_draw(
														&temp_trash,
														stores[0],
														temp_trash.energy_count - 1
													);
												}
												
												printf(
													"Energy was drained from %s.\n",
													fighter->name
												);
											}
										}
										
										// Cancel
										else {
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
											
											cancel = true;
										}
									}

									// Discards energy cards attached to the player's fighter
									else if (!strcmp(word, "pay")) {
										int price;
										sscanf(buffer, "%*s%d", &price);
										
										index = code >> choice_count++ * CHOICE_SHIFT & ERROR_INDEX;
										
										if (!index) {
											payer = players[1].active;
										}
										
										else if (0 < index && index <= BENCH_SIZE && players[0].bench[index - 1]) {
											payer = players[1].bench[index - 1];
										}
										
										CardStore temp_trash;
										card_store_init(&temp_trash);
										
										for (int i = 0; i < price; i++) {
											index = code >> choice_count++ * CHOICE_SHIFT & ERROR_INDEX;
											
											if (0 <= index && index < payer->energy_count) {
												store_energy(
													&temp_trash,
													pay(
														payer,
														index
													)
												);
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
											
											if (played) {
												printf(
													"Your opponent paid with energy from %s.\n",
													payer->name
												);
											}
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
												for (i = 0; i < count && players[1].trash.energy_count; i++) {
													index = code >> choice_count++ * CHOICE_SHIFT & ERROR_INDEX;
													
													rigged_draw(
														&players[1].trash,
														&players[1].hand,
														index
													);
												}
												
												printf(
													"Your opponent drew %d card%s.\n",
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
											
											// Searches the deck for cards as long as it is not empty
											for (i = 0; i < count && card_count(players[1].deck); i++) {
												index = code >> choice_count++ * CHOICE_SHIFT & ERROR_INDEX;
												
												rigged_draw(
													&players[1].deck,
													&players[1].hand,
													index
												);
											}
											
											printf(
												"Your opponent drew %d card%s.\n",
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
											
											for (int i = 0; i < players[1].deck.fighter_count && !playable; i++) {
												if (players[1].deck.fighters[i]->rank) {
													if (
														!strcmp(
															players[1].deck.fighters[i]->old_rank,
															players[1].active->name
														)
													) {
														playable = true;
													}
													
													for (int j = 0; j < BENCH_SIZE && !playable; j++) {
														if (players[1].bench[j]) {
															if (
																!strcmp(
																	players[1].deck.fighters[i]->old_rank,
																	players[1].bench[j]->name
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
												for (i = 0; i < count && players[1].deck.fighter_count; i++) {
													index = code >> choice_count++ * CHOICE_SHIFT & ERROR_INDEX;
													
													Fighter* new_form = get_fighter(
														&players[1].deck,
														index
													);
													
													index = code >> choice_count++ * CHOICE_SHIFT & ERROR_INDEX;
													
													Fighter** old_form = NULL;
														
													if (!index) {
														old_form = &players[1].active;
													}
													
													else {
														old_form = &players[1].bench[index - 1];
													}
														
													printf(
														"%s was ranked up into %s.\n",
														(*old_form)->name,
														new_form->name
													);
														
													store_fighter(
														stores[1],
														rank_up(
															old_form,
															new_form
														)
													);
												}
											}
											
											// Else, no fighter is gotten and the card's functionality ceases
											// The player is not refunded
											else {
												end2 = true;
												printf("There were no valid cards to draw...\n");
											}
										}
										
										// Searches the deck for any card
										else {
											int count;
											sscanf(buffer, "%*s%d", &count);
											int i;
											
											// Searches the deck for cards as long as it is not empty
											for (i = 0; i < count && card_count(players[1].deck); i++) {
												index = code >> choice_count++ * CHOICE_SHIFT & ERROR_INDEX;
												
												rigged_draw(
													&players[1].deck,
													&players[1].hand,
													index
												);
											}
											
											printf(
												"Your opponent drew %d card%s.\n",
												i,
												i == 1 ? "" : "s"
											);
										}
									}
								}
								
								if (cancel) {
									store_supporter(&players[1].hand, supporter);
								}
								
								else {
									// If the effects were paid for, the payment is taken
									while (payment.energy_count) {
										rigged_draw(
											&payment,
											stores[1],
											payment.energy_count - 1
										);
									}
									
									store_supporter(stores[1], supporter);
								}
							}
						}
						
						// Energy
						else {
							Energy* energy = get_energy(&players[1].hand, index);
							
							// Operation cancelled
							if (code == -1) {
								store_energy(&players[1].hand, energy);
							}
							
							// Attached to active
							else if (!code) {
								attach(players[1].active, energy);
								printf(
									"\n%s was attached to %s.\n",
									energy->name,
									players[1].active->name
								);
							}
							
							// Attached to bench
							else {
								attach(players[1].bench[code - 1], energy);
								printf(
									"\n%s was attached to %s.\n",
									energy->name,
									players[1].bench[code - 1]->name
								);
							}
						}
					}
					
					// Opponent attempted to retreat
					else if (!strcmp(word, "retreat")) {
						// The ability, "Banish", banishes discarded cards
						CardStore* store =
							effect_search(players[1], "banish")
							? &the_void
							: &players[1].trash
						;
						CardStore temp_trash;
						card_store_init(&temp_trash);
						
						while (true) {
							messenger_read(&messenger, message, BUFFER_SIZE);
							int index;
							sscanf(message, "%d", &index);
							
							// The retreat is cancelled and the energy is refunded
							if (index == -1) {
								while (temp_trash.energy_count) {
									attach(
										players[1].active,
										get_energy(
											&temp_trash,
											temp_trash.energy_count - 1
										)
									);
								}
								
								break;
							}
							
							// The retreat is performed
							else if (index == -2) {
								sscanf(message, "%*d%d", &index);
								
								Fighter* temp = players[1].active;
								players[1].active = players[1].bench[index];
								players[1].bench[index] = temp;
								printf(
									"\n%s retreated to the bench.\n"
									"%s was sent out.\n",
									temp->name,
									players[1].active->name
								);
								
								break;
							}
							
							// The energy is moved to the buffer
							else {
								store_energy(
									&temp_trash,
									pay(players[1].active, index)
								);
							}
						}
						
						// The energy is moved to the trash
						while (temp_trash.energy_count) {
							rigged_draw(
								&temp_trash,
								store,
								temp_trash.energy_count - 1
							);
						}
					}
					
					// Opponent attacked
					else if (!strcmp(word, "attack")) {
						int index;
						sscanf(message, "%*s%d", &index);
						attack(
							players + 1, players + 0, index,
							&the_void, &messenger
						);
						attacked = true;
					}
				
					// Opponent used an ability
					else if (!strcmp(word, "ability")) {
						int index;
						sscanf(message, "%*s%d", &index);
						ability(
							players + 1, players, index,
							&the_void, &messenger
						);
					}
					
					// Effects on benched fighters are cleared
					for (int i = 0; i < 2; i++) {
						for (int j = 0; j < BENCH_SIZE; j++) {
							if (players[i].bench[j]) {
								unaffect_fighter(players[i].bench[j], "");
							}
						}
					}

					// If a fighter with the "Aggressive" ability was placed on
					//   bench, it is switched in unless another fighter with
					//   the ability is in the active position
					for (int i = 0; i < 2; i++) {
						if (!effect_check(players[i].active, "aggro")) {
							for (int j = 0; j < BENCH_SIZE; j++) {
								if (players[i].bench[j]) {
									if (effect_check(players[i].bench[j], "aggro")) {
										Fighter* temp = players[i].active;
										players[i].active = players[i].bench[j];
										players[i].bench[j] = temp;
										unaffect_fighter(players[i].bench[j], "");
							
										printf(
											"\n%s forced its way into the active position!\n"
											"%s switched out.\n",
											players[i].active->name,
											players[i].bench[j]->name
										);
							
										break;
									}
								}
							}
						}
					}
					
					// Check for defeated fighters
					for (int i = 0; i < 2; i++) {
						bool banish = effect_search(players[i], "banish");
						
						// The bench is checked first to prevent invalid switch ins
						for (int j = 0; j < BENCH_SIZE; j++) {
							// A valid fighter
							if (players[i].bench[j]) {
								// Stamina allows a fighter to take more damage before defeat
								int stamina = effect_count_f(
									players[i].bench[j], "stamina"
								);
								
								if (players[i].bench[j]->health[0] <= -stamina) {
									bool lost = effect_check(
										players[i].bench[j], "lost"
									);
									
									// The "Banish" and "Lost" abilities banish cards
									CardStore* store =
										banish || lost
										? &the_void
										: &players[i].trash
									;
									
									// Energy is discarded
									while (players[i].bench[j]->energy_count) {
										store_energy(
											store,
											pay(
												players[i].bench[j],
												players[i].bench[j]->energy_count - 1
											)
										);
									}
									
									// The fighter is reset and discarded
									reset_fighter(players[i].bench[j]);
									store_fighter(store, players[i].bench[j]);
									
									// The defeat is announced
									printf(
										"\n%s was defeated.\n",
										players[i].bench[j]->name
									);
									
									// The fighter on the bench is removed
									players[i].bench[j] = NULL;
									
									// A random prize is drawn
									// The "Lost" ability also prevents a prize draw
									if (!lost && card_count(players[!i].prizes)) {
										int counts[3] = {
											players[!i].hand.fighter_count,
											players[!i].hand.supporter_count,
											players[!i].hand.energy_count
										};
										
										random_draw(&players[!i].prizes, &players[!i].hand);
										
										printf(
											"\n%s drew a prize.\n",
											i ? "You" : "Your opponent"
										);
										
										if (i) {
											if (counts[0] < players[!i].hand.fighter_count) {
												printf(
													"You drew %s.\n",
													players[!i].hand.fighters[counts[0]]->name
												);
											}
											
											else if (counts[1] < players[!i].hand.supporter_count) {
												printf(
													"You drew %s.\n",
													players[!i].hand.supporters[counts[1]]->name
												);
											}
											
											else {
												printf(
													"You drew %s.\n",
													players[!i].hand.energy[counts[2]]->name
												);
											}
										}
									}
								}
							}
						}
						
						// The active fighter is checked
						// Stamina allows a fighter to take more damage before defeat
						int stamina = effect_count_f(
							players[i].active, "stamina"
						);
						
						if (players[i].active->health[0] <= -stamina) {
							bool lost = effect_check(
								players[i].active, "lost"
							);
							
							bool redirect_void = effect_check(
								players[i].active, "redirect void"
							);
							
							// The "Banish" and "Lost" abilities banish cards
							CardStore* store =
								banish || lost || redirect_void
								? &the_void
								: &players[i].trash
							;
							
							// Energy is discarded
							while (players[i].active->energy_count) {
								store_energy(
									store,
									pay(
										players[i].active,
										players[i].active->energy_count - 1
									)
								);
							}
							
							// The fighter is reset and discarded
							reset_fighter(players[i].active);
							store_fighter(store, players[i].active);
							
							// The defeat is announced
							printf(
								"\n%s was defeated.\n",
								players[i].active->name
							);
							
							// The fighter on the bench is removed
							players[i].active = NULL;
							
							// A random prize is drawn
							// The "Lost" ability also prevents a prize draw
							if (!lost && card_count(players[!i].prizes)) {
								int counts[3] = {
									players[!i].hand.fighter_count,
									players[!i].hand.supporter_count,
									players[!i].hand.energy_count
								};
								
								random_draw(&players[!i].prizes, &players[!i].hand);
								
								printf(
									"\n%s drew a prize.\n",
									i ? "You" : "Your opponent"
								);
								
								if (i) {
									if (counts[0] < players[!i].hand.fighter_count) {
										printf(
											"You drew %s.\n",
											players[!i].hand.fighters[counts[0]]->name
										);
									}
									
									else if (counts[1] < players[!i].hand.supporter_count) {
										printf(
											"You drew %s.\n",
											players[!i].hand.supporters[counts[1]]->name
										);
									}
									
									else {
										printf(
											"You drew %s.\n",
											players[!i].hand.energy[counts[2]]->name
										);
									}
								}
							}
						}
					}
				
					// Checks for a winner
					// The player with more win conditions is the winner
					//   Prize check
					int win[2] = {
						!card_count(players[0].prizes),
						!card_count(players[1].prizes)
					};
					
					int empty[2] = {1, 1};
					
					for (int i = 0; i < 2; i++) {
						for (int j = 0; j < BENCH_SIZE; j++) {
							if (players[i].bench[j]) {
								empty[i] = 0;
							}
						}
						
						if (players[i].active) {
							empty[i] = 0;
						}
						
						win[!i] += 2 * empty[i];
					}
					
					if (win[0] & 1) {
						printf("\nYou drew all of your prizes!\n");
					}
					
					if (win[0] & 2) {
						printf(
							"%sYou defeated all of your opponent's fighters!\n",
							win[0] & 1 ? "" : "\n"
						);
					}
					
					if (win[1] & 1) {
						printf("\nYour opponent drew all of their prizes...\n");
					}
					
					if (win[1] & 2) {
						printf(
							"%sYour opponent defeated all of your fighters...\n",
							win[1] & 1 ? "" : "\n"
						);
					}
					
					// Win condition count for tiebreaker
					if (win[0] && win[1]) {
						if (win[0] == 3 && win[1] != 3) {
							printf("\nYou win!\n");
						}
						
						else if (win[0] != 3 && win[1] == 3) {
							printf("\nYou lose...\n");
						}
						
						else {
							printf("\nIt's a tie.\n");
						}
						
						end = true;
						break;
					}
					
					else if (win[0]) {
						printf("\nYou win!\n");
						end = true;
						break;
					}
					
					else if (win[1]) {
						printf("\nYou lose...\n");
						end = true;
						break;
					}
					
					// If the game didn't end, new active fighters are sent
					//   out if they were defeated
					if (!players[1].active) {
						messenger_read(&messenger, message, BUFFER_SIZE);
						int index;
						sscanf(message, "%d", &index);
						players[1].active = players[1].bench[index];
						players[1].bench[index] = NULL;
						
						printf(
							"\nYour opponent sent out %s.\n",
							players[1].active->name
						);
					}
					
					if (!players[0].active) {
						int index;
						
						while (true) {
							printf("\nSend out a fighter:\n");
							
							for (int i = 0; i < BENCH_SIZE; i++) {
								if (players[0].bench[i]) {
									printf(
										"%s (%d)\n",
										players[0].bench[i]->name,
										i
									);
								}
							}
						
							printf("\nIndex (-1 to cancel): ");
							fgets(message, BUFFER_SIZE, stdin);
							sscanf(message, "%d", &index);
							
							if (0 <= index && index < BENCH_SIZE) {
								if (players[0].bench[index]) {
									print_fighter(players[0].bench[index]);
									display_energy(players[0].bench[index]);
									printf("\nSend out this fighter (y/n)? ");
									fgets(message, BUFFER_SIZE, stdin);
									char answer;
									sscanf(message, "%c", &answer);
									
									if (answer == 'y') {
										break;
									}
								}
								
								else {
									printf("\nInvalid index.\n");
								}
							}
							
							else {
								printf("\nInvalid index.\n");
							}
						}
						
						players[0].active = players[0].bench[index];
						players[0].bench[index] = NULL;
						
						printf(
							"\nYou sent out %s.\n",
							players[0].active->name
						);
						
						sprintf(message, "%d", index);
						messenger_send(&messenger, message);
					}
				}
			}
			
			// End of turn effects don't occur if the game ends
			if (!end) {
				// End of turn effects
				//   Fighters played can be ranked up next turn
				//   Fighters have their ability uses reset
				players[turn].active->first_turn = false;
				players[turn].active->uses = 0;
				
				for (int i = 0; i < BENCH_SIZE; i++) {
					if (players[turn].bench[i]) {
						players[turn].bench[i]->first_turn = false;
						players[turn].bench[i]->uses = 0;
					}
				}
				
				// The player and their fighters lose their power boosts
				unaffect_player(players + turn, "power");
				unaffect_fighter(players[turn].active, "power");
				
				// The effect of increased gravity is removed
				unaffect_fighter(players[turn].active, "gravity");
				
				// Invincibility and immunity are removed at the end
				//   of the opponent's turn
				unaffect_fighter(players[!turn].active, "invincible");
				unaffect_fighter(players[!turn].active, "immune");
				
				// Redirection of the defending fighter upon defeat is removed
				unaffect_fighter(players[!turn].active, "redirect");
				
				// The player's fighters are healed at the end of the
				//   turn with the "Healing Aura" ability
				int healing = effect_count(players[turn], "end_heal");
				
				players[turn].active->health[0] += healing;
				
				if (players[turn].active->health[0] > players[turn].active->health[1]) {
					healing -=
						players[turn].active->health[0]
						- players[turn].active->health[1]
					;
					players[turn].active->health[0] = players[turn].active->health[1];
				}
				
				printf(
					"\n%s was healed %d damage with "
					"the abilities of your%s fighters.\n",
					players[turn].active->name,
					healing,
					turn ? " opponent's" : ""
				);
				
				// The turn switches over
				turn = 1 - turn;
			}
		}
		
		// Dynamically allocated memory is freed
		free_players(players);
		free_store(&the_void);
		
		printf("\nPlay again (y/n)? ");
		fgets(message, BUFFER_SIZE, stdin);
		messenger_send(&messenger, message);
		char answer;
		sscanf(message, "%c", &answer);
		messenger_read(&messenger, message, BUFFER_SIZE);
		
		if (answer == 'n') {
			break;
		}
		
		sscanf(message, "%c", &answer);
		
		if (answer == 'n') {
			printf("\nYour opponent left...\n");
			break;
		}
	}
	
	// The server and SDL are shut down
	server_quit(&server);
	sdlnet_quit();
	
	return 0;
}