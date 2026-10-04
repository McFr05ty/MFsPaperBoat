#ifndef RARE_ENEMY_H
#define RARE_ENEMY_H

#include <stdio.h>
#include "port/ui/cvar_prefixes.h"
#include <libultraship/bridge/consolevariablebridge.h>

// Setting name for the Rare Enemies checkbox.
#define CVAR_RARE_ENEMIES CVAR_ENHANCEMENT("RareEnemies")
// Setting name for the spawn chance slider (0-100). RARE_ENEMY_CHANCE_PERCENT is its default.
#define CVAR_RARE_CHANCE CVAR_ENHANCEMENT("RareSpawnChance")

// Chance (0-100) that a hostile overworld enemy spawns rare, rolled each time a map loads.
#define RARE_ENEMY_CHANCE_PERCENT 5


// Reward tuning for rare fights (easy to change).
#define RARE_STAR_POINT_MULTIPLIER 2
#define RARE_STAR_POINT_MINIMUM_BONUS 1
#define RARE_STAR_POINT_FLOOR 5  // every defeated enemy in a rare fight drops at least this many star points

// Holographic look tuning (overworld).
#define RARE_HOLO_STRENGTH 60  // percent of rainbow mixed in (0-100)
#define RARE_HOLO_SPEED 3      // how fast the colors cycle (frameCounter rises 2 per frame)
#define RARE_HOLO_SPREAD 48    // hue offset between neighboring palette colors

// Rare fight bonuses, applied inside the damage code so scripts cannot reset them.
#define RARE_ATTACK_BONUS 2
#define RARE_DEFENSE_BONUS 1

// True while the current battle was started by a rare overworld enemy.
#define RARE_FIGHT_ACTIVE() (CVarGetInteger(CVAR_RARE_ENEMIES, 0) && !gCurrentEncounter.scriptedBattle && gCurrentEncounter.curEnemy != nullptr && gCurrentEncounter.curEnemy->isRare)

#endif
