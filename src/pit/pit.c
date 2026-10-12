#include "common.h"
#include "map.h"
#include "game_modes.h"
#include "pit_of_trials.h"
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

void pit_request_enter(void) {
    int start = CVarGetInteger(CVAR_PIT_START_FLOOR, 1);

    if (start < 1) {
        start = 1;
    }
    if (start > PIT_LAST_FLOOR) {
        start = PIT_LAST_FLOOR;
    }
    sPitFloor = start;
    sPitRecordBest = (start == 1);
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
