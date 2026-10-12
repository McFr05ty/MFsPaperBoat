#ifndef STRANGE_SACK_H
#define STRANGE_SACK_H

// Strange Sack: the key item Mario gets on floor 40 of the Pit of Trials. While he owns it he can carry 20 consumables
// instead of 10 (see get_inventory_capacity in src/inventory.c). It is the unused third prison key (item 0x07F) with a new
// name, description and icon (the mailbag).
#define STRANGE_SACK_ITEM ITEM_UNUSED_PRISON_KEY_3

// The item's name, short description and full description. The game has no text for them, so they are made up in memory
// (see strange_sack_message). They are section 0xF0 of the message table, which is not used, and they are below the range
// the game treats as memory addresses. (The Holofoil Hero badge has 0x00F00001 to 0x00F00003.)
#define STRANGE_SACK_MSG_NAME  0x00F00004
#define STRANGE_SACK_MSG_SHORT 0x00F00005
#define STRANGE_SACK_MSG_FULL  0x00F00006

#ifdef __cplusplus
extern "C" {
#endif

// The text for one of the three message IDs above, or NULL for any other message. The message loader asks this for every
// message. (src/strange_sack.c)
unsigned char* strange_sack_message(unsigned int msgID);

#ifdef __cplusplus
}
#endif

#endif
