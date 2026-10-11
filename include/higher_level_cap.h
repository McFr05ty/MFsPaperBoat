#ifndef HIGHER_LEVEL_CAP_H
#define HIGHER_LEVEL_CAP_H

#include "port/ui/cvar_prefixes.h"
#include <libultraship/bridge/consolevariablebridge.h>

#define CVAR_HIGHER_LEVEL_CAP CVAR_ENHANCEMENT("HigherLevelCap")

// Mario's permanent stat limits: HP 50, FP 50, BP 30 normally, 99 each with the toggle on.
static inline int hlc_on(void) {
    return CVarGetInteger(CVAR_HIGHER_LEVEL_CAP, 0) != 0;
}

static inline int hlc_hp_cap(void) {
    return hlc_on() ? 99 : 50;
}

static inline int hlc_fp_cap(void) {
    return hlc_on() ? 99 : 50;
}

static inline int hlc_bp_cap(void) {
    return hlc_on() ? 99 : 30;
}

// Largest current max HP or FP, which includes the HP Plus and FP Plus bonus. A stat already above the
// normal limit keeps the wide limit even if the toggle is off, so turning it off never takes stats away.
static inline int hlc_cur_cap(int hardMax) {
    return (hlc_on() || hardMax > 50) ? 99 : 75;
}

// The level-up screen draws a tens digit and a ones digit, so it cannot show more than 99.
static inline int hlc_clamp99(int value) {
    return value > 99 ? 99 : value;
}

#endif
