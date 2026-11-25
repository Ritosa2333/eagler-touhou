#include "midboss3_core.hpp"
#include <iostream>
#include <stdexcept>
#include <vector>

using Core = soew::Midboss3Core;
using Flag = Core::Flag;
unsigned checks = 0;
void check(bool value, const char* message) {
    ++checks;
    if (!value) throw std::runtime_error(message);
}

void cycle_test(unsigned rank) {
    Core m;
    Core::Hooks h;
    struct Shot { unsigned frame; soew::EnemyBulletRequest request; };
    std::vector<Shot> shots;
    std::vector<int> hit_order;
    h.bullet = [&](const auto& b) { shots.push_back({m.phase_frame, b}); };
    h.damage = [&](int x, int, int w, int hgt) {
        check(w == 40 && hgt == 30, "hitbox dimensions");
        hit_order.push_back(x);
        return 0u;
    };
    m.trigger(102); check(!m.active, "early trigger");
    m.trigger(103); check(m.active, "trigger");
    for (unsigned frame = 1; frame <= 576; ++frame) {
        const unsigned back = frame & 1;
        m.begin_frame(back);
        const auto first = shots.size();
        const auto hits = hit_order.size();
        m.update(back, frame & 3, 1, rank, h);
        check(m.phase_frame == (frame == 576 ? 127 : frame), "shared phase progression");
        if (frame == 1) {
            check(m.actor[0].left[0] == 32 && m.actor[1].left[1] == 352 && m.actor[0].top[1] == -16, "initial coordinates");
            continue;
        }
        check(hit_order.size() == hits + 2 && hit_order[hits] == m.actor[0].left[back]+12 && hit_order[hits+1] == m.actor[1].left[back]+12, "two actor collision order");
        unsigned expected = 0;
        if (frame >= 128 && frame < 152 && frame % 4 == 0) expected = 2;
        if (frame >= 152 && frame < 192 && frame % 8 == 0) expected = 2;
        if (frame >= 352 && frame < 416 && frame % 4 == 0) expected = 2;
        if (frame >= 416 && frame < 576 && frame % 60 == 0) expected = 2;
        check(shots.size() == first + expected, "shot timing and shared phase wrap");
        for (unsigned i = 0; i < expected; ++i) {
            const auto& b = shots[first + i].request;
            check(b.left == m.actor[i].left[back] + 28, "shot x after movement");
            check(b.top == m.actor[i].top[back] + 12 + ((frame >= 416 && frame < 576) ? 2 : 0), "shot y before movement");
            unsigned group = 0x20, speed = 40, angle = 0;
            if (frame < 152) angle = i ? 128 - 8 * ((frame - 128) / 4 + 1) : 8 * (frame == 128 ? 1 : (frame - 128) / 4);
            else if (frame < 192) { group = rank == 0 ? 2 : 14; angle = i ? 48 + 16 * ((frame - 152) / 8 + 1) : 88 - 16 * ((frame - 152) / 8 + 1); }
            else if (frame < 416) { speed = 50; angle = i ? 8 * ((frame - 352) / 4 + 1) : 120 - 8 * ((frame - 352) / 4 + 1); }
            else if (frame < 576) group = 0x19;
            else angle = 120;
            if(!(b.group == group && b.speed == speed && b.angle == std::uint8_t(angle)))std::cerr<<"frame="<<frame<<" actor="<<i<<" group="<<unsigned(b.group)<<" speed="<<unsigned(b.speed)<<" angle="<<unsigned(b.angle)<<" expected="<<unsigned(std::uint8_t(angle))<<'\n';
            check(b.group == group && b.speed == speed && b.angle == std::uint8_t(angle), "shot parameters");
        }
    }
    check(m.cycles == 1, "first cycle count");
    m.begin_frame(1); m.update(1, 0, 1, rank, h);
    check(m.phase_frame == 128 && m.cycles == 2 && m.angle[0] == 0 && m.angle[1] == 120, "per actor phase 128 resets both angles");
}

