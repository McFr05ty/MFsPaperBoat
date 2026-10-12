#include "common.h"
#include "message_ids.h"
#include "map.h"
#include "entity.h"
#include "mapfs/tik_18_shape.h"
#include "mapfs/tik_18_hit.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "pit_of_trials.h"

#define NAMESPACE pit_room

#include "world/common/enemy/Gloomba_Wander.inc.c"

// A Pit room borrows the geometry of tik_18 (see pit.c) and runs its own script.
extern EvtScript tik_18_EVS_SetupMusic;
extern EvtScript tik_18_EVS_SetupDrips;

enum {
    NPC_Gloomba = 0,
};

EntryList N(Entrances) = {
    [0] { 60.0, 140.0, 7.0, 90.0 },  // where Mario drops in
    [1] { 60.0, -10.0, 7.0, 90.0 },  // the progress pipe (centre of the room)
};

// Debug: hides the model chosen with the Pit menu slider (-1 = none), so we can find models by eye.
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

// Enemies defeated earlier in this same map would not spawn again, so forget them on room entry.
API_CALLABLE(N(ClearDefeatedFlags)) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(gCurrentEncounter.defeatFlags[0]); i++) {
        gCurrentEncounter.defeatFlags[gGameStatusPtr->mapID][i] = 0;
    }
    return ApiStatus_DONE2;
}

API_CALLABLE(N(RequestNextRoom)) {
    pit_request_enter();
    return ApiStatus_DONE2;
}

// Same as the default enemy defeat script, plus raising the pipe flag on a win.
EvtScript N(EVS_NpcDefeat_Gloomba) = {
    Call(SetNpcRotation, NPC_SELF, 0, 0, 0)
    Call(GetBattleOutcome, LVar0)
    Switch(LVar0)
        CaseEq(OUTCOME_PLAYER_WON)
            Set(GF_Unused_EVT_0A, true)
            Call(DoNpcDefeat)
        CaseEq(OUTCOME_PLAYER_FLED)
            Call(OnPlayerFled, 0)
        CaseEq(OUTCOME_ENEMY_FLED)
            Call(SetEnemyFlagBits, NPC_SELF, ENEMY_FLAG_FLED, true)
            Call(RemoveNpc, NPC_SELF)
    EndSwitch
    Return
    End
};

EvtScript N(EVS_NpcInit_Gloomba) = {
    Call(BindNpcDefeat, NPC_SELF, Ref(N(EVS_NpcDefeat_Gloomba)))
    Return
    End
};

NpcData N(NpcData_Gloomba) = {
    .id = NPC_Gloomba,
    .pos = { -100.0f, -10.0f, 0.0f },
    .yaw = 90,
    .territory = {
        .wander = {
            .isFlying = true,
            .moveSpeedOverride = NO_OVERRIDE_MOVEMENT_SPEED,
            .wanderShape = SHAPE_CYLINDER,
            .centerPos  = { -100, -10, 0 },
            .wanderSize = { 30 },
            .detectShape = SHAPE_CYLINDER,
            .detectPos  = { -100, -10, 0 },
            .detectSize = { 200 },
        }
    },
    .init = &N(EVS_NpcInit_Gloomba),
    .settings = &N(NpcSettings_Gloomba_Wander),
    .flags = ENEMY_FLAG_IGNORE_ENTITY_COLLISION | ENEMY_FLAG_FLYING | ENEMY_FLAG_NO_SHADOW_RAYCAST,
    .drops = GLOOMBA_DROPS,
    .animations = GLOOMBA_ANIMS,
};

NpcGroupList N(DefaultNPCs) = {
    NPC_GROUP(N(NpcData_Gloomba), BTL_TIK_FORMATION_0A, BTL_TIK_STAGE_00),
    {}
};

// Mario goes down the pipe: ask for the next room (for now, the same one again).
EvtScript N(EVS_PipeExit) = {
    Call(N(RequestNextRoom))
    Wait(100)
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
    // The doorways stay as archways. Their colliders are solid and their exit triggers are never bound.
    Exec(N(EVS_DebugModels))
    // The pipe starts hidden and rises when the flag is set by the defeat script.
    Set(GF_Unused_EVT_0A, false)
    Call(N(ClearDefeatedFlags))
    Call(MakeNpcs, true, Ref(N(DefaultNPCs)))
    Call(MakeEntity, Ref(Entity_BlueWarpPipe), 60, -10, 7, 0,
        1, Ref(N(EVS_PipeExit)),
        EVT_INDEX_OF_GAME_FLAG(GF_Unused_EVT_0A), MAKE_ENTITY_END)
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
