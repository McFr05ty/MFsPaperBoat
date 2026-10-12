#ifndef PIT_TEXT_H
#define PIT_TEXT_H

#include "common.h"

// Helpers for the Pit NPCs (the shop and the Mover): they build their speech and menus in memory from plain text,
// because the game's message codes are not ASCII.

// Writes text (letters, digits, space . ? ! : - ' and \n) as message codes at dst[pos]. Returns the new write position.
s32 pit_text_put(u8* dst, s32 pos, const char* text);

// Builds a whole speech message in dst. The text is shown, then:
// - isLast: wait for the button, and the window closes.
// - not last: the window stays open (yield), so a choice menu can show up next to it. EndSpeech closes it later.
void pit_text_speech(u8* dst, const char* text, bool isLast);

#endif
