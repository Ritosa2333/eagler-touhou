// H-SPELL-SESSION: 00420380/00420d00, 0041e262 and 0041e473.
// Synthetic instructions exercise production argument decoding and rank gates.
#include "ecl_test_fixtures.hpp"
#include "game/StageSim.hpp"
#include "game/SessionState.hpp"
#include "game/ReplaySession.hpp"

using namespace th13;

namespace {
struct Rig {
    StageSim sim;
    StageEnemy enemy;
    SessionState session;
    EclContext context;
    Rig() {
        sim.current = &enemy;
        sim.session = &session;
        sim.gvars.sim = &sim;
        sim.commands.sim = &sim;
        context.variables = &sim.gvars;
        enemy.life = 100;
    }
    int execute(const std::vector<u8>& bytes) {
        context.instruction = reinterpret_cast<const EclInstruction*>(bytes.data());
        return sim.commands.execute(context, context.instruction->opcode);
    }
};

std::vector<u8> spell(u16 opcode) {
    std::vector<u8> bytes, encrypted;
    u8 key = 0x77, increment = 7;
    for (char c : std::string("Synthetic card\0", 15)) {
        encrypted.push_back(u8(c) ^ key);
        key = u8(key + increment);
        increment = u8(increment + 0x10);
    }
    th13test::emit(bytes, 0, opcode, 0, 0xff, 0, {20, 30, 0, u32(encrypted.size())},
                   encrypted.data(), u32(encrypted.size()));
    return bytes;
}
}

TEST(spell_selection_has_one_live_integer_and_float_source) {
    Rig rig;
    i32 integer = 0;
    float floating = 0;
    for (i32 selected : {-1, 0, 13, 126}) {
        rig.session.selected_spell_id = selected;
        CHECK(rig.sim.gvars.integer(-9907, integer));
        CHECK(rig.sim.gvars.floating(-9907, floating));
        CHECK_EQ(integer, selected);
        CHECK_EQ(floating, float(selected));
    }
    rig.sim.session = nullptr;
    CHECK(rig.sim.gvars.integer(-9907, integer));
    CHECK(rig.sim.gvars.floating(-9907, floating));
    CHECK_EQ(integer, -1);
    CHECK_EQ(floating, -1.0f);
}

TEST(begin_and_end_in_one_ecl_turn_keep_both_events_and_award_once) {
    Rig rig;
    std::vector<SpellEvent> events;
    i32 awarded = 0;
    rig.sim.spell_event = [&](const SpellEvent& event, const StageEnemy&) {
        events.push_back(event);
        return true;
    };
    rig.sim.spell_award = [&](i32 score) { awarded += score; };
    CHECK_EQ(rig.execute(spell(522)), 0);
    std::vector<u8> end;
    th13test::emit(end, 0, 523, 0, 0xff, 0, {});
    CHECK_EQ(rig.execute(end), 0);
    CHECK_EQ(events.size(), 2u);
    CHECK(events[0].kind == SpellEventKind::Begin);
    CHECK(events[1].kind == SpellEventKind::End);
    CHECK_EQ(events[0].state.id, 20);
    CHECK_EQ(events[1].state.id, 20);
    CHECK_EQ(awarded, 400000); // default rank3 + stage1, internal score units
    CHECK_EQ(rig.execute(end), 0);
    CHECK_EQ(events.size(), 2u);
    CHECK_EQ(awarded, 400000);
}

TEST(timeout_global_has_matching_integer_and_float_views) {
    Rig rig;
    i32 integer = -1;
    float floating = -1;
    for (bool timed_out : {false, true}) {
        rig.enemy.phase_timed_out = timed_out;
        CHECK(rig.sim.gvars.integer(-9986, integer));
        CHECK(rig.sim.gvars.floating(-9986, floating));
        CHECK_EQ(integer, timed_out ? 1 : 0);
        CHECK_EQ(floating, timed_out ? 1.0f : 0.0f);
    }
}

TEST(spell_opcodes_use_their_original_rank_offsets) {
    Rig rig;
    for (u16 opcode : {522, 528, 537, 538, 539}) {
        for (i32 rank = 0; rank < 6; ++rank) {
            rig.sim.diff = rank;
            rig.enemy.age = 150;
            rig.enemy.invulnerable = 90;
            const auto bytes = spell(opcode);
            CHECK_EQ(rig.execute(bytes), 0);
            const i32 expected = opcode < 537 ? 20 : 20 + rank - (opcode - 537);
            CHECK_EQ(rig.enemy.spell_id, expected);
            CHECK_EQ(rig.enemy.spell_name, std::string("Synthetic card"));
            CHECK(rig.enemy.spell_active);
            CHECK_EQ(rig.enemy.spell_life, 700);
            CHECK_EQ(rig.enemy.age, 0.0f);
            CHECK_EQ(rig.enemy.invulnerable, 90);
        }
    }
}

TEST(timer_reset_preserves_invulnerability) {
    Rig rig;
    rig.enemy.age = 150;
    rig.enemy.invulnerable = 90;
    std::vector<u8> bytes;
    th13test::emit(bytes, 0, 513, 0, 0xff, 0, {});
    CHECK_EQ(rig.execute(bytes), 0);
    CHECK_EQ(rig.enemy.age, 0.0f);
    CHECK_EQ(rig.enemy.invulnerable, 90);
}

