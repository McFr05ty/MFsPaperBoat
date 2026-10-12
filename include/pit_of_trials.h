#ifndef PIT_OF_TRIALS_H
#define PIT_OF_TRIALS_H

#include "port/ui/cvar_prefixes.h"

#define CVAR_PIT_START_FLOOR CVAR_ENHANCEMENT("PitStartFloor")
// The deepest floor Mario has reached. Kept in the config, so it is shared by all save files.
#define CVAR_PIT_BEST_FLOOR CVAR_ENHANCEMENT("PitBestFloor")
// Chance (0-100) that a rest floor has Rip Cheato's shop, and that a floor (1-90) has the Mover. Set with the Pit menu sliders.
#define CVAR_PIT_SHOP_CHANCE CVAR_ENHANCEMENT("PitShopChance")
#define CVAR_PIT_MOVER_CHANCE CVAR_ENHANCEMENT("PitMoverChance")
// "Disable Attack Badges" (on by default): while it is on, the Spin Attack, First Attack and Bump Attack badges do nothing
// inside the Pit. Turning it off on any floor marks the run (see pit_run_is_weenie).
#define CVAR_PIT_NO_ATTACK_BADGES CVAR_ENHANCEMENT("PitNoAttackBadges")
// Whether the deepest floor reached (CVAR_PIT_BEST_FLOOR) was reached on a run that turned that option off.
#define CVAR_PIT_BEST_FLOOR_WEENIE CVAR_ENHANCEMENT("PitBestFloorWeenie")
// Whether the deepest floor reached was reached on an "impressive" run: Hard Mode on at 2.0x health and 1.5x attack or higher,
// the action command options at their defaults, Super Guard off or at 1 frame, "Disable Attack Badges" on, and the Mover
// and shop chances at their defaults or lower.
#define CVAR_PIT_BEST_FLOOR_IMPRESSIVE CVAR_ENHANCEMENT("PitBestFloorImpressive")
// Debug builds only: make the entrance sign show the weenie or the impressive line whatever the record says.
#define CVAR_PIT_DEBUG_SIGN_WEENIE CVAR_ENHANCEMENT("PitDebugSignWeenie")
#define CVAR_PIT_DEBUG_SIGN_IMPRESSIVE CVAR_ENHANCEMENT("PitDebugSignImpressive")
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

// Sets the deepest floor reached (the number on the entrance sign) back to 0, and clears the weenie mark with it.
void pit_reset_best_floor(void);

// True if the deepest floor was reached on a run that had "Disable Attack Badges" off on some floor.
int pit_best_floor_weenie(void);

// Marks the deepest floor as reached on such a run.
void pit_mark_best_floor_weenie(void);

// True if the deepest floor was reached on an impressive run (see CVAR_PIT_BEST_FLOOR_IMPRESSIVE).
int pit_best_floor_impressive(void);

// Takes the impressive mark off the deepest floor.
void pit_clear_best_floor_impressive(void);

// True if the Spin Attack, First Attack and Bump Attack badges are switched off right now: Mario is in the Pit and
// "Disable Attack Badges" is on.
int pit_attack_badges_blocked(void);

// True if the settings have been impressive on every floor of the current run so far.
int pit_run_is_impressive(void);

// True if "Disable Attack Badges" has been off on any floor of the current run. The boss then pays out a Dried Shroom.
int pit_run_is_weenie(void);

// How the first enemy of a battle looks in the overworld: 16 animation IDs (in the order of NpcData::animations)
// and the collision height, radius and level. Returns 0 if the battle has no usable enemy.
int pit_enemy_look(int battleID, int* anims, int* height, int* radius, int* level);

// Picks a battle ID for the current floor.
int pit_pick_battle(void);

// Called once per game frame. Starts the map change if a request is waiting.
void pit_poll(void);

// The reward chests in the rest rooms (src/pit/pit_rewards.c). Each rest floor has a fixed reward that is paid out once
// per save file.
// Returns the reward for this floor and marks it as taken, or ITEM_NONE (0) if it was already taken or the floor has none.
int pit_reward_take(int floor);

// The coins the chest holds once its reward is gone.
int pit_reward_consolation(int floor);

// Forgets which rewards were taken, so the chests pay out again (debug).
void pit_reward_reset(void);

#ifdef __cplusplus
}
#endif

#endif
