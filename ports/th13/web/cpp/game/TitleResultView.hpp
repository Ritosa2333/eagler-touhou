#pragma once

#include "TitlePlayerDataMenu.hpp"

#include <array>

namespace th13 {

class ReplayFile;
std::string replay_save_slot_row(i32 slot, const ReplayFile*, bool compact);

struct TitleResultScreenData {
    i32 character = 0;
    i32 difficulty = 0;
    i32 rank = -1;
    TitlePlayerDataScore current;
    std::array<TitlePlayerDataScore, 10> records{};
};

}
