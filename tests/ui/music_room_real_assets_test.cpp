// 音楽室（Music Room）真实资产身份回归（native，production TitleMusicRoomMenu /
// TitleMusicRoomView + 原始 title.anm / text.anm / ascii.anm / musiccmt.txt）。
//
// 与旧 title_music_room_view_test.cpp 的区别：旧套件用合成 ANM + 合成宿主
// 字符串，只证明状态机把“某个 CP932 串”交给栅格器；本套件用原版数据，
// 逐字节断言 8 行注释来自 musiccmt.txt 对应曲目块，且 VM 的脚本号/精灵号/
// 纹理 entry/矩形来自真实 ANM。expected 来源：
//   - musiccmt.txt：'@bgm/' 块内曲名 + 8 行注释（0x4724c0 语义）。
//   - text.anm entry0 '@'：scripts 41..48 → sprites 25..32，rect
//     (0,18*i,512,17)，脚本坐标 (64,320+18*i)（0x4554f0 注释块）。
//   - title.anm entry21 '@'：scripts 160..176 → sprites 132..148，rect
//     (0,18*i,512,18)，脚本坐标 (64,96+20*i)（0x4554f0 曲目行）。
//   - title.anm script106 → sprite84 / entry6 'title/select01.png'（徽记）。
//   - ascii.anm script19 = 粒子根（TitleMusicRoomView.cpp 头注）。
// 字形轮廓仍是 BLOCKED：本套件只认证“生产 view/menu 交给栅格回调的文本字节 +
// 资源身份”，不认证 GDI/SDL_ttf 逐像素字形；生产栅格器的实际提交由
// web/tests/sdl/music-room-text-identity.test.mjs 的 submission trace 覆盖。
#include "ui_test_fixtures.hpp"
#include "../harness/test_framework.hpp"

#include "game/TitleMusicRoomView.hpp"

#include <cmath>
#include <cstdio>
#include <map>
#include <string>
#include <vector>

using namespace th13;
using namespace th13::test;

