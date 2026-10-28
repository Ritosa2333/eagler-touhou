// 标题 Replay 菜单状态机单测（0x4525b0/0x452c30 移植对合）：
//   主菜单 Replay 项 → screen 11 → case0 建面（s104 徽记 + ascii s19 根）→
//   case1 >6 帧进列表 → 上下移行/左右换页（音 10）→ 空槽/导入行/合法录像行
//   文本（0x4c0d8c 族格式串）→ Z 确认进选关（缺关禁用）→ Z 确认关卡
//   （>31 帧排队 start_replay，stage 块链序映射）→ X 逐级返回（case5 收面）；
//   网页导入：request → hint 选中；符卡练习录像跳过选关。
// 录像夹具走保存路径合成（字面量压缩 → 双趟加密 → 0x24 头容器），
// 顶层头/stage 头字段按 0x448c40 布局（+0x0C 时刻、+0x14 分、+0x5C..0x70
// 装备、+0x68 路线、+0x6C 续关；stage +0x1C 起始分）。
#include "ui_test_fixtures.hpp"
#include <cstdio>
#include "../harness/test_framework.hpp"

#include "game/TitleScene.hpp"
#include "game/TitleMainMenu.hpp"
#include "game/FrameScheduler.hpp"

#include <ctime>
#include <memory>
#include <string>
#include <vector>

using namespace th13;
using namespace th13::test;

namespace {

// ---- 合成 .rpy（保存路径：字面量压缩 → 0x4723a0 双趟加密 → 0x24 头容器）----
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

// 顶层头字段（0x448c40 布局）+ stage 块链。
std::vector<u8> build_replay(i32 character, i32 rank, i32 route, i32 continues, i64 stamp,
                             bool spell_practice, const std::vector<std::pair<i32, i32>>& stages,
                             float slow = 0.f) {
    const u32 frames = 60;
    std::vector<u8> plain(0x74, 0);
    const char name[] = "TESTER";
    std::memcpy(plain.data(), name, sizeof name - 1);
    u8 flag = 0;
    if (spell_practice) flag |= 2;  // 顶层头 +0xA bit1（0x447900 录制侧置位）
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
    // 0x24 字节头容器（原版 0x4484d0 布局；+0x14..0x23 保留零，此前 12 字节
    // 容器写 +0x1C/+0x20 越界导致 decode 读到错位 comp/uncomp）。
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

// ---- 平台桩：75 槽 catalog + 导入/启动记录 + 文本捕获 ----
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
    // TitleReplayPlatform
    i32 replay_count() const override { return i32(slots.size()); }
    const ReplayFile* replay_entry(i32 index) const override {
        return index >= 0 && index < i32(slots.size()) && slots[u32(index)] ? slots[u32(index)].get()
                                                                           : nullptr;
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
        if (started_file >= 0) return false;  // 只应排队一次
        started_file = file_index;
        started_stage = stage_index;
        return true;
    }
    void put(i32 slot, const std::string& name, const std::vector<u8>& bytes) {
        slots[u32(slot)] = std::make_shared<ReplayFile>(decode_replay(bytes));
        names[u32(slot)] = name;
    }
};

// title.anm 合成镜像（title_state_test 同款布局；104/89 平凡脚本已覆盖）。
std::vector<u8> title_anm_fixture() {
    std::vector<std::vector<AnmIns>> scripts(243);
    for (auto& script : scripts) script = {wait(), stop()};
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
        animation_updates[0].run = [](void* p) {
            return static_cast<AnmManager*>(p)->update(true) ? 1 : 5;
        };
        animation_updates[1].run = [](void* p) {
            return static_cast<AnmManager*>(p)->update(false) ? 1 : 5;
        };
        CHECK(scheduler.add(animation_updates[0], FramePass::Update, 9) >= 0);
        CHECK(scheduler.add(animation_updates[1], FramePass::Update, 34) >= 0);
        CHECK(load_bank(manager, 16, title_anm_fixture()));
        CHECK(load_bank(manager, 17, synthetic_anm(1, trivial_scripts(1))));
        CHECK(load_bank(manager, 2, synthetic_anm(8, trivial_scripts(20))));
        CHECK(load_bank(manager, 19, synthetic_anm(4, trivial_scripts(4))));
        CHECK(load_bank(manager, 8, synthetic_anm(4, trivial_scripts(4))));
        scene = std::make_unique<TitleScene>(progress, selection, manager, host, scheduler,
                                             TitleSceneResources{});
        CHECK(scene->error.empty());
        scene->state.return_reason = 0;
    }

