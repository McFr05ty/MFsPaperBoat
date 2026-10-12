#include "common.h"
#include "message_ids.h"
#include "map.h"
#include "inventory.h"
#include "pit_of_trials.h"
#include "pit_text.h"
#include <libultraship/bridge/consolevariablebridge.h>

#include "sprite/npc/RipCheato.h"

// The Pit shop: on some rest floors Rip Cheato sets up shop. He sells a few random items. The prices rise with
// the floor. Everything is done with the game's own dialogue boxes and choice menu.

#define NAMESPACE pit_shop

#include "world/common/complete/GiveReward.inc.c"

// How many items are for sale on one visit.
#define PIT_SHOP_STOCK 3
// Percent of the base price added for each floor (floor 10 costs +50%, floor 90 costs +450%).
#define PIT_SHOP_PRICE_PER_FLOOR 5

enum {
    NPC_RipCheato = 1,
};

enum {
    SHOP_RESULT_LEAVE    = 0,
    SHOP_RESULT_BOUGHT   = 1,
    SHOP_RESULT_NO_COINS = 2,
    SHOP_RESULT_NO_ROOM  = 3,
};

// The items he can sell (the price starts from the item's normal shop price).
static const s16 N(Pool)[] = {
    ITEM_MUSHROOM,
    ITEM_SUPER_SHROOM,
    ITEM_ULTRA_SHROOM,
    ITEM_LIFE_SHROOM,
    ITEM_DRIED_SHROOM,
    ITEM_FIRE_FLOWER,
    ITEM_THUNDER_RAGE,
    ITEM_THUNDER_BOLT,
    ITEM_SHOOTING_STAR,
    ITEM_POW_BLOCK,
    ITEM_STOP_WATCH,
    ITEM_DIZZY_DIAL,
    ITEM_SLEEPY_SHEEP,
    ITEM_SUPER_SODA,
    ITEM_TASTY_TONIC,
    ITEM_MAPLE_SYRUP,
    ITEM_HONEY_SYRUP,
    ITEM_JAMMIN_JELLY,
};

// Today's stock.
static s32 sStockItem[PIT_SHOP_STOCK];
static s32 sStockPrice[PIT_SHOP_STOCK];
static s32 sStockSold[PIT_SHOP_STOCK];

// What the choice menu currently shows: menu line -> stock slot.
static s32 sMenuMap[PIT_SHOP_STOCK];
static s32 sMenuCount;

// Messages are built from plain text when the shop is set up (the game's message codes are not ASCII).
static u8 N(GreetingMsg)[160];
static u8 N(SoldOutMsg)[96];
static u8 N(NoCoinsMsg)[96];
static u8 N(NoRoomMsg)[96];
static u8 N(ThanksMsg)[96];
static u8 N(ByeMsg)[96];
static u8 N(MenuMsg)[256];

// Decides if the shop is here on this rest floor (LVar0 = true or false), and sets up the stock and the messages.
API_CALLABLE(N(Roll)) {
    s32 poolSize = 0;
    s32 order[ARRAY_COUNT(N(Pool))];
    s32 floor = pit_current_floor();
    s32 chance = CVarGetInteger(CVAR_PIT_SHOP_CHANCE, PIT_SHOP_DEFAULT_CHANCE); // 0-100, from the Pit menu
    s32 i, j, tmp;

    if (rand_int(99) >= chance) {
        script->varTable[0] = false;
        return ApiStatus_DONE2;
    }

    // Only items that have a name can be sold. A few unused items have no text, which shows up as a blank name.
    for (i = 0; i < ARRAY_COUNT(N(Pool)); i++) {
        if (gItemTable[N(Pool)[i]].nameMsg != MSG_NONE) {
            order[poolSize++] = i;
        }
    }
    for (i = 0; i < PIT_SHOP_STOCK; i++) {
        s32 base;
        s32 price;

        j = i + rand_int(poolSize - 1 - i);
        tmp = order[i];
        order[i] = order[j];
        order[j] = tmp;

        sStockItem[i] = N(Pool)[order[i]];
        base = gItemTable[sStockItem[i]].sellValue;
        if (base < 1) {
            base = 10;
        }
        price = (base * (100 + floor * PIT_SHOP_PRICE_PER_FLOOR)) / 100;
        if (price < 1) {
            price = 1;
        }
        if (price > 999) {
            price = 999;
        }
        sStockPrice[i] = price;
        sStockSold[i] = false;
    }

    pit_text_speech(N(GreetingMsg), "Psst. Care to buy some goods?\nPrices rise with each floor.", false);
    pit_text_speech(N(SoldOutMsg), "I am all sold out for now.", true);
    pit_text_speech(N(NoCoinsMsg), "You cannot afford that.", false);
    pit_text_speech(N(NoRoomMsg), "Your pockets are full.", false);
    pit_text_speech(N(ThanksMsg), "A pleasure doing business.", false);
    pit_text_speech(N(ByeMsg), "Come back any time.", true);

    script->varTable[0] = true;
    return ApiStatus_DONE2;
}

