#ifndef PIT_OF_TRIALS_H
#define PIT_OF_TRIALS_H

#include "port/ui/cvar_prefixes.h"

#define CVAR_PIT_START_FLOOR CVAR_ENHANCEMENT("PitStartFloor")
// The deepest floor Mario has reached. Kept in the config, so it is shared by all save files.
#define CVAR_PIT_BEST_FLOOR CVAR_ENHANCEMENT("PitBestFloor")
// Chance (0-100) that a rest floor has Rip Cheato's shop, and that a floor (1-90) has the Mover. Set with the Pit menu sliders.
#define CVAR_PIT_SHOP_CHANCE CVAR_ENHANCEMENT("PitShopChance")
#define CVAR_PIT_MOVER_CHANCE CVAR_ENHANCEMENT("PitMoverChance")
#define PIT_SHOP_DEFAULT_CHANCE 33
#define PIT_MOVER_DEFAULT_CHANCE 3
// The Mover's chance can not be set higher than this (the slider stops here, and a higher saved value is cut down).
#define PIT_MOVER_MAX_CHANCE 10

#ifdef __cplusplus
extern "C" {
#endif

// Asks the game thread to jump to the Pit test room. Safe to call from the menu.
void pit_request_enter(void);

// Moves on to the next floor (called from the room script when Mario goes down the pipe).
void pit_request_next_floor(void);

// The Mover: moves on, skipping this many floors (the floor after them is where Mario lands, at most floor 99).
void pit_request_skip_floors(int count);

// The floor Mario is on (1-100).
int pit_current_floor(void);

// The deepest floor reached so far (0 if none).
int pit_best_floor(void);

// Remembers that Mario reached this floor, if it is deeper than the best so far.
void pit_record_floor(int floor);

// How the first enemy of a battle looks in the overworld: 16 animation IDs (in the order of NpcData::animations)
// and the collision height, radius and level. Returns 0 if the battle has no usable enemy.
int pit_enemy_look(int battleID, int* anims, int* height, int* radius, int* level);

// Picks a battle ID for the current floor.
int pit_pick_battle(void);

// Called once per game frame. Starts the map change if a request is waiting.
void pit_poll(void);

#ifdef __cplusplus
}
#endif

#endif