TEST(spell_damage_rounds_relative_to_live_phase_threshold) {
    // Direct retail4179e0 probe: initial HP5000, accumulator35000, max10000.
    // +3f5c is the live phase threshold, not +3f50's max life.
    struct Case { i32 threshold, damage, life; };
    const Case cases[] = {
        {0,1,4999},{0,6,4999},{0,7,4999},{0,8,4998},{0,80,4988},
        {3000,1,4999},{3000,6,4999},{3000,7,4999},{3000,8,4998},{3000,80,4988},
        {6000,1,5000},{6000,6,5000},{6000,7,4999},{6000,8,4999},{6000,80,4989},
    };
    for (const auto& c : cases) {
        Rig rig;
        rig.enemy.life = 5000;
        rig.enemy.spell_active = true;
        rig.enemy.spell_base_life = 10000;
        rig.enemy.spell_life = 35000;
        rig.enemy.phase_life_threshold = c.threshold;
        rig.sim.damage(rig.enemy, float(c.damage));
        CHECK_EQ(rig.enemy.life, float(c.life));
        CHECK_EQ(rig.enemy.spell_life, 35000 - c.damage);
    }
}

TEST(spell_practice_resolves_the_timeout_slot_again) {
    struct Slots final : EclVariables {
        i32 reads = 0;
        bool integer(i32, i32& out) override { out = reads++; return true; }
        bool floating(i32, float&) override { return false; }
        i32* integer_destination(i32) override { return nullptr; }
        float* float_destination(i32) override { return nullptr; }
    } slots;
    Rig rig;
    rig.context.variables = &slots;
    rig.sim.mode_flags = 0x20;
    rig.enemy.flags = 0x1000000;
    std::vector<u8> bytes;
    const char label[] = "unused";
    th13test::emit(bytes, 0, 514, 1, 0xff, 0,
                  {u32(-10000), 100, 1200, sizeof label}, label, sizeof label);
    CHECK_EQ(rig.execute(bytes), 0);
    CHECK_EQ(slots.reads, 2);
    CHECK_EQ(rig.enemy.interrupts[0].health_label, std::string("BossDead"));
    CHECK_EQ(rig.enemy.interrupts[0].timeout_label, std::string("BossDead"));
    CHECK_EQ(rig.enemy.interrupts[1].timeout_label, std::string("BossEscape"));
    CHECK_EQ(rig.enemy.interrupts[1].life, -1);
}

TEST(extra_and_overdrive_do_not_alias_easy_and_normal) {
    for (i32 rank = 0; rank < 6; ++rank)
        CHECK_EQ(ecl_difficulty_mask(rank), u8(1u << rank));
    CHECK_EQ(ecl_difficulty_mask(-1), 0);
    CHECK_EQ(ecl_difficulty_mask(6), 0);
    CHECK_EQ(ecl_difficulty_mask(4) & ecl_difficulty_mask(0), 0);
    CHECK_EQ(ecl_difficulty_mask(5) & ecl_difficulty_mask(1), 0);
}

TEST(replay_restoration_normalizes_selected_spell_and_mode_together) {
    ReplayFile file;
    ReplayStage stage;
    stage.header.resize(0xc4);
    file.stages.push_back(stage);
    SessionState session;
    HudPlayerState player;
    HudScoreState score;
    ReplayRestoreLog log;
    std::string error;
    for (i32 selected : {-1, 13}) {
        for (u32 before : {0u, 0x10u, 0x20u}) {
            std::memcpy(file.stages[0].header.data() + 0x30, &selected, 4);
            session.run_mode_flags = before | 0x100u;
            CHECK(ReplaySession::restore_stage(file, 0,
                {&session, &player, &score, nullptr, nullptr}, log, error));
            CHECK_EQ(session.selected_spell_id, before == 0x20u ? selected : -1);
            CHECK_EQ(session.run_mode_flags, 0x100u | (selected >= 0 ? 0x20u : 0u));
        }
    }
}

TEST(spell_practice_boss_interrupt_exits_selected_card_without_reading_hp) {
    Rig rig;
    rig.sim.mode_flags = 0x20;
    rig.enemy.flags = 0x1000000;
    std::vector<u8> bytes;
    const char label[] = "NextCard";
    // Referenced HP=-10000 would consume RNG if incorrectly evaluated.
    th13test::emit(bytes, 0, 514, 2, 0xff, 0,
                  {0, u32(-10000), 1200, sizeof label}, label, sizeof label);
    GameRandom expected = rig.sim.random;
    CHECK_EQ(rig.execute(bytes), 0);
    CHECK_EQ(rig.enemy.interrupts[0].life, 0);
    CHECK_EQ(rig.enemy.interrupts[0].timeout, 1200);
    CHECK_EQ(rig.enemy.interrupts[0].health_label, std::string("BossDead"));
    CHECK_EQ(rig.enemy.interrupts[0].timeout_label, std::string("BossEscape"));
    CHECK_EQ(rig.sim.random.next(), expected.next());
    rig.sim.mode_flags = 0;
    CHECK_EQ(rig.execute(bytes), 0);
    CHECK_EQ(rig.enemy.interrupts[0].health_label, std::string("NextCard"));
}

int main() { return th13test::run_all("spell session and ECL dispatch"); }
