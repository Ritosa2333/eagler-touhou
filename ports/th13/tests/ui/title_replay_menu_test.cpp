#include "../harness/test_framework.hpp"
#include "ui_test_fixtures.hpp"
#include <cstdio>

#include "game/FrameScheduler.hpp"
#include "game/TitleMainMenu.hpp"
#include "game/TitleScene.hpp"

#include <ctime>
#include <memory>
#include <string>
#include <vector>

using namespace th13;
using namespace th13::test;

namespace {

struct BitWriter {
    std::vector<u8> bytes;
    u32 bit = 0;
    void put(u32 value, u32 count) {
        bytes.resize(bytes.size() + (bit + count + 7) / 8 - bit / 8, 0);
        for (u32 i = count; i > 0; --i) {
            if (value >> (i - 1) & 1) bytes[bit >> 3] |= u8(0x80 >> (bit & 7));
            ++bit;
        }
    }
};

std::vector<u8> literal_compress(const std::vector<u8>& plain) {
    BitWriter writer;
    for (u8 byte : plain) {
        writer.put(1, 1);
        writer.put(byte, 8);
    }
    writer.put(0, 1);
    writer.put(0, 13);
    return std::move(writer.bytes);
}

std::vector<u8> build_replay(i32 character, i32 rank, i32 route, i32 continues, i64 stamp, bool spell_practice,
                             const std::vector<std::pair<i32, i32>>& stages, float slow = 0.f) {
    const u32 frames = 60;
    std::vector<u8> plain(0x74, 0);
    const char name[] = "TESTER";
    std::memcpy(plain.data(), name, sizeof name - 1);
    u8 flag = 0;
    if (spell_practice) flag |= 2;
    plain[0x0A] = flag;
    std::memcpy(plain.data() + 0x0C, &stamp, 8);
    const u32 score = 24681357;
    std::memcpy(plain.data() + 0x14, &score, 4);
    std::memcpy(plain.data() + 0x54, &slow, 4);
    plain[0x58] = u8(stages.size());
    std::memcpy(plain.data() + 0x5C, &character, 4);
    const i32 shot = 0;
    std::memcpy(plain.data() + 0x60, &shot, 4);
    std::memcpy(plain.data() + 0x64, &rank, 4);
    std::memcpy(plain.data() + 0x68, &route, 4);
    std::memcpy(plain.data() + 0x6C, &continues, 4);
    const i32 practice = spell_practice ? 41 : -1;
    std::memcpy(plain.data() + 0x70, &practice, 4);
    for (const auto& stage : stages) {
        std::vector<u8> input(frames * 6, 0);
        std::vector<u8> events((frames + 29) / 30, 60);
        const u32 blob = u32(input.size() + events.size());
        std::vector<u8> block(0xC4, 0);
        const u16 number = u16(stage.first);
        std::memcpy(block.data(), &number, 2);
        std::memcpy(block.data() + 4, &frames, 4);
        std::memcpy(block.data() + 8, &blob, 4);
        std::memcpy(block.data() + 0x1C, &stage.second, 4);
        plain.insert(plain.end(), block.begin(), block.end());
        plain.insert(plain.end(), input.begin(), input.end());
        plain.insert(plain.end(), events.begin(), events.end());
    }
    std::vector<u8> body = literal_compress(plain);
    ReplayFile::xor_encrypt(body.data(), body.size(), 0x7D, 0x3A, 0x100);
    ReplayFile::xor_encrypt(body.data(), body.size(), 0x5C, 0xE1, 0x400);
    std::vector<u8> file(0x24, 0);
    std::memcpy(file.data(), "t13r", 4);
    const u16 version = 2;
    std::memcpy(file.data() + 4, &version, 2);
    const u32 user_offset = u32(0x24 + body.size());
    std::memcpy(file.data() + 0x0C, &user_offset, 4);
    const u32 hundred = 0x100;
    std::memcpy(file.data() + 0x10, &hundred, 4);
    const u32 comp = u32(body.size()), uncomp = u32(plain.size());
    std::memcpy(file.data() + 0x1C, &comp, 4);
    std::memcpy(file.data() + 0x20, &uncomp, 4);
    file.insert(file.end(), body.begin(), body.end());
    return file;
}

ReplayFile decode_replay(const std::vector<u8>& bytes) {
    ReplayFile file;
    CHECK(file.decode(bytes.data(), bytes.size()));
    return file;
}

struct ReplayMenuHost final : TitleScenePlatform {
    AnmManager& manager;
    std::array<std::shared_ptr<ReplayFile>, 75> slots;
    std::array<std::string, 75> names;
    i32 import_hint = -1;
    std::string import_error;
    bool request_ok = true;
    int request_calls = 0;
    i32 started_file = -1, started_stage = -1;
    int sounds = 0;
    struct Text {
        Vec3 position;
        u32 color;
        std::string text;
    };
    std::vector<Text> texts;
    i32 start_stage = -1;
    explicit ReplayMenuHost(AnmManager& manager) : manager(manager) {}
    bool clear_title_overlay() override { return true; }
    bool queue_title_music(i32, i32, const std::string&) override { return true; }
    bool clear_current_wave() override { return true; }
    bool fade_out_title_music() override { return true; }
    bool sound(i32) override {
        ++sounds;
        return true;
    }
    bool begin_transition(u32& animation) override {
        animation = manager.create(8, 0, -1, 0);
        return animation != 0;
    }
    bool start_game(i32 stage) override {
        start_stage = stage;
        return true;
    }
    bool title_exit(i32) override { return true; }
    bool hud_text(const HudTextDraw& text) override {
        texts.push_back({text.position, text.style.color, text.text});
        return true;
    }
    i32 replay_count() const override { return i32(slots.size()); }
    const ReplayFile* replay_entry(i32 index) const override {
        return index >= 0 && index < i32(slots.size()) && slots[u32(index)] ? slots[u32(index)].get() : nullptr;
    }
    std::string replay_filename(i32 index) const override {
        return index >= 0 && index < i32(slots.size()) ? names[u32(index)] : std::string();
    }
    bool request_replay_file() override {
        ++request_calls;
        return request_ok;
    }
    std::string replay_import_error() const override { return import_error; }
    bool start_replay(i32 file_index, i32 stage_index) override {
        if (started_file >= 0) return false;
        started_file = file_index;
        started_stage = stage_index;
        return true;
    }
    void put(i32 slot, const std::string& name, const std::vector<u8>& bytes) {
        slots[u32(slot)] = std::make_shared<ReplayFile>(decode_replay(bytes));
        names[u32(slot)] = name;
    }
};

std::vector<u8> title_anm_fixture() {
    std::vector<std::vector<AnmIns>> scripts(243);
    for (auto& script : scripts)
        script = {wait(), stop()};
    auto spawn_range = [&](i32 root, i32 first, i32 last) {
        scripts[u32(root)].clear();
        for (i32 child = first; child <= last; child++)
            scripts[u32(root)].push_back(spawn(child));
        scripts[u32(root)].push_back(wait());
        scripts[u32(root)].push_back(stop());
    };
    spawn_range(0, 3, 22);
    for (i32 item = 0; item < 10; item++) {
        scripts[u32(3 + item)].clear();
        scripts[u32(3 + item)].push_back(label(7 + item));
        scripts[u32(3 + item)].push_back(wait());
        scripts[u32(3 + item)].push_back(stop());
    }
    spawn_range(1, 23, 48);
    spawn_range(2, 49, 82);
    scripts[89] = {spawn(92), spawn(93), spawn(91), spawn(90), wait(), stop()};
    scripts[94] = {spawn(97), spawn(96), spawn(100), wait(), stop()};
    scripts[95] = {spawn(99), spawn(96), spawn(100), wait(), stop()};
    scripts[123].clear();
    for (i32 child : {83, 84, 118, 119, 120, 121, 113, 114, 115, 116})
        scripts[123].push_back(spawn(child));
    scripts[123].push_back(wait());
    scripts[123].push_back(stop());
    return synthetic_anm(8, scripts);
}

struct ReplayHarness {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    SessionState progress;
    TitleSelectionSettings selection;
    FrameScheduler scheduler;
    ReplayMenuHost host{manager};
    std::array<FrameCallback, 2> animation_updates{};
    std::unique_ptr<TitleScene> scene;

