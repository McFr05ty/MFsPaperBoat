#include "tik_02.h"
#include "entity.h"
#include "pit_of_trials.h"

#include "world/common/entity/Chest.inc.c"

EvtScript N(EVS_OpenChest) = EVT_OPEN_CHEST(ITEM_SHRINK_STOMP, GF_TIK02_Chest_ShrinkStomp);

// Pit of Trials: going down the entrance pipe starts a run.
API_CALLABLE(N(StartPit)) {
    pit_request_enter();
    return ApiStatus_DONE2;
}

extern EvtScript pit_sign_EVS_ReadEntranceSign; // src/pit/pit_sign.c

EvtScript N(EVS_PitPipeExit) = {
    Call(N(StartPit))
    Wait(100)
    Return
    End
};

EvtScript N(EVS_MakeEntities) = {
    Call(MakeEntity, Ref(Entity_Chest), 280, -10, 0, 0, 0, MAKE_ENTITY_END)
    Call(AssignChestFlag, GF_TIK02_Chest_ShrinkStomp)
    Call(AssignScript, Ref(N(EVS_OpenChest)))
    // Pit of Trials entrance. It is made after the chest lines above, which apply to the last entity made.
    Call(MakeEntity, Ref(Entity_BlueWarpPipe), 175, -10, -65, 0,
        1, Ref(N(EVS_PitPipeExit)),
        EVT_INDEX_OF_GAME_FLAG(GF_TIK02_Defeated_Blooper), MAKE_ENTITY_END)
    // The sign beside the entrance pipe: the Pit's name and the deepest floor reached.
    Call(MakeEntity, Ref(Entity_Signpost), 125, -10, -60, 0, MAKE_ENTITY_END)
    Call(AssignScript, Ref(pit_sign_EVS_ReadEntranceSign))
        // The save block and heart block next to the Pit entrance.
        Call(MakeEntity, Ref(Entity_SavePoint), 100, 50, -60, 0, MAKE_ENTITY_END)
        Call(MakeEntity, Ref(Entity_HeartBlock), 235, 50, -60, 0, MAKE_ENTITY_END)
    Return
    End
};
