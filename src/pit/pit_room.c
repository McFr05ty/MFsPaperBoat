#include "common.h"
#include "message_ids.h"
#include "map.h"
#include "mapfs/tik_18_shape.h"
#include "mapfs/tik_18_hit.h"

#define NAMESPACE pit_room

// A Pit room borrows the geometry of tik_18 (see pit.c) and runs its own script.
extern EvtScript tik_18_EVS_SetupMusic;
extern EvtScript tik_18_EVS_SetupDrips;

EntryList N(Entrances) = {
    { 60.0, 140.0, 7.0, 90.0 },
};

EvtScript N(EVS_Main) = {
    Set(GB_WorldLocation, LOCATION_TOAD_TOWN_TUNNELS)
    Call(SetSpriteShading, SHADING_TIK_18)
    EVT_SETUP_CAMERA_NO_LEAD(0, 0, 0)
    Exec(tik_18_EVS_SetupDrips)
    Exec(tik_18_EVS_SetupMusic)
    Call(PlaySound, SOUND_LOOP_TIK18_WATER)
    // The doors are not used. Their triggers are never bound, and the door models are hidden.
    Call(EnableModel, MODEL_exite, false)
    Call(EnableModel, MODEL_exitw, false)
    // Mario drops in above the centre of the floor.
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
