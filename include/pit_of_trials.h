#ifndef PIT_OF_TRIALS_H
#define PIT_OF_TRIALS_H

#include "port/ui/cvar_prefixes.h"

// Debug: index of a Pit room model to hide, -1 for none.
#define CVAR_PIT_DEBUG_HIDE_MODEL CVAR_ENHANCEMENT("PitDebugHideModel")
#define CVAR_PIT_START_FLOOR CVAR_ENHANCEMENT("PitStartFloor")

#ifdef __cplusplus
extern "C" {
#endif

// Asks the game thread to jump to the Pit test room. Safe to call from the menu.
void pit_request_enter(void);

// Moves on to the next floor (called from the room script when Mario goes down the pipe).
void pit_request_next_floor(void);

// The floor Mario is on (1-99).
int pit_current_floor(void);

// Picks a battle ID for the current floor.
int pit_pick_battle(void);

// Called once per game frame. Starts the map change if a request is waiting.
void pit_poll(void);

#ifdef __cplusplus
}
#endif

#endif
