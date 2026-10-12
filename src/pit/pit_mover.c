#include "common.h"
#include "message_ids.h"
#include "map.h"
#include "inventory.h"
#include "pit_of_trials.h"
#include "pit_text.h"
#include <libultraship/bridge/consolevariablebridge.h>

#include "sprite/npc/DisguisedMoustafa.h"

// The Mover (Sheek, Moustafa's disguise): now and then he is on a Pit floor and moves Mario ahead for coins.
// On a battle floor he takes the place of the enemy, so that floor has nothing to fight.

#define NAMESPACE pit_mover

// He only shows up on floors up to this one.
#define PIT_MOVER_LAST_FLOOR 90
// What he offers: how many floors he skips, and the price on floor 1.
#define PIT_MOVER_OFFERS 2
// Percent of the base price added for each floor after the first (floor 90 costs about 5.5 times as much).
#define PIT_MOVER_PRICE_PER_FLOOR 5

enum {
    NPC_Mover = 2,
};

enum {
    MOVER_RESULT_LEAVE    = 0,
    MOVER_RESULT_PAID     = 1,
    MOVER_RESULT_NO_COINS = 2,
};

static const s32 N(Skips)[PIT_MOVER_OFFERS] = { 3, 5 };
static const s32 N(BasePrices)[PIT_MOVER_OFFERS] = { 30, 50 };

// Today's prices, and how many floors the last paid skip covers.
static s32 sPrice[PIT_MOVER_OFFERS];
static s32 sSkipped;

static u8 N(GreetingMsg)[128];
static u8 N(NoCoinsMsg)[64];
static u8 N(ByeMsg)[64];
static u8 N(GoMsg)[96];
static u8 N(MenuMsg)[160];

static s32 N(PriceOnFloor)(s32 base, s32 floor) {
    s32 price = (base * (100 + (floor - 1) * PIT_MOVER_PRICE_PER_FLOOR)) / 100;

    if (price < 1) {
        price = 1;
    }
    if (price > 999) {
        price = 999;
    }
    return price;
}

// Decides if the Mover is here on this floor (LVar0 = true or false), and sets up the prices and messages.
API_CALLABLE(N(Roll)) {
    u8* dst = N(MenuMsg);
    s32 floor = pit_current_floor();
    s32 chance = CVarGetInteger(CVAR_PIT_MOVER_CHANCE, PIT_MOVER_DEFAULT_CHANCE); // from the Pit menu
    s32 pos = 0;
    s32 i;

    if (chance > PIT_MOVER_MAX_CHANCE) {
        chance = PIT_MOVER_MAX_CHANCE;
    }
    if (floor > PIT_MOVER_LAST_FLOOR || rand_int(99) >= chance) {
        script->varTable[0] = false;
        return ApiStatus_DONE2;
    }

    for (i = 0; i < PIT_MOVER_OFFERS; i++) {
        sPrice[i] = N(PriceOnFloor)(N(BasePrices)[i], floor);
    }

    pit_text_speech(N(GreetingMsg), "Psst. Need to get ahead?\nI can move you for a fee.", false);
    pit_text_speech(N(NoCoinsMsg), "You cannot afford that.", false);
    pit_text_speech(N(ByeMsg), "Maybe next time.", true);

    // The choice menu: one line for each skip with its price, then "No thanks".
    dst[pos++] = MSG_CHAR_READ_STYLE;
    dst[pos++] = MSG_STYLE_CHOICE;
    dst[pos++] = 100;
    dst[pos++] = 60;
    dst[pos++] = 200;
    dst[pos++] = 16 * (PIT_MOVER_OFFERS + 1) + 14;

    for (i = 0; i < PIT_MOVER_OFFERS; i++) {
        char digits[8];

        dst[pos++] = MSG_CHAR_READ_FUNCTION;
        dst[pos++] = MSG_READ_FUNC_CURSOR;
        dst[pos++] = i;
        dst[pos++] = MSG_CHAR_READ_FUNCTION;
        dst[pos++] = MSG_READ_FUNC_OPTION;
        dst[pos++] = i;
        pos = pit_text_put(dst, pos, "Skip ");
        int_to_string(N(Skips)[i], digits, 10);
        pos = pit_text_put(dst, pos, digits);
        pos = pit_text_put(dst, pos, " floors: ");
        int_to_string(sPrice[i], digits, 10);
        pos = pit_text_put(dst, pos, digits);
        dst[pos++] = MSG_CHAR_READ_FUNCTION;
        dst[pos++] = MSG_READ_FUNC_OPTION;
        dst[pos++] = 0xFF;
        dst[pos++] = MSG_CHAR_READ_ENDL;
    }

    dst[pos++] = MSG_CHAR_READ_FUNCTION;
    dst[pos++] = MSG_READ_FUNC_CURSOR;
    dst[pos++] = PIT_MOVER_OFFERS;
    dst[pos++] = MSG_CHAR_READ_FUNCTION;
    dst[pos++] = MSG_READ_FUNC_OPTION;
    dst[pos++] = PIT_MOVER_OFFERS;
    pos = pit_text_put(dst, pos, "No thanks");
    dst[pos++] = MSG_CHAR_READ_FUNCTION;
    dst[pos++] = MSG_READ_FUNC_OPTION;
    dst[pos++] = 0xFF;

    // B leaves too.
    dst[pos++] = MSG_CHAR_READ_FUNCTION;
    dst[pos++] = MSG_READ_FUNC_SET_CANCEL;
    dst[pos++] = PIT_MOVER_OFFERS;
    dst[pos++] = MSG_CHAR_READ_FUNCTION;
    dst[pos++] = MSG_READ_FUNC_END_CHOICE;
    dst[pos++] = PIT_MOVER_OFFERS + 1;
    dst[pos++] = MSG_CHAR_READ_END;

    script->varTable[0] = true;
    return ApiStatus_DONE2;
}

