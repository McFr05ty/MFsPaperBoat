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

// Maps with sprite shading draw sprites with their own two-step color mode, which the tint above cannot
// override. That mode multiplies the sprite by (vertex shade - env color + 1), so the env color is the
// one thing we can change to tint it: a lower env color brightens a channel, a higher one darkens it.
// With no tint active this sets the normal env color (100, 100, 100).
extern "C" void rare_set_shading_env(void) {
    int r = 100;
    int g = 100;
    int b = 100;

    if (sTintActive) {
        // Stretch the soft tint (which only goes down to 255 - RARE_TINT_STRENGTH) back to the full 0-255 range.
        int tr = 255 - ((255 - sTintR) * 100) / RARE_TINT_STRENGTH;
        int tg = 255 - ((255 - sTintG) * 100) / RARE_TINT_STRENGTH;
        int tb = 255 - ((255 - sTintB) * 100) / RARE_TINT_STRENGTH;
        tr = tr < 0 ? 0 : tr;
        tg = tg < 0 ? 0 : tg;
        tb = tb < 0 ? 0 : tb;
        r = 255 - (155 * tr) / 255;
        g = 255 - (155 * tg) / 255;
        b = 255 - (155 * tb) / 255;
    }
    gDPSetEnvColor(gMainGfxPos++, r, g, b, 255);
}

// Multiplies the next sprite piece by the tint. Small pieces are drawn by sprite.c and large ones by imgfx.c,
// which sets its own color mode, so imgfx.c calls this after it sets the plain one.
extern "C" void rare_tint_reapply(void) {
    if (!sTintActive) {
        return;
    }

    gDPSetCombineMode(gMainGfxPos++, PM_CC_2D, PM_CC_2D);
    gDPSetPrimColor(gMainGfxPos++, 0, 0, sTintR, sTintG, sTintB, 255);
}

void RegisterRareEnemyTint_Init() {
    REGISTER_LISTENER(SpriteComponentPreDraw, EVENT_PRIORITY_NORMAL, [](IEvent* event) {
        rare_tint_reapply();
    });
}

static RegisterShipInitFunc initRareEnemyTintFunc(RegisterRareEnemyTint_Init);
