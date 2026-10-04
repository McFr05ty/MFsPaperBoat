#include "port/hooks/Events.h"
#include "port/ui/cvar_prefixes.h"
#include "port/ShipInit.hpp"

extern "C" {
#include "dx/versioning.h"

extern SaveData gCurrentSaveFile;
}


#include "hard_mode.h"
#include <libultraship/bridge/consolevariablebridge.h>

extern "C" {
#include "item_enum.h"
}

// Hard Mode: a save file loaded while Hard Mode is on gets a Lucky Star key item if it does not have one.
// No notification is shown. If the key item list is full, nothing is added.
static void RegisterHardModeLuckyStar_Init() {
    REGISTER_LISTENER(OnPostSaveFileLoad, EVENT_PRIORITY_NORMAL, [](IEvent* event) {
        if (!CVarGetInteger(CVAR_HARD_MODE, 0)) {
            return;
        }

        // The Lucky Star is only a label: the game checks hasActionCommands to enable action commands and blocking.
        // Set both together, and only if the item can be given, so the two never get out of sync.
        int starSlot = -1;
        int emptySlot = -1;
        const int slotCount = (int) (sizeof(gCurrentSaveFile.player.keyItems) / sizeof(gCurrentSaveFile.player.keyItems[0]));
        for (int i = 0; i < slotCount; i++) {
            if (gCurrentSaveFile.player.keyItems[i] == ITEM_LUCKY_STAR) {
                starSlot = i;
                break;
            }
            if (gCurrentSaveFile.player.keyItems[i] == ITEM_NONE && emptySlot < 0) {
                emptySlot = i;
            }
        }

        if (starSlot < 0 && emptySlot >= 0) {
            gCurrentSaveFile.player.keyItems[emptySlot] = ITEM_LUCKY_STAR;
            starSlot = emptySlot;
        }

        if (starSlot >= 0) {
            gCurrentSaveFile.player.hasActionCommands = true;
        }
    });
}

static RegisterShipInitFunc initHardModeLuckyStarFunc(RegisterHardModeLuckyStar_Init);