    void frame(u32 pressed = 0, u32 repeated = 0) {
        scene->controls({0, pressed, repeated, 0});
        CHECK(scheduler.update() >= 0);
        CHECK(scheduler.draw() >= 0);
    }
    void advance(int frames, u32 pressed = 0, u32 repeated = 0) {
        for (int i = 0; i < frames; i++) frame(pressed, repeated);
    }
    void reach_menu() {
        advance(kTitleBootIntroFrames + kTitleMenuReadyDelay + 3);
        CHECK_EQ(int(scene->state.screen), int(TitleScreen::Main));
        CHECK_EQ(scene->state.substate, 2);
    }
    // 主菜单光标到 Replay(4)：↓ 跳过禁用 1（0→2→3→4）→ Z 确认 → 转屏。
    void enter_replay() {
        for (int i = 0; i < 3; i++) frame(0x20);
        CHECK_EQ(scene->state.menu.cursor, 4);
        frame(0x01);
        advance(25);
        CHECK_EQ(int(scene->state.screen), int(TitleScreen::Replay));
        advance(8);  // case1 age>6 → 列表
        CHECK_EQ(scene->state.substate, 2);
    }
    const ReplayMenuHost::Text* find_text(const char* prefix) {
        for (const auto& text : host.texts)
            if (text.text.rfind(prefix, 0) == 0) return &text;
        return nullptr;
    }
};

constexpr i64 kStamp = 1719226000;  // 顶层头 +0x0C（保存时刻）
}  // namespace

// case0/case1/case2：建面 + 列表导航 + 分页 + 空槽行文本 + X 返回。
TEST(replay_menu_navigates_pages_and_returns) {
    ReplayHarness harness;
    const std::vector<u8> bytes = build_replay(3, 1, 8, 0, kStamp, false, {{1, 0}});
    harness.host.put(0, "th13_01.rpy", bytes);  // th13_01.rpy → 编号槽 0
    harness.reach_menu();
    harness.enter_replay();
    CHECK(harness.scene->state.handles[104] != 0);      // s104 徽记
    CHECK(harness.scene->state.replay_ascii_root != 0); // ascii s19 粒子根
    CHECK(harness.scene->state.menu.cursor == 0);
    CHECK(harness.scene->error.empty());
    // 页 0 行文本：槽 0 = "No.01 <名字> <yy>/<mm>/<dd> .. <角色> <难度> <路线>"、
    // 槽 1 = 空行。颜色 = 选中 ffffff00 / 未选 ff808080；行位 (58, 80+18r)。
    const ReplayFile& file = *harness.host.slots[0];
    std::tm broken{};
    const std::time_t stamp = std::time_t(kStamp);
    CHECK(localtime_r(&stamp, &broken) != nullptr);
    char expected[128];
    std::snprintf(expected, sizeof expected,
                  "No.01 TESTER %.2d/%.2d/%.2d %.2d:%.2d Youmu   Normal  All %2.1f%%",
                  broken.tm_year % 100, broken.tm_mon + 1, broken.tm_mday, broken.tm_hour,
                  broken.tm_min, double(file.slow_rate));
    harness.host.texts.clear();
    harness.frame();
    CHECK(harness.host.texts.size() >= 25);
    CHECK_EQ(harness.host.texts[0].text, std::string(expected));
    CHECK_EQ(harness.host.texts[0].color, 0xffffff00u);
    CHECK_NEAR(harness.host.texts[0].position.x, 58.f, 1e-5);
    CHECK_NEAR(harness.host.texts[0].position.y, 80.f, 1e-5);
    CHECK_EQ(harness.host.texts[1].text,
             std::string("No.02 -------- --/--/-- --:-- ------- ------- --- ---%"));
    CHECK_EQ(harness.host.texts[1].color, 0xff808080u);
    CHECK_NEAR(harness.host.texts[1].position.y, 95.f, 1e-5);  // 80 + 行距 15
    // ↓ 移行（音 10）；→ 换页（页 1 全空用户行 "User …"；0x452300 用户槽 25+）。
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
    CHECK_EQ(harness.host.texts[0].text,
             std::string("User  -------- --/--/-- --:-- ------- ------- --- ---%"));
    // ← 回页 0；X 逐级返回（case5 收面 → 主菜单；徽记/粒子根槽清零）。
    harness.frame(0x40);
    harness.frame(0x100);
    harness.advance(8);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Main));
    CHECK_EQ(harness.scene->state.handles[104], 0u);
    CHECK_EQ(harness.scene->state.replay_ascii_root, 0u);
    CHECK(harness.scene->error.empty());
}

