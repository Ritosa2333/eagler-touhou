#include "game/PracticeSession.hpp"
#include <cstdlib>
#include <iostream>
int main() {
    using th13::PracticeRequest;
    using th13::resolve_practice;
    PracticeRequest disabled;
    disabled.enabled = false;
    disabled.section = 1;
    const auto idle = resolve_practice(disabled, 1);
    if (idle.applied || idle.stage != 1) return 1;
    int boss = 0;
    for (int id = 1; id <= th13::practice_section_count; ++id) {
        if (th13::practice_sections[id].stage == 2 && th13::practice_sections[id].group == 2 &&
            th13::practice_sections[id].index == 1) boss = id;
    }
    if (!boss) return 2;
    PracticeRequest enabled;
    enabled.enabled = true;
    enabled.section = boss;
    enabled.life = 8;
    enabled.bomb = 8;
    enabled.power = 400;
    const auto warped = resolve_practice(enabled, 1);
    if (!warped.applied || warped.stage != 2 || warped.section != boss || warped.life != 8) return 3;
    std::cout << "practice-session ok\n";
    return 0;
}