// Takes the menu choice (LVar0). If Mario can pay, takes the coins and makes the "next stop" message.
// LVar0 = what happened (MOVER_RESULT_...).
API_CALLABLE(N(ResolveChoice)) {
    s32 choice = script->varTable[0];
    s32 dest;
    s32 pos;
    char digits[8];

    if (choice < 0 || choice >= PIT_MOVER_OFFERS) {
        script->varTable[0] = MOVER_RESULT_LEAVE;
        return ApiStatus_DONE2;
    }
    if (gPlayerData.coins < sPrice[choice]) {
        script->varTable[0] = MOVER_RESULT_NO_COINS;
        return ApiStatus_DONE2;
    }

    gPlayerData.coins -= sPrice[choice];
    sSkipped = N(Skips)[choice];

    // Mario lands on the floor after the skipped ones (never on the boss floor).
    dest = pit_current_floor() + 1 + sSkipped;
    if (dest > 99) {
        dest = 99;
    }
    int_to_string(dest, digits, 10);
    pos = 0;
    N(GoMsg)[pos++] = MSG_CHAR_READ_STYLE;
    N(GoMsg)[pos++] = MSG_STYLE_RIGHT;
    pos = pit_text_put(N(GoMsg), pos, "Hold tight. Next stop: floor ");
    pos = pit_text_put(N(GoMsg), pos, digits);
    pos = pit_text_put(N(GoMsg), pos, ".");
    N(GoMsg)[pos++] = MSG_CHAR_READ_WAIT;
    N(GoMsg)[pos++] = MSG_CHAR_READ_END;

    script->varTable[0] = MOVER_RESULT_PAID;
    return ApiStatus_DONE2;
}

API_CALLABLE(N(RequestSkip)) {
    pit_request_skip_floors(sSkipped);
    return ApiStatus_DONE2;
}

NpcSettings N(NpcSettings_Mover) = {
    .height = 30,
    .radius = 24,
    .level = ACTOR_LEVEL_NONE,
};

