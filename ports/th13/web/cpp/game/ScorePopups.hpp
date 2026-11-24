#pragma once
#include "AnmManager.hpp"
#include "AnmRenderer.hpp"
#include <array>
#include <string>

namespace th13 {
class ScorePopups {
public:
    ScorePopups(AnmManager& animations, AnmEnvironment& environment)
        : animations(animations), environment(environment) {}
    bool initialize(i32 bank);
    bool spawn(i32 value, float x, float y, u32 color);
    void update(float rate);
    bool draw(AnmRenderer& renderer, float player_x, float player_y);
    std::string error;

public:
    struct Entry {
        std::array<i32, 8> digits{};
        i32 count = 0;
        float x = 0, y = 0, z = 0;
        u32 color = 0xffffffffu;
        float scale = 0;
        i32 age = 0;
        bool active = false;
    };
    struct RollbackState {
        AnmVm glyph;
        std::array<Entry, 10> pool;
        u32 cursor;
        bool available;
    };
    RollbackState capture_rollback() const { return {glyph, pool, cursor, available_}; }
    void restore_rollback(const RollbackState& state) {
        glyph = state.glyph;
        pool = state.pool;
        cursor = state.cursor;
        available_ = state.available;
    }

private:
    AnmManager& animations;
    AnmEnvironment& environment;
    AnmVm glyph;
    std::array<Entry, 10> pool{};
    u32 cursor = 0;
    bool available_ = false;
};
}
