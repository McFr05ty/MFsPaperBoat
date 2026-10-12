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
extern EvtScript pit_sign_EVS_ReadSign; // src/pit/pit_sign.c

// The Mover (src/pit/pit_mover.c): pit_mover_Roll sets LVar0 to true when he is on this floor.
API_CALLABLE(pit_mover_Roll);
extern NpcGroupList pit_mover_NPCs;

enum {
    NPC_Gloomba = 0,
};

EntryList N(Entrances) = {
    [0] { 60.0, 140.0, 7.0, 90.0 },  // where Mario drops in
    [1] { 60.0, -10.0, 7.0, 90.0 },  // the progress pipe (centre of the room)
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
    pit_request_next_floor();
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

// The battle chosen for this floor (see PrepareEnemy below).
static s32 sFloorBattle;

// Gives the enemy this floor's battle, using that battle's own arena (stage -1).
API_CALLABLE(N(PickBattle)) {
    Enemy* enemy = script->owner1.enemy;
    Encounter* encounter = gCurrentEncounter.encounterList[enemy->encounterIndex];

    encounter->battle = sFloorBattle;
    encounter->stage = -1;
    return ApiStatus_DONE2;
}

EvtScript N(EVS_NpcInit_Gloomba) = {
    Call(N(PickBattle))
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

// Picks this floor's battle, then makes the overworld enemy look like the first enemy of that battle.
// This must run before the NPC is made. The NPC data below keeps the Gloomba look if the battle has no usable enemy.
API_CALLABLE(N(PrepareEnemy)) {
    int anims[16];
    int height;
    int radius;
    int level;

    sFloorBattle = pit_pick_battle();
    if (pit_enemy_look(sFloorBattle, anims, &height, &radius, &level)) {
        memcpy(&N(NpcData_Gloomba).animations, anims, sizeof(N(NpcData_Gloomba).animations));
        N(NpcSettings_Gloomba_Wander).height = height;
        N(NpcSettings_Gloomba_Wander).radius = radius;
        N(NpcSettings_Gloomba_Wander).level = level;
    }
    return ApiStatus_DONE2;
}

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
    // The pipe starts hidden and rises when the flag is set by the defeat script.
    Set(GF_Unused_EVT_0A, false)
    Call(N(ClearDefeatedFlags))
    // Now and then the Mover is here instead of the enemy. With nothing to fight, the pipe is up from the start.
    Call(pit_mover_Roll)
    IfEq(LVar0, true)
        Set(GF_Unused_EVT_0A, true)
        Call(MakeNpcs, true, Ref(pit_mover_NPCs))
    Else
        Call(N(PrepareEnemy))
        Call(MakeNpcs, true, Ref(N(DefaultNPCs)))
    EndIf
    Call(MakeEntity, Ref(Entity_BlueWarpPipe), 60, -10, 7, 0,
        1, Ref(N(EVS_PipeExit)),
        EVT_INDEX_OF_GAME_FLAG(GF_Unused_EVT_0A), MAKE_ENTITY_END)
    // The floor sign, off to the side so it stays out of the way.
    Call(MakeEntity, Ref(Entity_Signpost), 20, -10, -30, 0, MAKE_ENTITY_END)
    Call(AssignScript, Ref(pit_sign_EVS_ReadSign))
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
