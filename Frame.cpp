#include "Frame.h"

static constexpr uint8_t FLAG = 0x7E;
static constexpr uint8_t ESC  = 0x7D;

std::vector<uint8_t> encodeFrame(const Frame& f) {
    std::vector<uint8_t> out;
    out.push_back(FLAG);
    out.push_back(f.conn);

    for (uint8_t b : f.data) {
        if (b == FLAG || b == ESC)
            out.push_back(ESC);
        out.push_back(b);
    }

    out.push_back(FLAG);
    return out;
}
