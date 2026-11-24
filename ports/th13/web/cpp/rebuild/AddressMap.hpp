#pragma once

#include <cstdint>

namespace th13::rebuild {

constexpr uint32_t kImageBase = 0x400000;
constexpr uint32_t kEntryPoint = 0x47BDA0;

struct AddressAnchor {
    const char* subsystem;
    uint32_t originalEntry;
};

constexpr AddressAnchor kAnchors[] = {
    {"rng", 0x47BBA2},
    {"supervisor", 0x439EE0},
    {"anm-vm", 0x462450},
    {"ecl-vm", 0x4772A0},
    {"ecl-dispatch", 0x41B260},
    {"enemy-manager", 0x418E00},
    {"bullet-manager", 0x40E360},
    {"item-manager", 0x42E380},
    {"player", 0x443240},
    {"spirit", 0x438660},
    {"replay", 0x448C40},
    {"msg-dialog", 0x428ae0},
    {"stage-clear", 0x42cd40},
    {"post-game-overlay", 0x43dbc0},
    {"ending-scene", 0x416130},
    {"ending-update", 0x416a10},
    {"scene-dispatch", 0x43c1c0},
    {"score-save", 0x449e50},
};

}
