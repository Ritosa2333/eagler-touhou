#include "game/PostGameFlow.hpp"
#include "game/PracticeSession.hpp"
#include <cstdlib>
#include <iostream>
namespace {
struct Results : th13::PostGamePlatform {
    th13::i32 destination = 0;
    bool practice_ended = false;
    void request_destination(th13::i32 next) override { destination = next; }
    bool request_practice_end() override {
        practice_ended = true;
        return true;
    }
    bool request_stage6_fade() override { return true; }
    void bgm(th13::i32) override {}
};
}

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
            th13::practice_sections[id].index == 1)
            boss = id;
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
    th13::SessionState session;
    session.stage = 1;
    if (!th13::records_practice_stage(session, false)) return 4;
    const auto active = th13::begin_thprac(enabled, session, 1);
    if (!active.applied || active.stage != 2 || active.bomb != 8 || active.power != 400 || enabled.enabled ||
        session.practice_mode != 1 || (session.run_mode_flags & 0x30u) != 0x10u ||
        th13::records_practice_stage(session, false))
        return 5;

    th13::SessionState ordinary;
    if (th13::begin_thprac(enabled, ordinary, 1).applied || !th13::records_practice_stage(ordinary, false)) return 6;

    enabled = {true, boss, 8, 8, 400, 200};
    th13::SessionState spell;
    spell.run_mode_flags = 0x20u;
    if (th13::begin_thprac(enabled, spell, 2).applied || !enabled.enabled || spell.practice_mode != 0 ||
        spell.run_mode_flags != 0x20u)
        return 7;

    th13::HudPlayerState player;
    th13::HudScoreState score;
    session.stage = 6;
    Results practice_results;
    th13::PostGameFlow practice_flow(session, player, score, practice_results);
    practice_flow.on_stage_clear();
    if (!practice_results.practice_ended || practice_results.destination != 0) return 8;

    ordinary.stage = 6;
    Results story_results;
    th13::PostGameFlow story_flow(ordinary, player, score, story_results);
    story_flow.on_stage_clear();
    for (int frame = 0; frame < 380; ++frame)
        story_flow.update();
    if (story_results.practice_ended || story_results.destination != 0xf) return 9;

    std::cout << "practice-session ok\n";
    return 0;
}
