#include "common.h"
#include "message_ids.h"
#include "map.h"
#include "entity.h"
#include "mapfs/tik_18_shape.h"
#include "mapfs/tik_18_hit.h"
#include "pit_of_trials.h"

#define NAMESPACE pit_rest

#include "world/common/entity/Chest.inc.c"

// A Pit rest room borrows the geometry of tik_18 (see pit.c) and runs its own script.
extern EvtScript tik_18_EVS_SetupMusic;
extern EvtScript tik_18_EVS_SetupDrips;

EntryList N(Entrances) = {
    [0] { 60.0, 140.0, 7.0, 90.0 },   // where Mario drops in
    [1] { 90.0, -10.0, 7.0, 90.0 },   // progress pipe, centre of the room, shifted 30 right
    [2] { 300.0, -10.0, 0.0, 90.0 },  // return pipe, near the rightmost wall
};

API_CALLABLE(N(RequestNextFloor)) {
    pit_request_next_floor();
    return ApiStatus_DONE2;
}

EvtScript N(EVS_ProgressPipeExit) = {
    Call(N(RequestNextFloor))
    Wait(100)
    Return
    End
};

// Leaving the Pit: clear the spare flags used by the Pit rooms, then go back to the tunnels.
EvtScript N(EVS_ReturnPipeExit) = {
    Set(GF_Unused_EVT_0A, false)
    Set(GF_Unused_EVT_0B, false)
    Set(GF_Unused_EVT_0C, false)
    Call(GotoMap, Ref("tik_02"), 1)
    Wait(100)
    Return
    End
};

EvtScript N(EVS_OpenChest) = EVT_OPEN_CHEST(ITEM_JELLY_ULTRA, GF_Unused_EVT_0C);

NpcGroupList N(DefaultNPCs) = {
    {}
};

EvtScript N(EVS_Main) = {
    Set(GB_WorldLocation, LOCATION_TOAD_TOWN_TUNNELS)
    Call(SetSpriteShading, SHADING_TIK_18)
    EVT_SETUP_CAMERA_NO_LEAD(0, 0, 0)
    Exec(tik_18_EVS_SetupDrips)
    Exec(tik_18_EVS_SetupMusic)
    Call(PlaySound, SOUND_LOOP_TIK18_WATER)
    // Both pipes are raised from the start. The chest flag is cleared so every rest floor has a fresh chest.
    Set(GF_Unused_EVT_0A, true)
    Set(GF_Unused_EVT_0B, true)
    Set(GF_Unused_EVT_0C, false)
    Call(MakeNpcs, true, Ref(N(DefaultNPCs)))
    Call(MakeEntity, Ref(Entity_BlueWarpPipe), 90, -10, 7, 0,
        1, Ref(N(EVS_ProgressPipeExit)),
        EVT_INDEX_OF_GAME_FLAG(GF_Unused_EVT_0A), MAKE_ENTITY_END)
    Call(MakeEntity, Ref(Entity_BlueWarpPipe), 300, -10, 0, 0,
        2, Ref(N(EVS_ReturnPipeExit)),
        EVT_INDEX_OF_GAME_FLAG(GF_Unused_EVT_0B), MAKE_ENTITY_END)
    Call(MakeEntity, Ref(Entity_Chest), -100, -10, 0, 0, 0, MAKE_ENTITY_END)
    Call(AssignChestFlag, GF_Unused_EVT_0C)
    Call(AssignScript, Ref(N(EVS_OpenChest)))
    // Mario drops in above the centre of the room.
    Call(SetPlayerPos, 60, 140, 7)
    Call(TeleportPartnerToPlayer)
    Wait(1)
    Return
    End
};

MapSettings N(settings) = {
    .main = &N(EVS_Main),
    .entryList = &N(Entrances),
    .entryCount = ENTRY_COUNT(N(Entrances)),
    .tattle = { MSG_MapTattle_tik_18 },
};
