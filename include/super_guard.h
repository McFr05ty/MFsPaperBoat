#ifndef SUPER_GUARD_H
#define SUPER_GUARD_H

#include "port/ui/cvar_prefixes.h"
#include <libultraship/bridge/consolevariablebridge.h>

// Setting name for the Super Guard checkbox (off by default).
#define CVAR_SUPER_GUARD CVAR_ENHANCEMENT("SuperGuard")
// Setting name for the Super Guard timing slider (1-5 frames). SUPER_GUARD_WINDOW is its default.
#define CVAR_SUPER_GUARD_WINDOW CVAR_ENHANCEMENT("SuperGuardWindow")

// Super Guard: pressing B on the exact frame an enemy attack hits negates all damage.

// How many frames before the hit a B press still counts (1 = frame perfect).
#define SUPER_GUARD_WINDOW 1

// A B press in this many frames just before the window cancels the Super Guard (anti-mashing).
#define SUPER_GUARD_MASH_FRAMES 10

// Damage reflected onto the attacker when super guarding a direct attack (0 turns reflecting off).
#define SUPER_GUARD_REFLECT_DAMAGE 1

#endif
