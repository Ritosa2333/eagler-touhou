#pragma once
#include "AnmManager.hpp"
#include "AnmRenderer.hpp"
#include <array>
#include <string>

namespace th13 {
// 原版道具得分弹出池（DAT_004c22d4：0x44ad60 追加 / 0x44a970 更新 /
// 0x44aa20 绘制）。10 槽环形池，每槽最多 8 位十进制（低位在前）。
// 绘制用 ascii.anm sprite 196+digit（age<0x34）/207+（<0x38）/217+；
// value<0 的哨兵位 digit=10 用 sprite 206。alpha 由玩家到弹字位置的平方
// 距离分档（0x30 / 线性 / 0xD0），RGB 来自事件颜色。
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
        glyph = state.glyph; pool = state.pool; cursor = state.cursor; available_ = state.available;
    }
private:
    AnmManager& animations;
    AnmEnvironment& environment;
    AnmVm glyph;
    std::array<Entry, 10> pool{};
    u32 cursor = 0;
    // 合成测试 harness 的 ascii.anm 可能缺 sprite 196-226；缺失时禁用弹出
    // 池（生产 ascii.anm 恒有）。禁用不影响游戏状态/RNG。
    bool available_ = false;
};
}  // namespace th13