    ReplayHarness() {
        animation_updates[0].owner = animation_updates[1].owner = &manager;
        animation_updates[0].enabled = animation_updates[1].enabled = true;
        animation_updates[0].run = [](void* p) { return static_cast<AnmManager*>(p)->update(true) ? 1 : 5; };
        animation_updates[1].run = [](void* p) { return static_cast<AnmManager*>(p)->update(false) ? 1 : 5; };
        CHECK(scheduler.add(animation_updates[0], FramePass::Update, 9) >= 0);
        CHECK(scheduler.add(animation_updates[1], FramePass::Update, 34) >= 0);
        CHECK(load_bank(manager, 16, title_anm_fixture()));
        CHECK(load_bank(manager, 17, synthetic_anm(1, trivial_scripts(1))));
        CHECK(load_bank(manager, 2, synthetic_anm(8, trivial_scripts(20))));
        CHECK(load_bank(manager, 19, synthetic_anm(4, trivial_scripts(4))));
        CHECK(load_bank(manager, 8, synthetic_anm(4, trivial_scripts(4))));
        scene = std::make_unique<TitleScene>(progress, selection, manager, host, scheduler, TitleSceneResources{});
        CHECK(scene->error.empty());
        scene->state.return_reason = 0;
    }

    void frame(u32 pressed = 0, u32 repeated = 0) {
        scene->controls({0, pressed, repeated, 0});
        CHECK(scheduler.update() >= 0);
        CHECK(scheduler.draw() >= 0);
    }
    void advance(int frames, u32 pressed = 0, u32 repeated = 0) {
        for (int i = 0; i < frames; i++)
            frame(pressed, repeated);
    }
    void reach_menu() {
        advance(kTitleBootIntroFrames + kTitleMenuReadyDelay + 3);
        CHECK_EQ(int(scene->state.screen), int(TitleScreen::Main));
        CHECK_EQ(scene->state.substate, 2);
    }
    void enter_replay() {
        for (int i = 0; i < 3; i++)
            frame(0x20);
        CHECK_EQ(scene->state.menu.cursor, 4);
        frame(0x01);
        advance(25);
        CHECK_EQ(int(scene->state.screen), int(TitleScreen::Replay));
        advance(8);
        CHECK_EQ(scene->state.substate, 2);
    }
    const ReplayMenuHost::Text* find_text(const char* prefix) {
        for (const auto& text : host.texts)
            if (text.text.rfind(prefix, 0) == 0) return &text;
        return nullptr;
    }
};

constexpr i64 kStamp = 1719226000;
}

