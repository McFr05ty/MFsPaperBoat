#ifndef HARD_MODE_H
#define HARD_MODE_H

#include "port/ui/cvar_prefixes.h"
#include <libultraship/bridge/consolevariablebridge.h>

#define CVAR_HARD_MODE CVAR_ENHANCEMENT("HardMode")
#define CVAR_HARD_MODE_HEALTH CVAR_ENHANCEMENT("HardModeHealthChoice")
#define CVAR_HARD_MODE_ATTACK CVAR_ENHANCEMENT("HardModeAttackChoice")

// Default choices: 2 = 2.0x health and 1 = 1.5x attack, the original Hard Mode values.
#define HARD_MODE_DEFAULT_HEALTH_CHOICE 2
#define HARD_MODE_DEFAULT_ATTACK_CHOICE 1

// Multiplier options are stored as a combo-box choice: 0 = 1.0x, 1 = 1.5x, 2 = 2.0x, 3 = 2.5x, 4 = 3.0x.
// Returns the multiplier in half-steps (2 = 1.0x ... 6 = 3.0x). An unset or out-of-range setting uses the default choice.
static inline int hard_mode_halves(const char* cvar, int defaultChoice) {
    int choice = CVarGetInteger(cvar, defaultChoice);

    if (choice < 0 || choice > 4) {
        choice = defaultChoice;
    }
    return choice + 2;
}

#endif