// case2 Z 确认合法录像 → 选关（缺关禁用、行文本 = 下关起始分/顶层最终分）→
// case3 >31 帧排队 start_replay（stage 块链序）。
TEST(replay_menu_stage_select_and_launch) {
    ReplayHarness harness;
    // stages 1..3：起始分 0 / 111 / 2222；顶层最终分 24681357、续关 2。
    const std::vector<u8> bytes =
        build_replay(0, 1, 8, 2, kStamp, false, {{1, 0}, {2, 111}, {3, 2222}});
    harness.host.put(1, "th13_02.rpy", bytes);  // 编号槽 1 = 页 0 行 1
    harness.reach_menu();
    harness.enter_replay();
    harness.frame(0x20);  // ↓ 到行 1
    harness.frame(0x01);  // Z：槽 1 → case4 选关（age 0，头行起于列表位）
    CHECK_EQ(harness.scene->state.substate, 4);
    // 头行滑入（0x452c81-0x452caf）：age<10 时 y = 80 + 行%25×15×(10-age_f)/10。
    // Z 帧内 change_substate 后 age 已 tick 至 1，捕获帧 age=2、行 1 →
    // 80 + 15×8/10 = 92（线性递减至 age≥10 定格 80）。
    harness.host.texts.clear();
    harness.frame();
    CHECK(harness.host.texts.size() == 1);
    CHECK_NEAR(harness.host.texts[0].position.y, 92.f, 1e-4);
    CHECK_NEAR(harness.host.texts[0].position.x, 58.f, 1e-5);
    harness.advance(15);  // age 16 ≥10：滑入定格 80、关行在场（age>9）
    CHECK_EQ(harness.scene->state.menu.cursor, 0);
    CHECK_EQ(harness.scene->state.menu.count, 7);
    // 光标自 0 下移只落在启用行：关 1/2/3 在，关 4-7 缺 → 禁用；↓×3 停在…全禁
    // 用后 move 环绕回 1（MenuCursor 语义：0→1）。
    harness.advance(15);  // case4 age>14 收输入 + 行 age>9 在场
    harness.host.texts.clear();
    harness.frame();
    CHECK(harness.host.texts.size() >= 8);
    // 头行 (58,80)：页 0 格式。
    CHECK_NEAR(harness.host.texts[0].position.x, 58.f, 1e-5);
    CHECK_NEAR(harness.host.texts[0].position.y, 80.f, 1e-5);
    CHECK_EQ(harness.host.texts[0].color, 0xffffffffu);
    // 行 k = 关 k+1：行 0 显示关 2 起始分 111（%.8d%d = "00000111" + 续关数 0），
    // 行 1 显示关 3 起始分，行 2..6（关 3 块在、下关缺）= 顶层最终分 + 续关 2。
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
    // Z 确认关 1 → case3；>31 帧排队 start_replay(1, 链序 0)。
    harness.frame(0x01);
    CHECK_EQ(harness.scene->state.substate, 3);
    harness.advance(33);
    CHECK_EQ(harness.host.started_file, 1);
    CHECK_EQ(harness.host.started_stage, 0);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Replay));
    CHECK(harness.scene->error.empty());
}

// case4 X 回列表（光标/计数还原）；再次确认第 2 关 → start_replay 链序 1。
TEST(replay_menu_stage_escape_returns_and_second_stage) {
    ReplayHarness harness;
    const std::vector<u8> bytes =
        build_replay(0, 1, 8, 0, kStamp, false, {{1, 0}, {2, 111}, {3, 2222}});
    harness.host.put(0, "th13_01.rpy", bytes);
    harness.reach_menu();
    harness.enter_replay();
    harness.frame(0x01);
    CHECK_EQ(harness.scene->state.substate, 4);
    CHECK_EQ(harness.scene->state.menu.cursor, 0);
    harness.advance(15);
    harness.frame(0x20);  // ↓ → 关 2 行
    CHECK_EQ(harness.scene->state.menu.cursor, 1);
    harness.frame(0x100);  // X：回列表
    CHECK_EQ(harness.scene->state.substate, 2);
    CHECK_EQ(harness.scene->state.menu.cursor, 0);
    CHECK_EQ(harness.scene->state.menu.count, 25);
    harness.frame(0x01);  // Z：再确认槽 0 → 选关（光标重置 0）
    CHECK_EQ(harness.scene->state.substate, 4);
    CHECK_EQ(harness.scene->state.menu.cursor, 0);
    harness.advance(15);
    harness.frame(0x20);  // ↓ → 关 2 行
    CHECK_EQ(harness.scene->state.menu.cursor, 1);
    harness.frame(0x01);
    CHECK_EQ(harness.scene->state.substate, 3);
    harness.advance(33);
    CHECK_EQ(harness.host.started_stage, 1);  // 关 2 → 块链序 1
    CHECK(harness.scene->error.empty());
}

