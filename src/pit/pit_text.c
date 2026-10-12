#include "common.h"
#include "pit_text.h"

s32 pit_text_put(u8* dst, s32 pos, const char* text) {
    for (; *text != '\0'; text++) {
        char c = *text;

        if (c >= 'A' && c <= 'Z') {
            dst[pos++] = MSG_CHAR_UPPER_A + (c - 'A');
        } else if (c >= 'a' && c <= 'z') {
            dst[pos++] = MSG_CHAR_LOWER_A + (c - 'a');
        } else if (c >= '0' && c <= '9') {
            dst[pos++] = MSG_CHAR_DIGIT_0 + (c - '0');
        } else if (c == ' ') {
            dst[pos++] = MSG_CHAR_READ_SPACE;
        } else if (c == '.') {
            dst[pos++] = MSG_CHAR_PERIOD;
        } else if (c == '?') {
            dst[pos++] = MSG_CHAR_QUESTION;
        } else if (c == ':') {
            dst[pos++] = MSG_CHAR_COLON;
        } else if (c == '-') {
            dst[pos++] = MSG_CHAR_MINUS;
        } else if (c == '\n') {
            dst[pos++] = MSG_CHAR_READ_ENDL;
        }
    }
    return pos;
}

void pit_text_speech(u8* dst, const char* text, bool isLast) {
    s32 pos = 0;

    dst[pos++] = MSG_CHAR_READ_STYLE;
    dst[pos++] = MSG_STYLE_RIGHT;
    pos = pit_text_put(dst, pos, text);
    if (isLast) {
        dst[pos++] = MSG_CHAR_READ_WAIT;
    } else {
        dst[pos++] = MSG_CHAR_READ_FUNCTION;
        dst[pos++] = MSG_READ_FUNC_YIELD;
    }
    dst[pos++] = MSG_CHAR_READ_END;
}