// Same talk as the shop: the speech window stays open (yield) while the menu is up, EndSpeech closes it after.
EvtScript N(EVS_NpcInteract_Mover) = {
    Call(SpeakToPlayer, NPC_Mover, ANIM_DisguisedMoustafa_Talk, ANIM_DisguisedMoustafa_Idle, 0, Ref(N(GreetingMsg)))
    Label(0)
    Call(ShowCoinCounter, true)
    Call(ShowChoice, Ref(N(MenuMsg)))
    Call(N(ResolveChoice))
    Call(ShowCoinCounter, false)
    Call(EndSpeech, NPC_Mover, ANIM_DisguisedMoustafa_Talk, ANIM_DisguisedMoustafa_Idle, 0)
    Switch(LVar0)
        CaseEq(MOVER_RESULT_PAID)
            Call(SpeakToPlayer, NPC_Mover, ANIM_DisguisedMoustafa_Talk, ANIM_DisguisedMoustafa_Idle, 0, Ref(N(GoMsg)))
            Call(N(RequestSkip))
            Wait(100)
            Return
        CaseEq(MOVER_RESULT_NO_COINS)
            Call(SpeakToPlayer, NPC_Mover, ANIM_DisguisedMoustafa_Talk, ANIM_DisguisedMoustafa_Idle, 0, Ref(N(NoCoinsMsg)))
        CaseEq(MOVER_RESULT_LEAVE)
            Call(SpeakToPlayer, NPC_Mover, ANIM_DisguisedMoustafa_Talk, ANIM_DisguisedMoustafa_Idle, 0, Ref(N(ByeMsg)))
            Return
    EndSwitch
    // Back to the menu (Goto is not used inside the Switch, it would skip EndSwitch).
    Goto(0)
    Return
    End
};

EvtScript N(EVS_NpcInit_Mover) = {
    Call(BindNpcInteract, NPC_SELF, Ref(N(EVS_NpcInteract_Mover)))
    Return
    End
};

// Where he stands: between the chest and the sign in the rest room, and where the enemy would be in a battle room.
NpcData N(NpcData_Mover) = {
    .id = NPC_Mover,
    .pos = { -40.0f, -10.0f, 0.0f },
    .yaw = 90,
    .init = &N(EVS_NpcInit_Mover),
    .settings = &N(NpcSettings_Mover),
    .flags = BASE_PASSIVE_FLAGS,
    .drops = NO_DROPS,
    .animations = {
        .idle   = ANIM_DisguisedMoustafa_Idle,
        .walk   = ANIM_DisguisedMoustafa_Idle,
        .run    = ANIM_DisguisedMoustafa_Idle,
        .chase  = ANIM_DisguisedMoustafa_Idle,
        .anim_4 = ANIM_DisguisedMoustafa_Idle,
        .anim_5 = ANIM_DisguisedMoustafa_Idle,
        .death  = ANIM_DisguisedMoustafa_Idle,
        .hit    = ANIM_DisguisedMoustafa_Idle,
        .anim_8 = ANIM_DisguisedMoustafa_Idle,
        .anim_9 = ANIM_DisguisedMoustafa_Idle,
        .anim_A = ANIM_DisguisedMoustafa_Idle,
        .anim_B = ANIM_DisguisedMoustafa_Idle,
        .anim_C = ANIM_DisguisedMoustafa_Idle,
        .anim_D = ANIM_DisguisedMoustafa_Idle,
        .anim_E = ANIM_DisguisedMoustafa_Idle,
        .anim_F = ANIM_DisguisedMoustafa_Idle,
    },
    .tattle = MSG_NpcTattle_Sheek,
};

// Used by the battle room and the rest room: call pit_mover_Roll first, and make these NPCs if it says he is here.
NpcGroupList N(NPCs) = {
    NPC_GROUP(N(NpcData_Mover), BTL_KMR_1_FORMATION_06),
    {}
};

// The rest room, when both the shop and the Mover are here. One list: making NPCs twice would lose the first group.
extern NpcData pit_shop_NpcData_RipCheato; // src/pit/pit_shop.c

NpcGroupList N(WithShopNPCs) = {
    NPC_GROUP(pit_shop_NpcData_RipCheato, BTL_KMR_1_FORMATION_06),
    NPC_GROUP(N(NpcData_Mover), BTL_KMR_1_FORMATION_06),
    {}
};