// LVar0 = how many items are still for sale.
API_CALLABLE(N(CountStock)) {
    s32 count = 0;
    s32 i;

    for (i = 0; i < PIT_SHOP_STOCK; i++) {
        if (!sStockSold[i]) {
            count++;
        }
    }
    script->varTable[0] = count;
    return ApiStatus_DONE2;
}

// Builds the choice menu: one line per item still for sale ("Name: price"), then "Leave".
// LVar0 = the number of items on the menu.
API_CALLABLE(N(BuildMenu)) {
    u8* dst = N(MenuMsg);
    s32 pos = 0;
    s32 lines;
    s32 i;

    sMenuCount = 0;
    for (i = 0; i < PIT_SHOP_STOCK; i++) {
        if (!sStockSold[i]) {
            sMenuMap[sMenuCount++] = i;
        }
    }
    lines = sMenuCount + 1;

    // Choice window: x, y, width, height.
    dst[pos++] = MSG_CHAR_READ_STYLE;
    dst[pos++] = MSG_STYLE_CHOICE;
    dst[pos++] = 100;
    dst[pos++] = 60;
    dst[pos++] = 200;
    dst[pos++] = 16 * lines + 14;

    for (i = 0; i < sMenuCount; i++) {
        char digits[8];
        s32 slot = sMenuMap[i];

        // The item name goes in as a message variable (variables 0-2).
        set_message_text_var(gItemTable[sStockItem[slot]].nameMsg, i);

        dst[pos++] = MSG_CHAR_READ_FUNCTION;
        dst[pos++] = MSG_READ_FUNC_CURSOR;
        dst[pos++] = i;
        dst[pos++] = MSG_CHAR_READ_FUNCTION;
        dst[pos++] = MSG_READ_FUNC_OPTION;
        dst[pos++] = i;
        dst[pos++] = MSG_CHAR_READ_FUNCTION;
        dst[pos++] = MSG_READ_FUNC_VAR;
        dst[pos++] = i;
        int_to_string(sStockPrice[slot], digits, 10);
        pos = pit_text_put(dst, pos, ": ");
        pos = pit_text_put(dst, pos, digits);
        dst[pos++] = MSG_CHAR_READ_FUNCTION;
        dst[pos++] = MSG_READ_FUNC_OPTION;
        dst[pos++] = 0xFF;
        dst[pos++] = MSG_CHAR_READ_ENDL;
    }

    dst[pos++] = MSG_CHAR_READ_FUNCTION;
    dst[pos++] = MSG_READ_FUNC_CURSOR;
    dst[pos++] = sMenuCount;
    dst[pos++] = MSG_CHAR_READ_FUNCTION;
    dst[pos++] = MSG_READ_FUNC_OPTION;
    dst[pos++] = sMenuCount;
    pos = pit_text_put(dst, pos, "Leave");
    dst[pos++] = MSG_CHAR_READ_FUNCTION;
    dst[pos++] = MSG_READ_FUNC_OPTION;
    dst[pos++] = 0xFF;

    // B leaves too.
    dst[pos++] = MSG_CHAR_READ_FUNCTION;
    dst[pos++] = MSG_READ_FUNC_SET_CANCEL;
    dst[pos++] = sMenuCount;
    dst[pos++] = MSG_CHAR_READ_FUNCTION;
    dst[pos++] = MSG_READ_FUNC_END_CHOICE;
    dst[pos++] = lines;
    dst[pos++] = MSG_CHAR_READ_END;

    script->varTable[0] = sMenuCount;
    return ApiStatus_DONE2;
}

// Takes the menu choice (LVar0) and, if Mario can pay and carry it, pays and marks the item sold.
// LVar0 = what happened (SHOP_RESULT_...), LVarB = the item bought.
API_CALLABLE(N(ResolveChoice)) {
    s32 choice = script->varTable[0];
    s32 slot;

    if (choice < 0 || choice >= sMenuCount) {
        script->varTable[0] = SHOP_RESULT_LEAVE;
        return ApiStatus_DONE2;
    }

    slot = sMenuMap[choice];
    if (gPlayerData.coins < sStockPrice[slot]) {
        script->varTable[0] = SHOP_RESULT_NO_COINS;
    } else if (get_consumables_empty() <= 0) {
        script->varTable[0] = SHOP_RESULT_NO_ROOM;
    } else {
        gPlayerData.coins -= sStockPrice[slot];
        sStockSold[slot] = true;
        script->varTable[11] = sStockItem[slot];
        script->varTable[0] = SHOP_RESULT_BOUGHT;
    }
    return ApiStatus_DONE2;
}

