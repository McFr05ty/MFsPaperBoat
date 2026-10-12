#include "common.h"
#include "pit_of_trials.h"
#include "pit_text.h"
#include <libultraship/bridge/consolevariablebridge.h>

// The floor sign in the Pit rooms. Reading it shows which floor Mario is on.

#define NAMESPACE pit_sign

// Message text is made of character codes, not ASCII.
#define MSG_UC(c) (MSG_CHAR_UPPER_A + ((c) - 'A'))
#define MSG_LC(c) (MSG_CHAR_LOWER_A + ((c) - 'a'))

// "Pit of Trials" / "Floor [var 0] of 100". Message variable 0 is filled in with the floor before showing it.
static u8 N(SignMessage)[] = {
    MSG_CHAR_READ_STYLE, MSG_STYLE_SIGN,
    MSG_UC('P'), MSG_LC('i'), MSG_LC('t'), MSG_CHAR_READ_SPACE,
    MSG_LC('o'), MSG_LC('f'), MSG_CHAR_READ_SPACE,
    MSG_UC('T'), MSG_LC('r'), MSG_LC('i'), MSG_LC('a'), MSG_LC('l'), MSG_LC('s'),
    MSG_CHAR_READ_ENDL,
    MSG_UC('F'), MSG_LC('l'), MSG_LC('o'), MSG_LC('o'), MSG_LC('r'), MSG_CHAR_READ_SPACE,
    MSG_CHAR_READ_FUNCTION, MSG_READ_FUNC_VAR, 0,
    MSG_CHAR_READ_SPACE, MSG_LC('o'), MSG_LC('f'), MSG_CHAR_READ_SPACE,
    MSG_CHAR_DIGIT_1, MSG_CHAR_DIGIT_0, MSG_CHAR_DIGIT_0,
    MSG_CHAR_READ_WAIT,
    MSG_CHAR_READ_END,
};

// The sign next to the entrance pipe in the tunnels: "Pit of 100 Trials" / "Lowest floor reached: [var 0]".
static u8 N(EntranceMessage)[] = {
    MSG_CHAR_READ_STYLE, MSG_STYLE_SIGN,
    MSG_UC('P'), MSG_LC('i'), MSG_LC('t'), MSG_CHAR_READ_SPACE,
    MSG_LC('o'), MSG_LC('f'), MSG_CHAR_READ_SPACE,
    MSG_CHAR_DIGIT_1, MSG_CHAR_DIGIT_0, MSG_CHAR_DIGIT_0, MSG_CHAR_READ_SPACE,
    MSG_UC('T'), MSG_LC('r'), MSG_LC('i'), MSG_LC('a'), MSG_LC('l'), MSG_LC('s'),
    MSG_CHAR_READ_ENDL,
    MSG_UC('L'), MSG_LC('o'), MSG_LC('w'), MSG_LC('e'), MSG_LC('s'), MSG_LC('t'), MSG_CHAR_READ_SPACE,
    MSG_LC('f'), MSG_LC('l'), MSG_LC('o'), MSG_LC('o'), MSG_LC('r'), MSG_CHAR_READ_SPACE,
    MSG_LC('r'), MSG_LC('e'), MSG_LC('a'), MSG_LC('c'), MSG_LC('h'), MSG_LC('e'), MSG_LC('d'),
    MSG_CHAR_COLON, MSG_CHAR_READ_SPACE,
    MSG_CHAR_READ_FUNCTION, MSG_READ_FUNC_VAR, 0,
    MSG_CHAR_READ_WAIT,
    MSG_CHAR_READ_END,
};

// The same sign with a few lines added below the lowest floor. Built from the message above the first time it is needed.
// - weenie: the lowest floor was reached on a run that turned "Disable Attack Badges" off.
// - impressive: it was reached on a run with the impressive settings (see pit.c).
static u8 N(EntranceMessageWeenie)[160];
static u8 N(EntranceMessageImpressive)[160];
static b32 N(ExtraMessagesBuilt) = false;

static void N(BuildExtraMessage)(u8* dst, const char* text) {
    s32 len = sizeof(N(EntranceMessage)) - 2; // everything except the final WAIT and END
    s32 i;

    for (i = 0; i < len; i++) {
        dst[i] = N(EntranceMessage)[i];
    }
    dst[len++] = MSG_CHAR_READ_ENDL;
    len = pit_text_put(dst, len, text);
    dst[len++] = MSG_CHAR_READ_WAIT;
    dst[len++] = MSG_CHAR_READ_END;
}

// Fills in the floor number, and sets LVar0 to the sign to show: 0 = plain, 1 = weenie, 2 = impressive.
API_CALLABLE(N(SetBestFloorValue)) {
    set_message_int_var(pit_best_floor(), 0);
    if (!N(ExtraMessagesBuilt)) {
        N(BuildExtraMessage)(N(EntranceMessageWeenie), "That would be impressive\nif you weren't such a weenie.");
        N(BuildExtraMessage)(N(EntranceMessageImpressive), "Your Paper Killing Skills\nare Remarkable!");
        N(ExtraMessagesBuilt) = true;
    }
    script->varTable[0] = 0;
    if (pit_best_floor() > 0) {
        if (pit_best_floor_weenie()) {
            script->varTable[0] = 1;
        } else if (pit_best_floor_impressive()) {
            script->varTable[0] = 2;
        }
    }
#ifdef _DEBUG
    // Debug toggles in the menu force a line onto the sign, for testing it.
    if (CVarGetInteger(CVAR_PIT_DEBUG_SIGN_WEENIE, 0)) {
        script->varTable[0] = 1;
    } else if (CVarGetInteger(CVAR_PIT_DEBUG_SIGN_IMPRESSIVE, 0)) {
        script->varTable[0] = 2;
    }
#endif
    return ApiStatus_DONE2;
}

EvtScript N(EVS_ReadEntranceSign) = {
    Call(N(SetBestFloorValue))
    Call(DisablePlayerInput, true)
    IfEq(LVar0, 1)
        Call(ShowMessageAtScreenPos, Ref(N(EntranceMessageWeenie)), 160, 40)
    Else
        IfEq(LVar0, 2)
            Call(ShowMessageAtScreenPos, Ref(N(EntranceMessageImpressive)), 160, 40)
        Else
            Call(ShowMessageAtScreenPos, Ref(N(EntranceMessage)), 160, 40)
        EndIf
    EndIf
    Call(DisablePlayerInput, false)
    Return
    End
};

API_CALLABLE(N(SetFloorValue)) {
    set_message_int_var(pit_current_floor(), 0);
    return ApiStatus_DONE2;
}

EvtScript N(EVS_ReadSign) = {
    Call(N(SetFloorValue))
    Call(DisablePlayerInput, true)
    Call(ShowMessageAtScreenPos, Ref(N(SignMessage)), 160, 40)
    Call(DisablePlayerInput, false)
    Return
    End
};
