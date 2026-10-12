#include "common.h"
#include "pit_of_trials.h"

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

API_CALLABLE(N(SetBestFloorValue)) {
    set_message_int_var(pit_best_floor(), 0);
    return ApiStatus_DONE2;
}

EvtScript N(EVS_ReadEntranceSign) = {
    Call(N(SetBestFloorValue))
    Call(DisablePlayerInput, true)
    Call(ShowMessageAtScreenPos, Ref(N(EntranceMessage)), 160, 40)
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
