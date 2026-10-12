#include "common.h"
#include "strange_sack.h"
#include "pit_text.h"

// The name and descriptions of the Strange Sack, built from plain text the first time they are asked for.

static u8 sName[32]; // the game copies at most 32 bytes of a name into a message
static u8 sShortDesc[96];
static u8 sFullDesc[160];
static b32 sBuilt = false;

static void strange_sack_build(void) {
    s32 pos;

    pos = pit_text_put(sName, 0, "Strange Sack");
    sName[pos] = MSG_CHAR_READ_END;

    pos = pit_text_put(sShortDesc, 0, "Mario can carry 20 items.");
    sShortDesc[pos] = MSG_CHAR_READ_END;

    pos = pit_text_put(sFullDesc, 0, "A sack that is bigger on the inside.\nMario can carry 20 items.");
    sFullDesc[pos] = MSG_CHAR_READ_END;

    sBuilt = true;
}

unsigned char* strange_sack_message(unsigned int msgID) {
    if (msgID != STRANGE_SACK_MSG_NAME && msgID != STRANGE_SACK_MSG_SHORT && msgID != STRANGE_SACK_MSG_FULL) {
        return nullptr;
    }

    if (!sBuilt) {
        strange_sack_build();
    }

    if (msgID == STRANGE_SACK_MSG_NAME) {
        return sName;
    }
    if (msgID == STRANGE_SACK_MSG_SHORT) {
        return sShortDesc;
    }
    return sFullDesc;
}
