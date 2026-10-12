#include "battle/area/tik2/area.h"
#include "sprite/npc/Blooper.h"

// The Pit of Trials floor 100 boss: an Electro Blooper that is as big as a Super Blooper and has a lot more HP.
// It reuses the Electro Blooper's parts, status table and behavior. The extra damage is in PitOfTrials.cpp.
// It has its own actor type (ACTOR_TYPE_ELECTRO_BLOOPER2, otherwise unused) so only this boss gets the extra damage.

#define NAMESPACE pit_electro_super_blooper

#define PIT_BOSS_HP 150

extern EvtScript b_area_tik2_electro_blooper_EVS_Init;
extern ActorPartBlueprint b_area_tik2_electro_blooper_ActorParts[];
extern s32 b_area_tik2_electro_blooper_StatusTable[];

// Runs the Electro Blooper's setup, then grows it to Super Blooper size.
EvtScript N(EVS_Init) = {
    ExecWait(b_area_tik2_electro_blooper_EVS_Init)
    Call(SetActorScale, ACTOR_SELF, Float(2.0), Float(2.0), Float(2.0))
    Return
    End
};

ActorBlueprint pit_electro_super_blooper = {
    .flags = ACTOR_FLAG_FLYING,
    .type = ACTOR_TYPE_ELECTRO_BLOOPER2,
    .level = ACTOR_LEVEL_SUPER_BLOOPER1,
    .maxHP = PIT_BOSS_HP,
    .partCount = 2,
    .partsData = b_area_tik2_electro_blooper_ActorParts,
    .initScript = &N(EVS_Init),
    .statusTable = b_area_tik2_electro_blooper_StatusTable,
    .escapeChance = 0,
    .airLiftChance = 0,
    .hurricaneChance = 0,
    .spookChance = 0,
    .upAndAwayChance = 0,
    .spinSmashReq = 4,
    .powerBounceChance = 75,
    .coinReward = 0,
    .size = { -111, -88 },
    .healthBarOffset = { 0, 0 },
    .statusIconOffset = { 0, 60 },
    .statusTextOffset = { 40, 80 },
};