TEST(replay_menu_navigates_pages_and_returns) {
    ReplayHarness harness;
    const std::vector<u8> bytes = build_replay(3, 1, 8, 0, kStamp, false, {{1, 0}});
    harness.host.put(0, "th13_01.rpy", bytes);
    harness.reach_menu();
    harness.enter_replay();
    CHECK(harness.scene->state.handles[104] != 0);
    CHECK(harness.scene->state.replay_ascii_root != 0);
    CHECK(harness.scene->state.menu.cursor == 0);
    CHECK(harness.scene->error.empty());
    const ReplayFile& file = *harness.host.slots[0];
    std::tm broken{};
    const std::time_t stamp = std::time_t(kStamp);
    CHECK(localtime_r(&stamp, &broken) != nullptr);
    char expected[128];
    std::snprintf(expected, sizeof expected, "No.01 TESTER %.2d/%.2d/%.2d %.2d:%.2d Youmu   Normal  All %2.1f%%",
                  broken.tm_year % 100, broken.tm_mon + 1, broken.tm_mday, broken.tm_hour, broken.tm_min,
                  double(file.slow_rate));
    harness.host.texts.clear();
    harness.frame();
    CHECK(harness.host.texts.size() >= 25);
    CHECK_EQ(harness.host.texts[0].text, std::string(expected));
    CHECK_EQ(harness.host.texts[0].color, 0xffffff00u);
    CHECK_NEAR(harness.host.texts[0].position.x, 58.f, 1e-5);
    CHECK_NEAR(harness.host.texts[0].position.y, 80.f, 1e-5);
    CHECK_EQ(harness.host.texts[1].text, std::string("No.02 -------- --/--/-- --:-- ------- ------- --- ---%"));
    CHECK_EQ(harness.host.texts[1].color, 0xff808080u);
    CHECK_NEAR(harness.host.texts[1].position.y, 95.f, 1e-5);
    const int sounds_before = harness.host.sounds;
    harness.frame(0x20);
    CHECK_EQ(harness.scene->state.menu.cursor, 1);
    CHECK(harness.host.sounds == sounds_before + 1);
    harness.frame(0x80);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Replay));
    CHECK_EQ(harness.scene->state.substate, 2);
    harness.host.texts.clear();
    harness.frame();
    CHECK(harness.host.texts.size() >= 25);
    CHECK_EQ(harness.host.texts[0].text, std::string("User  -------- --/--/-- --:-- ------- ------- --- ---%"));
    harness.frame(0x40);
    harness.frame(0x100);
    harness.advance(8);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Main));
    CHECK_EQ(harness.scene->state.handles[104], 0u);
    CHECK_EQ(harness.scene->state.replay_ascii_root, 0u);
    CHECK(harness.scene->error.empty());
}