NpcSettings N(NpcSettings_RipCheato) = {
    .height = 30,
    .radius = 24,
    .level = ACTOR_LEVEL_NONE,
};

// How the talk goes: a speech window that stays open (yield) while the choice menu is up. After the menu, the speech
// window is closed (EndSpeech) and the next line is a new speech. Lines that lead back to the menu yield again.
EvtScript N(EVS_NpcInteract_RipCheato) = {
    Call(N(CountStock))
    IfEq(LVar0, 0)
        Call(SpeakToPlayer, NPC_RipCheato, ANIM_RipCheato_Talk, ANIM_RipCheato_Idle, 0, Ref(N(SoldOutMsg)))
        Return
    EndIf
    Call(SpeakToPlayer, NPC_RipCheato, ANIM_RipCheato_Talk, ANIM_RipCheato_Idle, 0, Ref(N(GreetingMsg)))
    Label(0)
    Call(ShowCoinCounter, true)
    Call(N(BuildMenu))
    Call(ShowChoice, Ref(N(MenuMsg)))
    Call(N(ResolveChoice))
    Call(ShowCoinCounter, false)
    Call(EndSpeech, NPC_RipCheato, ANIM_RipCheato_Talk, ANIM_RipCheato_Idle, 0)
    Switch(LVar0)
        CaseEq(SHOP_RESULT_BOUGHT)
            EVT_GIVE_REWARD(LVarB)
            Call(N(CountStock))
            IfEq(LVar0, 0)
                Call(SpeakToPlayer, NPC_RipCheato, ANIM_RipCheato_Talk, ANIM_RipCheato_Idle, 0, Ref(N(SoldOutMsg)))
                Return
            EndIf
            Call(SpeakToPlayer, NPC_RipCheato, ANIM_RipCheato_Talk, ANIM_RipCheato_Idle, 0, Ref(N(ThanksMsg)))
        CaseEq(SHOP_RESULT_NO_COINS)
            Call(SpeakToPlayer, NPC_RipCheato, ANIM_RipCheato_Talk, ANIM_RipCheato_Idle, 0, Ref(N(NoCoinsMsg)))
        CaseEq(SHOP_RESULT_NO_ROOM)
            Call(SpeakToPlayer, NPC_RipCheato, ANIM_RipCheato_Talk, ANIM_RipCheato_Idle, 0, Ref(N(NoRoomMsg)))
        CaseEq(SHOP_RESULT_LEAVE)
            Call(SpeakToPlayer, NPC_RipCheato, ANIM_RipCheato_Talk, ANIM_RipCheato_Idle, 0, Ref(N(ByeMsg)))
            Return
    EndSwitch
    // Back to the menu (Goto is not used inside the Switch, it would skip EndSwitch).
    Goto(0)
    Return
    End
};

EvtScript N(EVS_NpcInit_RipCheato) = {
    Call(BindNpcInteract, NPC_SELF, Ref(N(EVS_NpcInteract_RipCheato)))
    Return
    End
};

NpcData N(NpcData_RipCheato) = {
    .id = NPC_RipCheato,
    .pos = { 210.0f, -10.0f, -30.0f },
    .yaw = 270,
    .init = &N(EVS_NpcInit_RipCheato),
    .settings = &N(NpcSettings_RipCheato),
    .flags = BASE_PASSIVE_FLAGS,
    .drops = NO_DROPS,
    .animations = {
        .idle   = ANIM_RipCheato_Idle,
        .walk   = ANIM_RipCheato_Idle,
        .run    = ANIM_RipCheato_Idle,
        .chase  = ANIM_RipCheato_Idle,
        .anim_4 = ANIM_RipCheato_Idle,
        .anim_5 = ANIM_RipCheato_Idle,
        .death  = ANIM_RipCheato_Idle,
        .hit    = ANIM_RipCheato_Idle,
        .anim_8 = ANIM_RipCheato_Idle,
        .anim_9 = ANIM_RipCheato_Idle,
        .anim_A = ANIM_RipCheato_Idle,
        .anim_B = ANIM_RipCheato_Idle,
        .anim_C = ANIM_RipCheato_Idle,
        .anim_D = ANIM_RipCheato_Idle,
        .anim_E = ANIM_RipCheato_Idle,
        .anim_F = ANIM_RipCheato_Idle,
    },
    .tattle = MSG_NpcTattle_RipCheato,
};

// Used by the rest room: call pit_shop_Roll first, and make these NPCs if it says the shop is here.
NpcGroupList N(NPCs) = {
    NPC_GROUP(N(NpcData_RipCheato), BTL_KMR_1_FORMATION_06),
    {}
};