namespace {

bool read_file(const std::string& path, std::vector<u8>& out) {
    std::FILE* file = std::fopen(path.c_str(), "rb");
    if (!file) return false;
    std::fseek(file, 0, SEEK_END);
    const long size = std::ftell(file);
    std::fseek(file, 0, SEEK_SET);
    if (size < 0) { std::fclose(file); return false; }
    out.resize(size_t(size));
    const bool ok = std::fread(out.data(), 1, out.size(), file) == out.size();
    std::fclose(file);
    return ok;
}

struct Track {
    std::string name, title;
    std::vector<std::string> comments;
};

// musiccmt.txt：'@' 起块；块内 1 曲名 + 8 注释；CRLF 剥 \r。
std::vector<Track> parse_musiccmt(const std::vector<u8>& bytes) {
    std::vector<std::string> lines;
    size_t start = 0;
    for (size_t i = 0; i <= bytes.size(); ++i) {
        if (i != bytes.size() && bytes[i] != '\n') continue;
        size_t end = i;
        if (end > start && bytes[end - 1] == '\r') --end;
        lines.emplace_back(reinterpret_cast<const char*>(bytes.data() + start), end - start);
        start = i + 1;
    }
    std::vector<Track> tracks;
    for (size_t i = 0; i < lines.size(); ++i) {
        const std::string& line = lines[i];
        if (line.size() < 4 || line[0] != '@') continue;
        const size_t slash = line.find('/');
        if (slash == std::string::npos) continue;
        Track track;
        track.name = line.substr(slash + 1);
        for (u32 k = 0; k < 9; ++k) {
            if (i + 1 >= lines.size()) break;
            if (!lines[i + 1].empty() && lines[i + 1][0] == '@') break;
            ++i;
            if (k == 0) track.title = lines[i];
            else track.comments.push_back(lines[i]);
        }
        tracks.push_back(std::move(track));
    }
    return tracks;
}

std::string locked_title(i32 track) {
    char prefix[16];
    std::snprintf(prefix, sizeof prefix, "No.%2d ", int(track) + 1);
    std::string text = prefix;
    for (i32 i = 0; i < 11; ++i) text += "\x81\x48";
    return text;
}

struct MusicHost final : TitleMusicRoomPlatform {
    explicit MusicHost(const std::vector<Track>& source) : tracks(source) {}
    const std::vector<Track>& tracks;
    i32 played = -1, marked = -1, locked = 0;
    i32 track_count() const override { return i32(tracks.size()); }
    std::string track_title(i32 track) override {
        return track >= 0 && u32(track) < tracks.size() ? tracks[u32(track)].title : std::string{};
    }
    std::string track_comment(i32 track, i32 line) override {
        return track >= 0 && u32(track) < tracks.size() && line >= 0 &&
                       u32(line) < tracks[u32(track)].comments.size()
                   ? tracks[u32(track)].comments[u32(line)]
                   : std::string{};
    }
    bool track_heard(i32) const override { return false; }  // 新档全未听
    void mark_heard(i32 track) override { marked = track; }
    bool play_track(i32 track) override { played = track; return true; }
    bool play_locked_response() override { ++locked; return true; }
    bool resume_title_music() override { return true; }
};

struct Call {
    AnmVm* vm = nullptr;
    u32 color = 0;
    std::string text;
    AnmTextAlignment alignment = AnmTextAlignment::Left;
};

AnmVm* find_vm(AnmManager& manager, i32 bank, i32 script) {
    for (u32 layer = 0; layer < 33; ++layer)
        for (AnmVm* vm : manager.registry.layer(layer))
            if (vm && vm->source_script == script && manager.resource_id(vm->resource) == bank)
                return vm;
    return nullptr;
}

bool rect_is(const AnmSprite* sprite, float x, float y, float width, float height) {
    return sprite && sprite->x == x && sprite->y == y && sprite->width == width &&
           sprite->height == height;
}

struct Harness {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    TitleState state;
    MusicHost host;
    TitleMusicRoomMenu menu{state, host};
    TitleMusicRoomView view{manager, 16, 0, 2};
    std::map<i32, Call> last;  // script → 最近一次栅格调用
    std::vector<Call> navigation_calls;
    bool recording_navigation = false;
    Harness(const std::vector<Track>& tracks) : host(tracks) {
        state.screen = TitleScreen(kTitleScreenMusicRoom);
        menu.play_sound = [](i32) { return true; };
    }
    bool load(const std::vector<u8>& text, const std::vector<u8>& ascii,
              const std::vector<u8>& title) {
        if (!load_bank(manager, 0, text)) return false;
        if (!load_bank(manager, 2, ascii)) return false;
        if (!manager.sprite_fallback(2)) return false;
        return load_bank(manager, 16, title);
    }
    bool frame(u32 pressed = 0, u32 repeated = 0) {
        // 生产帧序（ApplicationState 调度器）：ANM 主链 update → 场景 update
        // → ANM 副链 update。主链 update 同时重建 registry 层桶，供 find_vm
        // 扫描徽记/粒子；也让行位置插值按生产节奏推进。
        if (!manager.update(false)) return false;
        if (!menu.update(pressed, repeated)) return false;
        if (!view.update(state, menu, host,
            [this](AnmVm& vm, u32 color, const std::string& text, AnmTextAlignment alignment) {
                const Call call{&vm, color, text, alignment};
                last[vm.source_script] = call;
                if (recording_navigation) navigation_calls.push_back(call);
                return true;
            })) return false;
        return manager.update(true);
    }
    void advance(i32 frames, u32 pressed = 0, u32 repeated = 0) {
        for (i32 i = 0; i < frames; ++i) CHECK(frame(pressed, repeated));
    }
    bool expect_text(i32 script, const std::string& text, u32 color) {
        const auto it = last.find(script);
        if (it == last.end()) {
            std::printf("  缺少 script %d 的栅格调用\n", int(script));
            return false;
        }
        if (it->second.text != text) {
            std::printf("  script %d 文本不一致（实际 %zu 字节，期望 %zu 字节）\n",
                        int(script), it->second.text.size(), text.size());
            return false;
        }
        if (it->second.color != color) {
            std::printf("  script %d 颜色 %08x != %08x\n", int(script),
                        unsigned(it->second.color), unsigned(color));
            return false;
        }
        return it->second.alignment == AnmTextAlignment::Left;
    }
    bool expect_comment_identity(i32 line) {
        const auto it = last.find(41 + line);
        if (it == last.end() || !it->second.vm) return false;
        AnmVm* vm = it->second.vm;
        if (vm->source_script != 41 + line || vm->visual.sprite != 25 + line) return false;
        if (!rect_is(vm->sprite, 0, 18.f * line, 512, 17)) return false;
        return vm->sprite && vm->sprite->texture == 0 && vm->sprite_resource &&
               vm->sprite_resource->textures.size() > 0 &&
               vm->sprite_resource->textures[0].name == "@";
    }
    bool expect_row_identity(i32 row) {
        const auto it = last.find(160 + row);
        if (it == last.end() || !it->second.vm) return false;
        AnmVm* vm = it->second.vm;
        if (vm->source_script != 160 + row || vm->visual.sprite != 132 + row) return false;
        if (!rect_is(vm->sprite, 0, 18.f * row, 512, 18)) return false;
        return vm->sprite && vm->sprite->texture == 21 && vm->sprite_resource &&
               vm->sprite_resource->textures.size() > 21 &&
               vm->sprite_resource->textures[21].name == "@";
    }
};

}  // namespace

