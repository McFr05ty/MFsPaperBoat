#include "port/ShipInit.hpp"
#include "port/Engine.h"
#include "port/hooks/Events.h"
#include "port/ui/cvar_prefixes.h"

#include "common.h"
#include "rare_enemy.h"

// Rare Enemies: a soft color tint multiplied over a sprite. The game draws a rare enemy normally, so texture
// packs show, between rare_tint_begin() and rare_tint_end(). While a tint is active, every sprite component
// drawn gets a combiner that multiplies its texture by the tint color.
static int sTintActive = 0;
static int sTintR = 255;
static int sTintG = 255;
static int sTintB = 255;

extern "C" void rare_tint_begin(int r, int g, int b) {
    sTintR = r < 0 ? 0 : (r > 255 ? 255 : r);
    sTintG = g < 0 ? 0 : (g > 255 ? 255 : g);
    sTintB = b < 0 ? 0 : (b > 255 ? 255 : b);
    sTintActive = 1;
}

extern "C" void rare_tint_end(void) {
    sTintActive = 0;
}

void RegisterRareEnemyTint_Init() {
    REGISTER_LISTENER(SpriteComponentPreDraw, EVENT_PRIORITY_NORMAL, [](IEvent* event) {
        if (!sTintActive) {
            return;
        }

        gDPSetCombineMode(gMainGfxPos++, PM_CC_2D, PM_CC_2D);
        gDPSetPrimColor(gMainGfxPos++, 0, 0, sTintR, sTintG, sTintB, 255);
    });
}

static RegisterShipInitFunc initRareEnemyTintFunc(RegisterRareEnemyTint_Init);
