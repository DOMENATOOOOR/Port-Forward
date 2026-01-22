#include "FrameParser.h"

static constexpr uint8_t FLAG = 0x7E;
static constexpr uint8_t ESC  = 0x7D;

std::optional<Frame> FrameParser::feed(uint8_t b) {
    if (!inFrame) {
        if (b == FLAG) {
            inFrame = true;
            escaped = false;
            haveConn = false;
            cur.data.clear();
        }
        return std::nullopt;
    }

    if (escaped) {
        cur.data.push_back(b);
        escaped = false;
        return std::nullopt;
    }

    if (b == ESC) {
        escaped = true;
        return std::nullopt;
    }

    if (b == FLAG) {
        inFrame = false;
        return cur;
    }

    if (!haveConn) {
        cur.conn = b;
        haveConn = true;
    } else {
        cur.data.push_back(b);
    }

    return std::nullopt;
}
