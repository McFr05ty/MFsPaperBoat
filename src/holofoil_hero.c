#include "common.h"
#include "holofoil_hero.h"
#include "pit_text.h"

// The new name and descriptions of the Kaiden badge, built from plain text the first time they are asked for.

static u8 sName[32]; // the game copies at most 32 bytes of a name into a message
static u8 sShortDesc[96];
static u8 sFullDesc[160];
static b32 sBuilt = false;

static void holofoil_hero_build(void) {
    s32 pos;

    pos = pit_text_put(sName, 0, "Holofoil Hero");
    sName[pos] = MSG_CHAR_READ_END;

    pos = pit_text_put(sShortDesc, 0, "Mario shines like a rare foe.");
    sShortDesc[pos] = MSG_CHAR_READ_END;

    pos = pit_text_put(sFullDesc, 0, "Gives Mario the holographic shine\nof a rare enemy.");
    sFullDesc[pos] = MSG_CHAR_READ_END;

    sBuilt = true;
}

unsigned char* holofoil_hero_message(unsigned int msgID) {
    if (msgID != HOLOFOIL_HERO_MSG_NAME && msgID != HOLOFOIL_HERO_MSG_SHORT && msgID != HOLOFOIL_HERO_MSG_FULL) {
        return nullptr;
    }

    if (!sBuilt) {
        holofoil_hero_build();
    }

    if (msgID == HOLOFOIL_HERO_MSG_NAME) {
        return sName;
    }
    if (msgID == HOLOFOIL_HERO_MSG_SHORT) {
        return sShortDesc;
    }
    return sFullDesc;
}