TEST(replay_menu_stage_select_and_launch) {
    ReplayHarness harness;
    const std::vector<u8> bytes = build_replay(0, 1, 8, 2, kStamp, false, {{1, 0}, {2, 111}, {3, 2222}});
    harness.host.put(1, "th13_02.rpy", bytes);
    harness.reach_menu();
    harness.enter_replay();
    harness.frame(0x20);
    harness.frame(0x01);
    CHECK_EQ(harness.scene->state.substate, 4);
    harness.host.texts.clear();
    harness.frame();
    CHECK(harness.host.texts.size() == 1);
    CHECK_NEAR(harness.host.texts[0].position.y, 92.f, 1e-4);
    CHECK_NEAR(harness.host.texts[0].position.x, 58.f, 1e-5);
    harness.advance(15);
    CHECK_EQ(harness.scene->state.menu.cursor, 0);
    CHECK_EQ(harness.scene->state.menu.count, 7);
    harness.advance(15);
    harness.host.texts.clear();
    harness.frame();
    CHECK(harness.host.texts.size() >= 8);
    CHECK_NEAR(harness.host.texts[0].position.x, 58.f, 1e-5);
    CHECK_NEAR(harness.host.texts[0].position.y, 80.f, 1e-5);
    CHECK_EQ(harness.host.texts[0].color, 0xffffffffu);
    CHECK_EQ(harness.host.texts[1].text, std::string("Stage 1  000001110"));
    CHECK_EQ(harness.host.texts[1].color, 0xffffff00u);
    CHECK_NEAR(harness.host.texts[1].position.x, 220.f, 1e-5);
    CHECK_NEAR(harness.host.texts[1].position.y, 128.f, 1e-5);
    CHECK_EQ(harness.host.texts[2].text, std::string("Stage 2  000022220"));
    CHECK_EQ(harness.host.texts[2].color, 0xff808080u);
    CHECK_EQ(harness.host.texts[3].text, std::string("Stage 3  246813572"));
    CHECK_EQ(harness.host.texts[4].text, std::string("Stage 4  ---------"));
    CHECK_EQ(harness.host.texts[5].text, std::string("Stage 5  ---------"));
    CHECK_EQ(harness.host.texts[6].text, std::string("Stage 6  ---------"));
    CHECK_EQ(harness.host.texts[7].text, std::string("Extra    ---------"));
    harness.frame(0x01);
    CHECK_EQ(harness.scene->state.substate, 3);
    harness.advance(33);
    CHECK_EQ(harness.host.started_file, 1);
    CHECK_EQ(harness.host.started_stage, 0);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Replay));
    CHECK(harness.scene->error.empty());
}

