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

// Picks Mario still needs: each level-up raises one stat, HP and FP by 5 and BP by 3, up to 99.
static inline int hlc_picks_left(int hp, int fp, int bp) {
    int n = 0;

    if (hp < 99) {
        n += (99 - hp + 4) / 5;
    }
    if (fp < 99) {
        n += (99 - fp + 4) / 5;
    }
    if (bp < 99) {
        n += (99 - bp + 2) / 3;
    }
    return n;
}

// Is Mario at the level cap? Normally level 27. With the toggle on: all of HP, FP and BP are at 99.
static inline int hlc_level_capped(int level, int hp, int fp, int bp) {
    if (hlc_on()) {
        return hlc_picks_left(hp, fp, bp) == 0;
    }
    return level >= 27;
}

// Will the pending level-up(s) reach the cap? Normally the original "level + pending == 27" test.
static inline int hlc_cap_reached(int level, int pending, int hp, int fp, int bp) {
    if (hlc_on()) {
        return pending >= hlc_picks_left(hp, fp, bp);
    }
    return level + pending == 27;
}

// Minimum star points per defeated enemy while the toggle is on and Mario is below the cap.
static inline int hlc_star_floor(int enemyLevel) {
    if (enemyLevel <= 5) {
        return 1;
    }
    if (enemyLevel <= 15) {
        return 2;
    }
    if (enemyLevel <= 20) {
        return 3;
    }
    return 4;
}

#endif
