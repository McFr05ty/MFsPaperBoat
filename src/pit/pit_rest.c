#include "common.h"
#include "message_ids.h"
#include "map.h"
#include "entity.h"
#include "mapfs/tik_18_shape.h"
#include "mapfs/tik_18_hit.h"
#include "pit_of_trials.h"
#include "pit_text.h"

#define NAMESPACE pit_rest

#include "world/common/entity/Chest.inc.c"

// A Pit rest room borrows the geometry of tik_18 (see pit.c) and runs its own script.
extern EvtScript tik_18_EVS_SetupMusic;
extern EvtScript tik_18_EVS_SetupDrips;
extern EvtScript pit_sign_EVS_ReadSign; // src/pit/pit_sign.c

// The shop (src/pit/pit_shop.c): pit_shop_Roll sets LVar0 to true when the shop is on this floor.
API_CALLABLE(pit_shop_Roll);
extern NpcGroupList pit_shop_NPCs;

// The Mover (src/pit/pit_mover.c): pit_mover_Roll sets LVar0 to true when he is on this floor.
// pit_mover_WithShopNPCs has both him and the shop, for when both are here.
API_CALLABLE(pit_mover_Roll);
extern NpcGroupList pit_mover_NPCs;
extern NpcGroupList pit_mover_WithShopNPCs;

EntryList N(Entrances) = {
    [0] { 60.0, 140.0, 7.0, 90.0 },   // where Mario drops in
    [1] { 120.0, -10.0, 7.0, 90.0 },   // progress pipe, centre of the room, shifted 60 right
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

// The chest on a rest floor. Its reward is paid out the first time on a save file (src/pit/pit_rewards.c). Afterwards it
// holds coins instead.
// LVar0 = the item (ITEM_NONE if the reward was already taken), LVar1 = the coins for then.
API_CALLABLE(N(PickReward)) {
    s32 floor = pit_current_floor();

    script->varTable[0] = pit_reward_take(floor);
    script->varTable[1] = pit_reward_consolation(floor);
    return ApiStatus_DONE2;
}

// "Nothing new in here. / Just [var 0] coins." Built in memory, with the number as message variable 0.
static u8 N(ConsolationMessage)[80];

API_CALLABLE(N(PrepareConsolation)) {
    Bytecode* args = script->ptrReadPos;
    s32 coins = evt_get_variable(script, *args++);
    u8* msg = N(ConsolationMessage);
    s32 pos = 0;

    msg[pos++] = MSG_CHAR_READ_STYLE;
    msg[pos++] = MSG_STYLE_SIGN;
    pos = pit_text_put(msg, pos, "Nothing new in here.\nJust ");
    msg[pos++] = MSG_CHAR_READ_FUNCTION;
    msg[pos++] = MSG_READ_FUNC_VAR;
    msg[pos++] = 0;
    pos = pit_text_put(msg, pos, " coins.");
    msg[pos++] = MSG_CHAR_READ_WAIT;
    msg[pos++] = MSG_CHAR_READ_END;

    set_message_int_var(coins, 0);
    return ApiStatus_DONE2;
}

EvtScript N(EVS_Chest_Consolation) = {
    Call(DisablePlayerInput, true)
    Call(N(PrepareConsolation), LVarB)
    Call(ShowMessageAtScreenPos, Ref(N(ConsolationMessage)), 160, 40)
    Call(AddCoin, LVarB)
    Wait(15)
    Call(DisablePlayerInput, false)
    Return
    End
};

EvtScript N(EVS_OpenChest) = {
    Call(N(PickReward))
    Set(LVarA, LVar0)
    Set(LVarB, LVar1)
    Set(GF_Unused_EVT_0C, true)
    IfNe(LVarA, ITEM_NONE)
        ExecWait(N(EVS_Chest_GetItem))
    Else
        ExecWait(N(EVS_Chest_Consolation))
    EndIf
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
    // Both pipes are raised from the start. The chest flag is cleared so every rest floor has a fresh chest.
    Set(GF_Unused_EVT_0A, true)
    Set(GF_Unused_EVT_0B, true)
    Set(GF_Unused_EVT_0C, false)
    // The shop and the Mover each have their own chance (Pit menu sliders), so both can be here. The NPCs are made in one go.
    Call(pit_shop_Roll)
    Set(LVarA, LVar0)
    Call(pit_mover_Roll)
    Set(LVarB, LVar0)
    IfEq(LVarA, true)
        IfEq(LVarB, true)
            Call(MakeNpcs, true, Ref(pit_mover_WithShopNPCs))
        Else
            Call(MakeNpcs, true, Ref(pit_shop_NPCs))
        EndIf
    Else
        IfEq(LVarB, true)
            Call(MakeNpcs, true, Ref(pit_mover_NPCs))
        EndIf
    EndIf
    Call(MakeEntity, Ref(Entity_BlueWarpPipe), 120, -10, 7, 0,
        1, Ref(N(EVS_ProgressPipeExit)),
        EVT_INDEX_OF_GAME_FLAG(GF_Unused_EVT_0A), MAKE_ENTITY_END)
    Call(MakeEntity, Ref(Entity_BlueWarpPipe), 300, -10, 0, 0,
        2, Ref(N(EVS_ReturnPipeExit)),
        EVT_INDEX_OF_GAME_FLAG(GF_Unused_EVT_0B), MAKE_ENTITY_END)
    Call(MakeEntity, Ref(Entity_Chest), -100, -10, 0, 0, 0, MAKE_ENTITY_END)
    Call(AssignChestFlag, GF_Unused_EVT_0C)
    Call(AssignScript, Ref(N(EVS_OpenChest)))
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
