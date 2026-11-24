#include "ScorePopups.hpp"
#include <cmath>
#include <limits>

namespace th13 {
namespace {
constexpr float kScreenX = 320.0f, kScreenY = 16.0f;
}

bool ScorePopups::initialize(i32 bank) {
    available_ = false;
    auto* resource = animations.resource(bank);
    if (!resource) return true;
    glyph = AnmVm{};
    glyph.environment = &environment;
    glyph.resource = resource;
    glyph.visual.flags = 7;
    glyph.visual.render_flags = 0x4000;
    if (!glyph.select_sprite(196)) return true;
    available_ = true;
    return true;
}

bool ScorePopups::spawn(i32 value, float x, float y, u32 color) {
    if (!available_) return true;
    Entry& e = pool[cursor];
    cursor = (cursor + 1) % 10;
    e = Entry{};
    if (value < 0) {
        e.digits[0] = 10;
        e.count = 1;
    } else {
        i32 v = value, n = 0;
        do {
            e.digits[u32(n++)] = v % 10;
            v /= 10;
        } while (v != 0 && n < 8);
        if (n == 0) {
            e.digits[0] = 0;
            n = 1;
        }
        e.count = n;
    }
    e.x = x;
    e.y = y;
    e.z = 0;
    e.color = color;
    e.scale = 0;
    e.age = 0;
    e.active = true;
    return true;
}

void ScorePopups::update(float rate) {
    for (auto& e : pool) {
        if (!e.active) continue;
        e.y = float(e.y - float(rate * 0.5f));
        e.scale = float(e.scale + rate);
        e.age = i32(e.scale);
        if (e.age > 60) e.active = false;
    }
}

bool ScorePopups::draw(AnmRenderer& renderer, float player_x, float player_y) {
    if (!available_) return true;
    for (auto& e : pool) {
        if (!e.active) continue;
        const float spacing = e.age < 8 ? float(8.0f / e.scale) : 8.0f;
        const float dx = float(player_x - e.x), dy = float(player_y - e.y);
        const i32 d2 = i32(float(dx * dx + dy * dy));
        u8 alpha;
        if (d2 > 0x4000)
            alpha = 0xD0;
        else if (d2 > 0x1000)
            alpha = u8(0x30 + ((d2 * 5 - 0x5000) * 0x20) / 0x3000);
        else
            alpha = 0x30;
        const u32 color = (e.color & 0x00ffffffu) | (u32(alpha) << 24);
        float x = float(e.x - float(float(e.count) * spacing * 0.5f));
        for (i32 i = e.count - 1; i >= 0; --i) {
            const i32 digit = e.digits[u32(i)];
            i32 sprite;
            if (e.age < 0x34 || digit == 10)
                sprite = 196 + digit;
            else if (e.age < 0x38)
                sprite = 207 + digit;
            else
                sprite = 217 + digit;
            auto& v = glyph.visual;
            v.flags = (v.flags & ~0x01e00000u) | 0x0000000fu;
            v.color = color;
            v.layer = 20;
            float px = float(x + kScreenX);
            if (std::isnan(px)) px = std::numeric_limits<float>::quiet_NaN();
            glyph.variables.position = {px, float(e.y + kScreenY), e.z};
            if (!glyph.select_sprite(sprite)) {
                error = "Score popup sprite unavailable: " + std::to_string(sprite);
                return false;
            }
            v.flags |= 8u;
            if (renderer.draw_glyph(glyph) == -2) {
                error = renderer.error.empty() ? "Score popup draw failed" : renderer.error;
                return false;
            }
            x = float(x + spacing);
        }
    }
    renderer.flush();
    return true;
}
}