TEST(replay_menu_stage_escape_returns_and_second_stage) {
    ReplayHarness harness;
    const std::vector<u8> bytes = build_replay(0, 1, 8, 0, kStamp, false, {{1, 0}, {2, 111}, {3, 2222}});
    harness.host.put(0, "th13_01.rpy", bytes);
    harness.reach_menu();
    harness.enter_replay();
    harness.frame(0x01);
    CHECK_EQ(harness.scene->state.substate, 4);
    CHECK_EQ(harness.scene->state.menu.cursor, 0);
    harness.advance(15);
    harness.frame(0x20);
    CHECK_EQ(harness.scene->state.menu.cursor, 1);
    harness.frame(0x100);
    CHECK_EQ(harness.scene->state.substate, 2);
    CHECK_EQ(harness.scene->state.menu.cursor, 0);
    CHECK_EQ(harness.scene->state.menu.count, 25);
    harness.frame(0x01);
    CHECK_EQ(harness.scene->state.substate, 4);
    CHECK_EQ(harness.scene->state.menu.cursor, 0);
    harness.advance(15);
    harness.frame(0x20);
    CHECK_EQ(harness.scene->state.menu.cursor, 1);
    harness.frame(0x01);
    CHECK_EQ(harness.scene->state.substate, 3);
    harness.advance(33);
    CHECK_EQ(harness.host.started_stage, 1);
    CHECK(harness.scene->error.empty());
}

TEST(replay_menu_import_row_and_hint_selection) {
    ReplayHarness harness;
    harness.reach_menu();
    harness.enter_replay();
    for (int i = 0; i < 2; i++)
        harness.frame(0x80);
    for (int i = 0; i < 24; i++)
        harness.frame(0x20);
    CHECK_EQ(harness.scene->state.menu.cursor, 24);
    harness.host.texts.clear();
    harness.frame();
    CHECK(harness.host.texts.size() >= 25);
    CHECK_EQ(harness.host.texts[24].text, std::string("Import  -------- --/--/-- --:-- ------- ------- --- ---%"));
    harness.frame(0x01);
    CHECK_EQ(harness.host.request_calls, 1);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Replay));
    const std::vector<u8> bytes = build_replay(3, 1, 8, 0, kStamp, false, {{1, 0}});
    harness.host.put(0, "th13_01.rpy", bytes);
    harness.scene->state.replay_import_hint = 0;
    harness.frame();
    CHECK_EQ(harness.scene->state.menu.cursor, 0);
    CHECK_EQ(harness.scene->state.replay_import_hint, -1);
    harness.frame(0x01);
    CHECK_EQ(harness.scene->state.substate, 4);
    CHECK(harness.scene->error.empty());
}

TEST(replay_menu_import_request_failure_stays_operable) {
    ReplayHarness harness;
    harness.host.request_ok = false;
    harness.host.import_error = "Replay import needs the browser runtime";
    harness.reach_menu();
    harness.enter_replay();
    for (int i = 0; i < 2; i++)
        harness.frame(0x80);
    for (int i = 0; i < 24; i++)
        harness.frame(0x20);
    harness.host.texts.clear();
    harness.frame(0x01);
    CHECK_EQ(harness.host.request_calls, 1);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Replay));
    CHECK_EQ(harness.scene->state.substate, 2);
    harness.host.texts.clear();
    harness.frame();
    CHECK(harness.host.texts.size() >= 25);
    CHECK_EQ(harness.host.texts[24].text, std::string("Import  Replay import needs the browser runtime"));
    CHECK(harness.scene->error.empty());
}

TEST(replay_menu_spell_practice_skips_stage_select) {
    ReplayHarness harness;
    const std::vector<u8> bytes = build_replay(3, 1, 8, 0, kStamp, true, {{1, 0}});
    harness.host.put(25, "th13_udSp01.rpy", bytes);
    harness.reach_menu();
    harness.enter_replay();
    harness.frame(0x80);
    harness.frame(0x01);
    CHECK_EQ(harness.scene->state.substate, 3);
    harness.advance(33);
    CHECK_EQ(harness.host.started_file, 25);
    CHECK_EQ(harness.host.started_stage, 0);
    CHECK(harness.scene->error.empty());
}

TEST(replay_menu_empty_slot_confirm_is_noop) {
    ReplayHarness harness;
    harness.reach_menu();
    harness.enter_replay();
    harness.frame(0x20);
    harness.frame(0x01);
    CHECK_EQ(harness.scene->state.substate, 2);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Replay));
    CHECK(harness.scene->error.empty());
}

int main() { return th13test::run_all("Replay 菜单"); }
