#include "port/ShipInit.hpp"
#include "port/Engine.h"
#include "port/hooks/Events.h"
#include "port/ui/cvar_prefixes.h"

#include "common.h"
#include "battle/battle.h"
#include "pit_of_trials.h"

// The floor 100 boss (Electro Super Blooper, see src/pit/pit_boss_actor.c) hits this many times harder than vanilla.
#define PIT_BOSS_DAMAGE_MULTIPLIER 4

int pit_best_floor(void) {
    return CVarGetInteger(CVAR_PIT_BEST_FLOOR, 0);
}

void pit_record_floor(int floor) {
    if (floor <= pit_best_floor()) {
        return;
    }
    CVarSetInteger(CVAR_PIT_BEST_FLOOR, floor);
    Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
}

void RegisterPitOfTrials_Init() {
    REGISTER_LISTENER(GameFrameUpdate, EVENT_PRIORITY_NORMAL, [](IEvent*) { pit_poll(); });

    // Multiplies the boss's base attack damage before defense is subtracted. The boss has its own actor type.
    REGISTER_LISTENER(OnEnemyAttackDamage, EVENT_PRIORITY_NORMAL, [](IEvent* event) {
        OnEnemyAttackDamage* ev = (OnEnemyAttackDamage*) event;
        if (ev->attacker != nullptr && ev->attacker->actorBlueprint != nullptr
            && ev->attacker->actorBlueprint->type == ACTOR_TYPE_ELECTRO_BLOOPER2 && *ev->damage > 0)
        {
            *ev->damage *= PIT_BOSS_DAMAGE_MULTIPLIER;
        }
    });
}

static RegisterShipInitFunc initPitOfTrialsFunc(RegisterPitOfTrials_Init);
