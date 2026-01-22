#pragma once
#include <vector>
#include <cstdint>

struct Frame {
    uint8_t conn;
    std::vector<uint8_t> data;
};

std::vector<uint8_t> encodeFrame(const Frame& f);
