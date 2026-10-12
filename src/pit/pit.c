#include "common.h"
#include "map.h"
#include "game_modes.h"
#include "pit_of_trials.h"
#include "hard_mode.h"
#include "super_guard.h"
#include <string.h>
#include <libultraship/bridge/consolevariablebridge.h>

extern AreaConfig gAreas[];
extern MapSettings pit_room_settings;
extern MapSettings pit_rest_settings;
extern MapSettings pit_boss_settings;

#define PIT_MAX_TIK_MAPS 48

// Copy of the Toad Town Tunnels map list with the Pit rooms added at the end.
static MapConfig sTikMaps[PIT_MAX_TIK_MAPS];
static s32 sPitAreaIndex = -1;
static s32 sPitRoomIndex = -1;
static s32 sPitRestIndex = -1;
static s32 sPitBossIndex = -1;
static volatile s32 sPitEnterRequested = 0;
// Runs that start on a later floor (the debug start floor) do not count towards the best floor.
static s32 sPitRecordBest = 1;
// The current run has had "Disable Attack Badges" off on some floor.
static s32 sPitRunWeenie = 0;
// The settings have been impressive on every floor of the current run so far (see pit_settings_impressive).
static s32 sPitRunImpressive = 1;
// The current run is the one that set the best floor.
static s32 sPitHoldsRecord = 0;

// A Pit room is a copy of the tik_18 entry (so its shape, collision and textures load)
// that runs our own script. Name lookups still find the real tik_18 first.
static b32 pit_install_rooms(void) {
    s32 i;
    s32 j;
    s32 count;
    s32 tik18 = -1;
    s32 tik02 = -1;

    if (sPitRoomIndex >= 0) {
        return true;
    }

    for (i = 0; gAreas[i].maps != nullptr; i++) {
        if (strcmp(gAreas[i].id, "area_tik") == 0) {
            break;
        }
    }
    if (gAreas[i].maps == nullptr) {
        return false;
    }

    count = gAreas[i].mapCount;
    if (count + 3 > PIT_MAX_TIK_MAPS) {
        return false;
    }

    memcpy(sTikMaps, gAreas[i].maps, count * sizeof(MapConfig));
    for (j = 0; j < count; j++) {
        if (strcmp(sTikMaps[j].id, "tik_18") == 0) {
            tik18 = j;
        }
        if (strcmp(sTikMaps[j].id, "tik_02") == 0) {
            tik02 = j;
        }
    }
    if (tik18 < 0 || tik02 < 0) {
        return false;
    }

    sTikMaps[count] = sTikMaps[tik18];
    sTikMaps[count].settings = &pit_room_settings;
    sTikMaps[count + 1] = sTikMaps[tik18];
    sTikMaps[count + 1].settings = &pit_rest_settings;
    // The boss room is a copy of the Blooper room (tik_02).
    sTikMaps[count + 2] = sTikMaps[tik02];
    sTikMaps[count + 2].settings = &pit_boss_settings;

    gAreas[i].maps = sTikMaps;
    gAreas[i].mapCount = count + 3;
    sPitAreaIndex = i;
    sPitRoomIndex = count;
    sPitRestIndex = count + 1;
    sPitBossIndex = count + 2;
    return true;
}

extern const s16 gPitBattleIDs[];
extern const s16 gPitBattleScores[];
extern const s32 gPitBattleCount;

#define PIT_LAST_FLOOR 100
#define PIT_BOSS_FLOOR 100
#define PIT_ANTI_GUY_FIRST_FLOOR 80
#define PIT_ANTI_GUY_PERCENT 5
#define PIT_ANTI_GUY_BATTLE 0x102C

static int sPitFloor = 1;

int pit_current_floor(void) {
    return sPitFloor;
}

// True while Mario is in one of the Pit rooms (they are extra maps in the Toad Town Tunnels area, after the real ones).
static int pit_is_active(void) {
    if (sPitRoomIndex < 0 || gGameStatusPtr->areaID != sPitAreaIndex) {
        return 0;
    }
    return gGameStatusPtr->mapID == sPitRoomIndex
        || gGameStatusPtr->mapID == sPitRestIndex
        || gGameStatusPtr->mapID == sPitBossIndex;
}

int pit_attack_badges_blocked(void) {
    return pit_is_active() && CVarGetInteger(CVAR_PIT_NO_ATTACK_BADGES, 1) != 0;
}

int pit_run_is_weenie(void) {
    return sPitRunWeenie;
}

int pit_run_is_impressive(void) {
    return sPitRunImpressive;
}

#define CVAR_ACTION_COMMAND_DIFFICULTY CVAR_ENHANCEMENT("ActionCommandDifficulty")
#define CVAR_BLOCK_WINDOW_MODE CVAR_ENHANCEMENT("BlockWindowMode")

