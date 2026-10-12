#include "common.h"
#include "message_ids.h"
#include "map.h"
#include "entity.h"
#include "mapfs/tik_02_shape.h"
#include "mapfs/tik_02_hit.h"
#include "pit_of_trials.h"
#include "holofoil_hero.h"
#include "inventory.h"

#define NAMESPACE pit_boss

#include "world/common/atomic/TexturePan.inc.c"
#include "world/common/entity/Chest.inc.c"
#include "world/common/enemy/Blooper.h"

// The floor 100 boss room borrows the geometry of tik_02, the Blooper room (see pit.c), and runs its own script.
extern EvtScript tik_02_EVS_SetupMusic;
extern EvtScript tik_02_EVS_PlayBlooperSong;
extern EvtScript tik_02_EVS_SetupDrips;

enum {
    NPC_Blooper = 0,
};

EntryList N(Entrances) = {
    [0] { -250.0, 140.0, 0.0, 90.0 },  // where Mario drops in
    [1] { 175.0, -10.0, -65.0, 180.0 },  // the return pipe
};

// Enemies defeated earlier in this same map would not spawn again, so forget them on room entry.
API_CALLABLE(N(ClearDefeatedFlags)) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(gCurrentEncounter.defeatFlags[0]); i++) {
        gCurrentEncounter.defeatFlags[gGameStatusPtr->mapID][i] = 0;
    }
    return ApiStatus_DONE2;
}

NpcSettings N(NpcSettings_Blooper) = {
    .height = 24,
    .radius = 24,
    .level = ACTOR_LEVEL_NONE,
};

// Same scene as the Blooper in tik_02: Mario walks in, the Blooper jumps out of the water and the boss fight starts.
EvtScript N(EVS_NpcIdle_Blooper) = {
    Loop(0)
        Call(GetPlayerPos, LVar0, LVar1, LVar2)
        IfGt(LVar0, 65)
            BreakLoop
        EndIf
        Wait(1)
    EndLoop
    Call(DisablePlayerInput, true)
    Exec(tik_02_EVS_PlayBlooperSong)
    Call(ShowMessageAtScreenPos, MSG_MGM_0000, 160, 40)
    Thread
        Call(InterpPlayerYaw, 315, 0)
        Wait(50)
        Call(InterpPlayerYaw, 45, 0)
    EndThread
    Call(UseSettingsFrom, CAM_DEFAULT, -25, LVar1, LVar2)
    Call(SetPanTarget, CAM_DEFAULT, -25, LVar1, LVar2)
    Call(SetCamSpeed, CAM_DEFAULT, Float(1.0))
    Call(PanToTarget, CAM_DEFAULT, 0, true)
    Call(WaitForCam, CAM_DEFAULT, Float(1.0))
    Wait(20)
    Thread
        Wait(10)
        Call(SetPanTarget, CAM_DEFAULT, 140, LVar1, LVar2)
        Call(WaitForCam, CAM_DEFAULT, Float(1.0))
    EndThread
    Call(PlaySoundAtNpc, NPC_SELF, SOUND_BLOOPER_MOVE, SOUND_SPACE_DEFAULT)
    Call(PlaySoundAtNpc, NPC_SELF, SOUND_LARGE_ACTOR_JUMP, SOUND_SPACE_DEFAULT)
    Call(SetNpcPos, NPC_SELF, -100, -300, -120)
    Call(SetNpcAnimation, NPC_SELF, ANIM_Blooper_Anim01)
    Call(SetNpcJumpscale, NPC_SELF, Float(1.0))
    Call(NpcJump0, NPC_SELF, 175, 0, 0, 40)
    Call(SetNpcAnimation, NPC_SELF, ANIM_Blooper_Anim0C)
    Call(ShakeCam, CAM_DEFAULT, 0, 5, Float(1.0))
    Wait(15)
    Call(StartBossBattle, SONG_SPECIAL_BATTLE)
    Return
    End
};

