#include "common.h"
#include "message_ids.h"
#include "map.h"
#include "mapfs/tik_18_shape.h"
#include "mapfs/tik_18_hit.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "pit_of_trials.h"

#define NAMESPACE pit_room

// A Pit room borrows the geometry of tik_18 (see pit.c) and runs its own script.
extern EvtScript tik_18_EVS_SetupMusic;
extern EvtScript tik_18_EVS_SetupDrips;

EntryList N(Entrances) = {
    { 60.0, 140.0, 7.0, 90.0 },
};

// Debug: hides the model chosen with the Pit menu slider (-1 = none), so we can find the door models.
API_CALLABLE(N(GetDebugHideModel)) {
    evt_set_variable(script, LVar0, CVarGetInteger(CVAR_PIT_DEBUG_HIDE_MODEL, -1));
    return ApiStatus_DONE2;
}

EvtScript N(EVS_DebugModels) = {
    Set(LVar9, -1)
    Label(0)
        Call(N(GetDebugHideModel))
        IfNe(LVar0, LVar9)
            IfGe(LVar9, 0)
                Call(EnableModel, LVar9, true)
            EndIf
            IfGe(LVar0, 0)
                Call(EnableModel, LVar0, false)
            EndIf
            Set(LVar9, LVar0)
        EndIf
        Wait(1)
        Goto(0)
    Return
    End
};

EvtScript N(EVS_Main) = {
    Set(GB_WorldLocation, LOCATION_TOAD_TOWN_TUNNELS)
    Call(SetSpriteShading, SHADING_TIK_18)
    EVT_SETUP_CAMERA_NO_LEAD(0, 0, 0)
    Exec(tik_18_EVS_SetupDrips)
    Exec(tik_18_EVS_SetupMusic)
    Call(PlaySound, SOUND_LOOP_TIK18_WATER)
    // The doors are not used. Their triggers are never bound. The door models are hidden from the Pit menu
    // slider (Hide model) while we find out which models they are.
    Exec(N(EVS_DebugModels))
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