// The settings that make a run impressive: Hard Mode on with at least the default 2.0x health and 1.5x attack, the action
// command options (difficulty and block window) at their defaults, Super Guard off or set to 1 frame, the Spin/First/Bump
// Attack badges disabled, and the Mover and the shop no more likely than their defaults.
static int pit_settings_impressive(void) {
    if (!CVarGetInteger(CVAR_HARD_MODE, 0)) {
        return 0;
    }
    if (hard_mode_halves(CVAR_HARD_MODE_HEALTH, HARD_MODE_DEFAULT_HEALTH_CHOICE) < HARD_MODE_DEFAULT_HEALTH_CHOICE + 2) {
        return 0;
    }
    if (hard_mode_halves(CVAR_HARD_MODE_ATTACK, HARD_MODE_DEFAULT_ATTACK_CHOICE) < HARD_MODE_DEFAULT_ATTACK_CHOICE + 2) {
        return 0;
    }
    if (CVarGetInteger(CVAR_ACTION_COMMAND_DIFFICULTY, 0) != 0 || CVarGetInteger(CVAR_BLOCK_WINDOW_MODE, 0) != 0) {
        return 0;
    }
    if (CVarGetInteger(CVAR_SUPER_GUARD, 0) && CVarGetInteger(CVAR_SUPER_GUARD_WINDOW, SUPER_GUARD_WINDOW) != 1) {
        return 0;
    }
    if (CVarGetInteger(CVAR_PIT_MOVER_CHANCE, PIT_MOVER_DEFAULT_CHANCE) > PIT_MOVER_DEFAULT_CHANCE) {
        return 0;
    }
    if (CVarGetInteger(CVAR_PIT_SHOP_CHANCE, PIT_SHOP_DEFAULT_CHANCE) > PIT_SHOP_DEFAULT_CHANCE) {
        return 0;
    }
    return CVarGetInteger(CVAR_PIT_NO_ATTACK_BADGES, 1) != 0;
}

void pit_request_enter(void) {
    int start = 1;

#ifdef _DEBUG
    // The start floor is a debug setting. Release builds ignore it (it may be left over in the config from a debug build).
    start = CVarGetInteger(CVAR_PIT_START_FLOOR, 1);
#endif

    if (start < 1) {
        start = 1;
    }
    if (start > PIT_LAST_FLOOR) {
        start = PIT_LAST_FLOOR;
    }
    sPitFloor = start;
    sPitRecordBest = (start == 1);
    sPitRunWeenie = 0;
    sPitRunImpressive = 1;
    sPitHoldsRecord = 0;
    sPitEnterRequested = 1;
}

void pit_request_next_floor(void) {
    if (sPitFloor < PIT_LAST_FLOOR) {
        sPitFloor++;
    }
    sPitEnterRequested = 1;
}

// The Mover: skips the given number of floors. Mario lands on the floor after the skipped ones, never on the boss floor.
void pit_request_skip_floors(int count) {
    sPitFloor += 1 + count;
    if (sPitFloor > PIT_LAST_FLOOR - 1) {
        sPitFloor = PIT_LAST_FLOOR - 1;
    }
    sPitEnterRequested = 1;
}

static int pit_near(int i, int target, int window) {
    int diff = gPitBattleScores[i] - target;

    if (diff < 0) {
        diff = -diff;
    }
    return diff <= window;
}

// Battle floors are 1-90 (every tenth floor is a rest floor). The target enemy level climbs from 5 to 30.
int pit_pick_battle(void) {
    int battleFloor = sPitFloor - sPitFloor / 10;
    int target = 50 + (battleFloor - 1) * 250 / 89;
    int window;
    int count = 0;
    int pick;
    int i;

    if (sPitFloor >= PIT_ANTI_GUY_FIRST_FLOOR && rand_int(99) < PIT_ANTI_GUY_PERCENT) {
        return PIT_ANTI_GUY_BATTLE;
    }
    for (window = 20; window <= 300; window += 10) {
        for (i = 0; i < gPitBattleCount; i++) {
            if (pit_near(i, target, window)) {
                count++;
            }
        }
        if (count > 0) {
            break;
        }
    }
    if (count == 0) {
        return gPitBattleIDs[0];
    }
    pick = rand_int(count - 1);
    for (i = 0; i < gPitBattleCount; i++) {
        if (pit_near(i, target, window)) {
            if (pick == 0) {
                return gPitBattleIDs[i];
            }
            pick--;
        }
    }
    return gPitBattleIDs[0];
}

void pit_poll(void) {
    // Checked every frame, so changing a setting on any floor counts, even in the middle of a fight.
    if (pit_is_active()) {
        if (CVarGetInteger(CVAR_PIT_NO_ATTACK_BADGES, 1) == 0) {
            sPitRunWeenie = 1;
            if (sPitHoldsRecord) {
                pit_mark_best_floor_weenie();
            }
        }
        if (!pit_settings_impressive()) {
            sPitRunImpressive = 0;
            if (sPitHoldsRecord) {
                pit_clear_best_floor_impressive();
            }
        }
    }

    if (!sPitEnterRequested) {
        return;
    }
    sPitEnterRequested = 0;

    if (gGameStatusPtr->context != CONTEXT_WORLD) {
        return;
    }
    if (!pit_install_rooms()) {
        return;
    }

    if (sPitRecordBest) {
        if (sPitFloor > pit_best_floor()) {
            sPitHoldsRecord = 1;
        }
        pit_record_floor(sPitFloor);
    }
    gGameStatusPtr->areaID = sPitAreaIndex;
    if (sPitFloor == PIT_BOSS_FLOOR) {
        gGameStatusPtr->mapID = sPitBossIndex;
    } else if (sPitFloor % 10 == 0) {
        gGameStatusPtr->mapID = sPitRestIndex;
    } else {
        gGameStatusPtr->mapID = sPitRoomIndex;
    }
    gGameStatusPtr->entryID = 0;
    set_map_transition_effect(TRANSITION_STANDARD);
    set_game_mode(GAME_MODE_CHANGE_MAP);
}