// The boss chest holds the Holofoil Hero badge. Mario only needs one, so when he already has it the chest holds an
// Ultra Jelly instead. A run that turned "Disable Attack Badges" off on any floor only earns a Dried Shroom.
API_CALLABLE(N(PickReward)) {
    if (pit_run_is_weenie()) {
        script->varTable[0] = ITEM_DRIED_SHROOM;
    } else {
        script->varTable[0] = has_item(HOLOFOIL_HERO_ITEM) ? ITEM_JELLY_ULTRA : HOLOFOIL_HERO_ITEM;
    }
    return ApiStatus_DONE2;
}

EvtScript N(EVS_OpenChest) = {
    Call(N(PickReward))
    Set(LVarA, LVar0)
    Set(GF_Unused_EVT_0C, true)
    ExecWait(N(EVS_Chest_GetItem))
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

// The Blooper falls back into the water, then the return pipe rises and a chest appears.
EvtScript N(EVS_NpcDefeat_Blooper) = {
    Wait(5)
    Thread
        Call(GetPlayerPos, LVar0, LVar1, LVar2)
        Call(SetPanTarget, CAM_DEFAULT, LVar0, LVar1, LVar2)
        Call(WaitForCam, CAM_DEFAULT, Float(1.0))
    EndThread
    Thread
        Call(SetNpcAnimation, NPC_SELF, ANIM_Blooper_Anim04)
        Call(MakeLerp, 0, 6 * 360, 40, EASING_LINEAR)
        Loop(0)
            Call(UpdateLerp)
            Call(SetNpcRotation, NPC_SELF, 0, LVar0, 0)
            Call(PlayerFaceNpc, NPC_SELF, false)
            Wait(1)
            IfEq(LVar1, 0)
                BreakLoop
            EndIf
        EndLoop
    EndThread
    Call(PlaySoundAtNpc, NPC_SELF, SOUND_BLOOPER_FALL, SOUND_SPACE_DEFAULT)
    Call(PlaySoundAtNpc, NPC_SELF, SOUND_LARGE_ACTOR_JUMP, SOUND_SPACE_DEFAULT)
    Call(SetNpcJumpscale, NPC_SELF, Float(1.0))
    Call(NpcJump0, NPC_SELF, -100, -300, -120, 40)
    Wait(1)
    Call(PanToTarget, CAM_DEFAULT, 0, false)
    Exec(tik_02_EVS_SetupMusic)
    Call(MakeEntity, Ref(Entity_Chest), 280, -10, 0, 0, 0, MAKE_ENTITY_END)
    Call(AssignChestFlag, GF_Unused_EVT_0C)
    Call(AssignScript, Ref(N(EVS_OpenChest)))
    Set(GF_Unused_EVT_0A, true)
    Call(DisablePlayerInput, false)
    Call(RemoveNpc, NPC_SELF)
    Return
    End
};

EvtScript N(EVS_NpcInit_Blooper) = {
    Call(BindNpcIdle, NPC_SELF, Ref(N(EVS_NpcIdle_Blooper)))
    Call(BindNpcDefeat, NPC_SELF, Ref(N(EVS_NpcDefeat_Blooper)))
    Call(SetNpcScale, NPC_SELF, Float(2.0), Float(2.0), Float(2.0))
    Return
    End
};

NpcData N(NpcData_Blooper) = {
    .id = NPC_Blooper,
    .pos = { NPC_DISPOSE_LOCATION },
    .yaw = 90,
    .init = &N(EVS_NpcInit_Blooper),
    .settings = &N(NpcSettings_Blooper),
    .flags = ENEMY_FLAG_PASSIVE | ENEMY_FLAG_DO_NOT_KILL | ENEMY_FLAG_ENABLE_HIT_SCRIPT | ENEMY_FLAG_IGNORE_WORLD_COLLISION | ENEMY_FLAG_IGNORE_ENTITY_COLLISION | ENEMY_FLAG_FLYING | ENEMY_FLAG_NO_DELAY_AFTER_FLEE | ENEMY_FLAG_NO_DROPS,
    .drops = NO_DROPS,
    .animations = BLOOPER_ANIMS,
};

// The boss fight is tik2 battle 03, the Electro Super Blooper (see src/pit/pit_boss_actor.c).
NpcGroupList N(DefaultNPCs) = {
    NPC_GROUP(N(NpcData_Blooper), BTL_TIK2_FORMATION_03, BTL_TIK2_STAGE_00),
    {}
};

EvtScript N(EVS_Main) = {
    Set(GB_WorldLocation, LOCATION_TOAD_TOWN_TUNNELS)
    Call(SetSpriteShading, SHADING_TIK_02)
    EVT_SETUP_CAMERA_NO_LEAD(0, 0, 0)
    Exec(tik_02_EVS_SetupDrips)
    Exec(tik_02_EVS_SetupMusic)
    Call(PlaySound, SOUND_LOOP_TIK02_WATER)
    Call(PlaySoundAtF, SOUND_LOOP_TIK02_FLOW2, SOUND_SPACE_WITH_DEPTH, -230, -180, 120)
    Call(PlaySoundAtF, SOUND_LOOP_TIK02_FLOW3, SOUND_SPACE_WITH_DEPTH, 20, -180, 120)
    // water streams
    Call(SetTexPanner, MODEL_nagare1, TEX_PANNER_1)
    Thread
        TEX_PAN_PARAMS_ID(TEX_PANNER_1)
        TEX_PAN_PARAMS_STEP(   50,  200,  110,  500)
        TEX_PAN_PARAMS_FREQ(    1,    1,    1,    1)
        TEX_PAN_PARAMS_INIT(    0,    0,    0,    0)
        Exec(N(EVS_UpdateTexturePan))
    EndThread
    // water leaking
    Call(SetTexPanner, MODEL_mizu, TEX_PANNER_2)
    Thread
        TEX_PAN_PARAMS_ID(TEX_PANNER_2)
        TEX_PAN_PARAMS_STEP(    0, -200, -100, -500)
        TEX_PAN_PARAMS_FREQ(    1,    1,    1,    1)
        TEX_PAN_PARAMS_INIT(    0,    0,    0,    0)
        Exec(N(EVS_UpdateTexturePan))
    EndThread
    Call(SetTexPanner, MODEL_mizu2, TEX_PANNER_3)
    Thread
        TEX_PAN_PARAMS_ID(TEX_PANNER_3)
        TEX_PAN_PARAMS_STEP(    0, -250, -100, -500)
        TEX_PAN_PARAMS_FREQ(    1,    1,    1,    1)
        TEX_PAN_PARAMS_INIT(    0,    0,    0,    0)
        Exec(N(EVS_UpdateTexturePan))
    EndThread
    // waterfalls
    Call(SetTexPanner, MODEL_taki, TEX_PANNER_4)
    Thread
        TEX_PAN_PARAMS_ID(TEX_PANNER_4)
        TEX_PAN_PARAMS_STEP(  -50, -900,  -70,-1200)
        TEX_PAN_PARAMS_FREQ(    1,    1,    1,    1)
        TEX_PAN_PARAMS_INIT(    0,    0,    0,    0)
        Exec(N(EVS_UpdateTexturePan))
    EndThread
    // The return pipe starts hidden and rises once the boss is beaten. The chest flag is cleared for a fresh chest.
    Set(GF_Unused_EVT_0A, false)
    Set(GF_Unused_EVT_0B, false)
    Set(GF_Unused_EVT_0C, false)
    Call(N(ClearDefeatedFlags))
    Call(MakeNpcs, true, Ref(N(DefaultNPCs)))
    Call(MakeEntity, Ref(Entity_BlueWarpPipe), 175, -10, -65, 0,
        1, Ref(N(EVS_ReturnPipeExit)),
        EVT_INDEX_OF_GAME_FLAG(GF_Unused_EVT_0A), MAKE_ENTITY_END)
    // Mario drops in on the west side of the room.
    Call(SetPlayerPos, -250, 140, 0)
    Call(TeleportPartnerToPlayer)
    Wait(1)
    Return
    End
};

MapSettings N(settings) = {
    .main = &N(EVS_Main),
    .entryList = &N(Entrances),
    .entryCount = ENTRY_COUNT(N(Entrances)),
    .tattle = { MSG_MapTattle_tik_02 },
};
