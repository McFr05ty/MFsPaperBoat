#include "common.h"
#include "map.h"
#include "game_modes.h"
#include "pit_of_trials.h"
#include <string.h>
#include <libultraship/bridge/consolevariablebridge.h>

extern AreaConfig gAreas[];
extern MapSettings pit_room_settings;
extern MapSettings pit_rest_settings;

#define PIT_MAX_TIK_MAPS 48

// Copy of the Toad Town Tunnels map list with the Pit rooms added at the end.
static MapConfig sTikMaps[PIT_MAX_TIK_MAPS];
static s32 sPitAreaIndex = -1;
static s32 sPitRoomIndex = -1;
static s32 sPitRestIndex = -1;
static volatile s32 sPitEnterRequested = 0;

// A Pit room is a copy of the tik_18 entry (so its shape, collision and textures load)
// that runs our own script. Name lookups still find the real tik_18 first.
static b32 pit_install_rooms(void) {
    s32 i;
    s32 j;
    s32 count;
    s32 tik18 = -1;

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
    if (count + 2 > PIT_MAX_TIK_MAPS) {
        return false;
    }

    memcpy(sTikMaps, gAreas[i].maps, count * sizeof(MapConfig));
    for (j = 0; j < count; j++) {
        if (strcmp(sTikMaps[j].id, "tik_18") == 0) {
            tik18 = j;
            break;
        }
    }
    if (tik18 < 0) {
        return false;
    }

    sTikMaps[count] = sTikMaps[tik18];
    sTikMaps[count].settings = &pit_room_settings;
    sTikMaps[count + 1] = sTikMaps[tik18];
    sTikMaps[count + 1].settings = &pit_rest_settings;

    gAreas[i].maps = sTikMaps;
    gAreas[i].mapCount = count + 2;
    sPitAreaIndex = i;
    sPitRoomIndex = count;
    sPitRestIndex = count + 1;
    return true;
}

extern const s16 gPitBattleIDs[];
extern const s16 gPitBattleScores[];
extern const s32 gPitBattleCount;

#define PIT_LAST_FLOOR 99
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
    sPitEnterRequested = 1;
}

void pit_request_next_floor(void) {
    if (sPitFloor < PIT_LAST_FLOOR) {
        sPitFloor++;
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

    gGameStatusPtr->areaID = sPitAreaIndex;
    gGameStatusPtr->mapID = (sPitFloor % 10 == 0) ? sPitRestIndex : sPitRoomIndex;
    gGameStatusPtr->entryID = 0;
    set_map_transition_effect(TRANSITION_STANDARD);
    set_game_mode(GAME_MODE_CHANGE_MAP);
}
