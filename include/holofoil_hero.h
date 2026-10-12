#ifndef HOLOFOIL_HERO_H
#define HOLOFOIL_HERO_H

// Holofoil Hero: the badge Mario gets for beating the Pit of Trials boss. It is the unused Kaiden badge (item 0x12D) with
// a new name and description. It keeps Kaiden's icon and move data, which costs 0 BP. While it is equipped, Mario is
// drawn with the shine rare enemies have.
#define HOLOFOIL_HERO_ITEM ITEM_KAIDEN

// Kaiden's own message IDs are placeholders (0), so the badge and its move use these instead. They are section 0xF0 of
// the message table, which the game does not use, and they are below the range the game treats as memory addresses.
// holofoil_hero_message() supplies their text.
#define HOLOFOIL_HERO_MSG_NAME  0x00F00001
#define HOLOFOIL_HERO_MSG_SHORT 0x00F00002
#define HOLOFOIL_HERO_MSG_FULL  0x00F00003

#ifdef __cplusplus
extern "C" {
#endif

// If the badge is equipped, starts the rare enemy tint and returns 1. The caller draws Mario, then calls rare_tint_end().
// (src/npc.c)
int holofoil_hero_tint_begin(void);

// Ends the tint (src/port/enhancements/graphics/RareEnemyTint.cpp).
void rare_tint_end(void);

// The text that replaces the Kaiden badge's name, short description and full description, or NULL for any other
// message. The message loader asks this for every message. (src/holofoil_hero.c)
unsigned char* holofoil_hero_message(unsigned int msgID);

#ifdef __cplusplus
}
#endif

#endif