void kill_test(unsigned first) {
    Core m; Core::Hooks h;
    unsigned score = 0; std::vector<unsigned> sounds; std::vector<soew::Midboss1Core::Spark> sparks;
    int hit_actor = -1;
    h.damage = [&](int x, int, int, int) { return hit_actor >= 0 && ((x < 200) == (hit_actor == 0)) ? 1u : 0u; };
    h.score = [&](unsigned n) { score += n; };
    h.sound = [&](unsigned n) { sounds.push_back(n); };
    h.spark = [&](const auto& s) { sparks.push_back(s); };
    m.trigger(103);
    for (unsigned frame = 1; frame <= 128; ++frame) { m.begin_frame(frame & 1); m.update(frame & 1, 0, 1, 1, h); }
    m.actor[first].damage = 279; hit_actor = int(first);
    m.begin_frame(1); m.update(1, 0, 1, 1, h);
    check(m.actor[first].damage == 280 && m.actor[first].flag == Flag::Alive && sounds.back() == 4 && score == 0, "damage threshold inclusive");
    m.begin_frame(0); m.update(0, 0, 1, 1, h);
    check(m.actor[first].flag == Flag::Dying && sounds.back() == 2 && score == 20000, "kill score and sound");
    hit_actor = -1;
    for (unsigned n = 1; n <= 48; ++n) {
        m.begin_frame(n & 1); m.update(n & 1, 0, 1, 1, h);
        check(m.actor[first].flag == (n == 48 ? Flag::Removed : Flag::Dying), "death duration");
        if (n < 48) check(m.actor[first].death_pattern == 10 + (n - 1) / 6 && m.actor[first].visible, "death animation pattern");
    }
    check(sparks.size() == 49 && sparks.back().speed == 128 && sparks.back().count == 32 && sparks.back().sprite, "death sparks");
    check(m.target_x == -1 && m.target_y == -1, "death clears target");
    const unsigned second = first ^ 1; m.actor[second].damage = 280; hit_actor = int(second);
    h.damage=[&](int x,int,int,int){return x==m.actor[second].left[1]+12?1u:0u;};
    m.begin_frame(1);
    // ASM 6981-7016 samples the surviving actor before movement and damage.
    const int expected_x=m.actor[second].left[1]+24,expected_y=m.actor[second].top[1]+16;
    m.update(1, 0, 1, 1, h);
    check(m.actor[second].flag == Flag::Dying && score == 40000, "second kill");
    check(m.target_x==expected_x&&m.target_y==expected_y,"second kill retains the pre-move target for this tick");
    hit_actor = -1;
    for (unsigned n = 0; n < 48; ++n) { m.begin_frame(n & 1); m.update(n & 1, 0, 1, 1, h); }
    check(m.actor[second].flag == Flag::Removed && m.active, "removed before invalidation");
    m.begin_frame(0); check(!m.active, "both removed deactivate");
}

void timeout_test() {
    Core m; Core::Hooks h; unsigned score = 0, hits = 0;
    h.score = [&](unsigned n) { score += n; };
    h.damage = [&](int, int, int, int) { ++hits; return 0u; };
    m.trigger(103);
    while (m.cycles < 4) { const unsigned back = (hits / 2) & 1; m.begin_frame(back); m.update(back, 0, 1, 1, h); }
    check(m.phase_frame == 128 && m.actor[0].flag == Flag::Dying && m.actor[1].flag == Flag::Dying && score == 0, "timeout without score");
}
int main() {
    try { cycle_test(0); cycle_test(1); kill_test(0); kill_test(1); timeout_test();
        std::cout << "midboss3_checks=" << checks << " failures=0\n"; return 0;
    } catch (const std::exception& e) { std::cerr << "check " << checks << ": " << e.what() << '\n'; return 1; }
}