TEST(music_room_real_assets_comment_text_and_anm_identity) {
    std::vector<u8> music_bytes, text_bytes, title_bytes, ascii_bytes;
    CHECK(read_file("artifacts/reference/raw/musiccmt.txt", music_bytes));
    CHECK(read_file("artifacts/reference/raw/text.anm", text_bytes));
    CHECK(read_file("artifacts/reference/raw/title.anm", title_bytes));
    CHECK(read_file("artifacts/reference/raw/ascii.anm", ascii_bytes));
    if (music_bytes.empty() || text_bytes.empty() || title_bytes.empty() || ascii_bytes.empty())
        return;
    const std::vector<Track> tracks = parse_musiccmt(music_bytes);
    CHECK_EQ(tracks.size(), 17u);
    for (const auto& track : tracks) CHECK_EQ(track.comments.size(), 8u);
    // 原版 track0 的空白注释行 = 单个 CP932 全角空格（0x81 0x40）。
    CHECK_EQ(tracks[0].comments[1], std::string("\x81\x40", 2));
    CHECK_EQ(tracks[0].comments[3], std::string("\x81\x40", 2));
    CHECK(tracks[0].comments[0] != tracks[10].comments[0]);

    Harness h(tracks);
    CHECK(h.load(text_bytes, ascii_bytes, title_bytes));
    const u32 baseline = h.manager.registry.count();

    // 进入音楽室并显影 8 行（case0 age≥10 → case1 隔帧 → case2）。
    h.advance(16);
    CHECK_EQ(h.state.substate, 2);
    h.advance(15);
    CHECK_EQ(h.menu.revealed_lines(), 8);

    // 8 行注释：文本逐字节 = musiccmt.txt track0；色 0xffffff；左对齐；
    // VM 脚本/精灵/纹理/矩形 = 原版 text.anm entry0 scripts41..48。
    for (i32 line = 0; line < 8; ++line) {
        CHECK(h.expect_text(41 + line, tracks[0].comments[u32(line)], 0xffffffu));
        CHECK(h.expect_comment_identity(line));
        CHECK_EQ(h.last[41 + line].vm->visual.sprite, 25 + line);
        const AnmVm* vm = h.last[41 + line].vm;
        CHECK_EQ(vm->sprite->x, 0.f);
        CHECK_EQ(vm->sprite->y, 18.f * line);
        CHECK_EQ(vm->sprite->width, 512.f);
        CHECK_EQ(vm->sprite->height, 17.f);
        CHECK_EQ(vm->variables.position.x, 64.f);
        CHECK_EQ(vm->variables.position.y, 320.f + 18.f * line);
    }
    // 17 行曲目：未听 = "No.%2d " + 11 全角？；身份 = title.anm entry21
    // scripts160..176 / sprites132..148 / 空白纹理 entry21。
    for (i32 row = 0; row < i32(tracks.size()); ++row) {
        CHECK(h.expect_text(160 + row, locked_title(row), 0xffffffu));
        CHECK(h.expect_row_identity(row));
        CHECK_EQ(h.last[160 + row].vm->visual.sprite, 132 + row);
        CHECK_EQ(h.last[160 + row].vm->sprite->texture, 21);
    }
    // 徽记 title.anm 106 / sprite84 / entry6 select01.png；粒子 ascii 19。
    AnmVm* badge = find_vm(h.manager, 16, 106);
    CHECK(badge != nullptr);
    if (badge) {
        CHECK_EQ(badge->visual.sprite, 84);
        CHECK_EQ(badge->sprite->texture, 6);
        CHECK_EQ(badge->sprite_resource->textures[6].name, std::string("title/select01.png"));
    }
    CHECK(find_vm(h.manager, 2, 19) != nullptr);

    // 记录 8 行注释 / 17 行曲目的 VM 句柄，翻页后必须仍是同一对象。
    u32 comment_handles[8]{}, row_handles[17]{};
    for (i32 line = 0; line < 8; ++line)
        comment_handles[line] = h.manager.registry.handle(*h.last[41 + line].vm);
    for (i32 row = 0; row < i32(tracks.size()); ++row)
        row_handles[row] = h.manager.registry.handle(*h.last[160 + row].vm);

    // ↓×10：row=10 → 10 行窗口 top=1（可见 1..10）；导航不重栅格文本。
    h.recording_navigation = true;
    h.navigation_calls.clear();
    h.advance(10, 0x20u);
    h.advance(30);
    h.recording_navigation = false;
    CHECK_EQ(h.menu.row(), 10);
    CHECK(h.navigation_calls.empty());
    for (i32 line = 0; line < 8; ++line) {
        AnmVm* vm = h.manager.registry.find(comment_handles[line]);
        CHECK(vm == h.last[41 + line].vm);
        CHECK(vm && vm->source_script == 41 + line && vm->visual.sprite == 25 + line);
    }
    for (i32 row = 0; row < i32(tracks.size()); ++row) {
        AnmVm* vm = h.manager.registry.find(row_handles[row]);
        CHECK(vm == h.last[160 + row].vm);
        CHECK(vm && vm->source_script == 160 + row && vm->visual.sprite == 132 + row);
        const bool visible = (vm->visual.flags & 2u) != 0;
        CHECK_EQ(visible, row >= 1 && row <= 10);
    }

    // Z 第一次（曲 10 未听）：锁定占位 8 行，色 0x8080ff，同一 8 个 VM/矩形。
    h.last.clear();
    CHECK(h.frame(0x80001u));
    CHECK_EQ(h.host.locked, 1);
    CHECK(h.menu.locked_armed());
    CHECK_EQ(h.menu.comment_track(), 10);
    h.advance(15);
    CHECK_EQ(h.menu.revealed_lines(), 8);
    for (i32 line = 0; line < 8; ++line) {
        CHECK(h.last.count(41 + line) == 1);
        CHECK_EQ(h.last[41 + line].color, 0x8080ffu);
        CHECK_EQ(h.last[41 + line].vm, h.manager.registry.find(comment_handles[line]));
        CHECK_EQ(h.last[41 + line].vm->visual.sprite, 25 + line);
        CHECK(rect_is(h.last[41 + line].vm->sprite, 0, 18.f * line, 512, 17));
    }

    // Z 第二次：播放曲 10，真实注释逐字节 = musiccmt.txt track10；色白；
    // 仍是同一 8 个 VM/脚本/精灵/纹理/矩形（身份保持）。
    h.last.clear();
    CHECK(h.frame(0x80001u));
    CHECK_EQ(h.host.played, 10);
    CHECK_EQ(h.host.marked, 0);
    CHECK(!h.menu.locked_armed());
    h.advance(15);
    CHECK_EQ(h.menu.revealed_lines(), 8);
    for (i32 line = 0; line < 8; ++line) {
        CHECK(h.expect_text(41 + line, tracks[10].comments[u32(line)], 0xffffffu));
        CHECK(h.expect_comment_identity(line));
        CHECK_EQ(h.last[41 + line].vm, h.manager.registry.find(comment_handles[line]));
    }

    // 收面回收。
    h.view.release();
    CHECK(h.manager.update());
    CHECK_EQ(h.manager.registry.count(), baseline);
}

int main() {
    std::printf("# music_room_real_assets_test\n");
    return th13test::run_all("music_room_real_assets_test");
}
