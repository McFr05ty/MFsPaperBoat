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

void pit_reset_best_floor(void) {
    CVarSetInteger(CVAR_PIT_BEST_FLOOR, 0);
    CVarSetInteger(CVAR_PIT_BEST_FLOOR_WEENIE, 0);
    CVarSetInteger(CVAR_PIT_BEST_FLOOR_IMPRESSIVE, 0);
    Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
}

int pit_best_floor_weenie(void) {
    return CVarGetInteger(CVAR_PIT_BEST_FLOOR_WEENIE, 0);
}

int pit_best_floor_impressive(void) {
    return CVarGetInteger(CVAR_PIT_BEST_FLOOR_IMPRESSIVE, 0);
}

void pit_clear_best_floor_impressive(void) {
    if (!pit_best_floor_impressive()) {
        return;
    }
    CVarSetInteger(CVAR_PIT_BEST_FLOOR_IMPRESSIVE, 0);
    Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
}

void pit_record_floor(int floor) {
    if (floor <= pit_best_floor()) {
        return;
    }
    CVarSetInteger(CVAR_PIT_BEST_FLOOR, floor);
    // A new record is a weenie record if this run has already turned the badge option off, and an impressive record
    // if the settings have been impressive so far.
    CVarSetInteger(CVAR_PIT_BEST_FLOOR_WEENIE, pit_run_is_weenie());
    CVarSetInteger(CVAR_PIT_BEST_FLOOR_IMPRESSIVE, pit_run_is_impressive());
    Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
}

void pit_mark_best_floor_weenie(void) {
    if (pit_best_floor_weenie()) {
        return;
    }
    CVarSetInteger(CVAR_PIT_BEST_FLOOR_WEENIE, 1);
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