// 导入行（末槽 74，页 2 行 24）：Z 触发 request_replay_file；hint 选中新槽；
// request 失败不切屏（错误经 Import 行展示）。
TEST(replay_menu_import_row_and_hint_selection) {
    ReplayHarness harness;
    harness.reach_menu();
    harness.enter_replay();
    // → ×2 到页 2、↓ ×24 到行 24（末槽 74 = 导入行）。
    for (int i = 0; i < 2; i++) harness.frame(0x80);
    for (int i = 0; i < 24; i++) harness.frame(0x20);
    CHECK_EQ(harness.scene->state.menu.cursor, 24);
    harness.host.texts.clear();
    harness.frame();
    CHECK(harness.host.texts.size() >= 25);
    CHECK_EQ(harness.host.texts[24].text,
             std::string("Import  -------- --/--/-- --:-- ------- ------- --- ---%"));
    harness.frame(0x01);
    CHECK_EQ(harness.host.request_calls, 1);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Replay));
    // 导入成功：宿主落槽 + hint → 下一帧菜单选中槽 0（页 0 行 0）。
    const std::vector<u8> bytes = build_replay(3, 1, 8, 0, kStamp, false, {{1, 0}});
    harness.host.put(0, "th13_01.rpy", bytes);
    harness.scene->state.replay_import_hint = 0;
    harness.frame();
    CHECK_EQ(harness.scene->state.menu.cursor, 0);
    CHECK_EQ(harness.scene->state.replay_import_hint, -1);
    harness.frame(0x01);
    CHECK_EQ(harness.scene->state.substate, 4);  // 确认进选关
    CHECK(harness.scene->error.empty());
}

// request_replay_file 失败（非浏览器宿主）：不 fail 菜单、错误文案上 Import 行。
TEST(replay_menu_import_request_failure_stays_operable) {
    ReplayHarness harness;
    harness.host.request_ok = false;
    harness.host.import_error = "Replay import needs the browser runtime";
    harness.reach_menu();
    harness.enter_replay();
    for (int i = 0; i < 2; i++) harness.frame(0x80);
    for (int i = 0; i < 24; i++) harness.frame(0x20);
    harness.host.texts.clear();
    harness.frame(0x01);
    CHECK_EQ(harness.host.request_calls, 1);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Replay));
    CHECK_EQ(harness.scene->state.substate, 2);  // 菜单仍可操作
    harness.host.texts.clear();
    harness.frame();
    CHECK(harness.host.texts.size() >= 25);
    CHECK_EQ(harness.host.texts[24].text,
             std::string("Import  Replay import needs the browser runtime"));
    CHECK(harness.scene->error.empty());
}

// 符卡练习录像（顶层头 +0xA bit1）：Z 跳过选关直进 case3，stage 链序 0。
TEST(replay_menu_spell_practice_skips_stage_select) {
    ReplayHarness harness;
    const std::vector<u8> bytes = build_replay(3, 1, 8, 0, kStamp, true, {{1, 0}});
    harness.host.put(25, "th13_udSp01.rpy", bytes);  // 用户槽 25
    harness.reach_menu();
    harness.enter_replay();
    harness.frame(0x80);  // → 页 1（槽 25 = 行 0）
    harness.frame(0x01);
    CHECK_EQ(harness.scene->state.substate, 3);  // 直进 case3
    harness.advance(33);
    CHECK_EQ(harness.host.started_file, 25);
    CHECK_EQ(harness.host.started_stage, 0);
    CHECK(harness.scene->error.empty());
}

// 空槽 Z 无效（0x452914 entry!=0 门）：仍在列表。
TEST(replay_menu_empty_slot_confirm_is_noop) {
    ReplayHarness harness;
    harness.reach_menu();
    harness.enter_replay();
    harness.frame(0x20);  // 行 1 = 空槽
    harness.frame(0x01);
    CHECK_EQ(harness.scene->state.substate, 2);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Replay));
    CHECK(harness.scene->error.empty());
}

int main() { return th13test::run_all("Replay 菜单"); }
