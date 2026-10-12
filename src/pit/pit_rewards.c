#include "common.h"
#include "inventory.h"
#include "strange_sack.h"
#include "pit_of_trials.h"

// The reward chests in the Pit's rest rooms (floors 10, 20 ... 90). Every rest floor has one fixed reward. It is paid out
// the first time the chest is opened on a save file (the save remembers it in gPlayerData.pitRewardsClaimed). After that
// the chest only holds some coins.

#define FIRST_REWARD_FLOOR 10
#define REWARD_FLOOR_STEP 10
#define REWARD_COUNT 9

// Most of these are the spare copies of badges (the X ones) that the game never hands out, so they can not clash with a
// badge Mario already found in the story.
static const s16 sRewards[REWARD_COUNT] = {
    ITEM_HP_PLUS_X,         // floor 10
    ITEM_HAPPY_HEART_X,     // floor 20
    ITEM_FP_PLUS_X,         // floor 30
    STRANGE_SACK_ITEM,      // floor 40
    ITEM_DEFEND_PLUS_X,     // floor 50
    ITEM_POWER_PLUS_X,      // floor 60
    ITEM_DAMAGE_DODGE_X,    // floor 70
    ITEM_HAPPY_FLOWER_X,    // floor 80
    ITEM_FLOWER_SAVER_X,    // floor 90
};

// Which reward a floor has (0 to REWARD_COUNT - 1), or -1 if it is not a rest floor.
static s32 reward_index(s32 floor) {
    s32 offset = floor - FIRST_REWARD_FLOOR;

    if (offset < 0 || offset % REWARD_FLOOR_STEP != 0 || offset / REWARD_FLOOR_STEP >= REWARD_COUNT) {
        return -1;
    }
    return offset / REWARD_FLOOR_STEP;
}

int pit_reward_take(int floor) {
    s32 idx = reward_index(floor);
    s32 itemID;

    if (idx < 0 || (gPlayerData.pitRewardsClaimed & (1 << idx))) {
        return ITEM_NONE;
    }

    gPlayerData.pitRewardsClaimed |= (1 << idx);
    itemID = sRewards[idx];

    // A key item can only be had once, so a second copy would be wasted.
    if (item_is_key(itemID) && has_item(itemID)) {
        return ITEM_NONE;
    }
    return itemID;
}

int pit_reward_consolation(int floor) {
    return floor;
}

void pit_reward_reset(void) {
    gPlayerData.pitRewardsClaimed = 0;
}
