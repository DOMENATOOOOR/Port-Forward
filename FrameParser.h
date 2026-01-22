#pragma once
#include "Frame.h"
#include <optional>

class FrameParser {
    bool inFrame = false;
    bool escaped = false;
    Frame cur;
    bool haveConn = false;

public:
    std::optional<Frame> feed(uint8_t b);
};
