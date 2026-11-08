// B2 窗口 2 契约：标题 Replay 启动等待期的 ascii.anm script17 祈祷树。
//
// 原版证据（retail v1.00c，只读 Unicorn 归因
// artifacts/round-16/agent-g-menu/segment-attribution.json）：
//   - 0x4525b0 case3 age==2 拍（0x452a75）→ 0x417370 → 0x46f820 在共享 ANM
//     管理器建 ascii.anm script17（480,392）。
//   - script17 op501×2 + op503(16) t=1；op200 t=5 每 4 帧回到 t=1。script16
//     每次出生执行 12 个变量读 = 24 visual index 单位。
//   - 标题 replay 启动等待 31 帧内共出生 8 次 = 192 units（原生 652..680）。
//   - 启动排队后 caption 树按 0x46fc30 卸载，不进入对局。
//
// 本测试用真实 artifacts/reference/raw/ascii.anm，按生产帧序
// （menu.update → age.tick → 副链 update(true) → 主链 update(false)）驱动
// TitleReplayMenu，断言出生次数/总 units 与 retire 线上契约。不伪造抽签。
#include "ui_test_fixtures.hpp"
#include "../harness/test_framework.hpp"

#include "game/TitleReplayMenu.hpp"
#include "game/ReplayFile.hpp"

#include <fstream>
#include <iterator>
#include <string>
#include <vector>

using namespace th13;

namespace {
std::vector<u8> asset(const char* name) {
    std::ifstream file(std::string("artifacts/reference/raw/") + name, std::ios::binary);
    return {std::istreambuf_iterator<char>(file), {}};
}

// 最小平台：1 个手造 stage 块（case3 启动只需 replay_entry/stage 链）。
struct PrayerHost final : TitleReplayPlatform {
    ReplayFile file;
    i32 started_file = -1, started_stage = -1;
    int sounds = 0;
    PrayerHost() {
        file.top_header.assign(0x74, 0);  // 非符卡练习（+0xA bit1 = 0）
        file.stages.resize(1);
        file.stages[0].number = 1;
        file.stages[0].frame_count = 1;
        file.stages[0].header.assign(0xC4, 0);
    }
    i32 replay_count() const override { return 1; }
    const ReplayFile* replay_entry(i32 index) const override {
        return index == 0 ? &file : nullptr;
    }
    std::string replay_filename(i32) const override { return "th13_01.rpy"; }
    bool start_replay(i32 file_index, i32 stage_index) override {
        if (started_file >= 0) return false;
        started_file = file_index;
        started_stage = stage_index;
        return true;
    }
};

// 注册表内仍存在的 ascii script17/16 VM 数（retire 后应为 0）。
int prayer_vms(AnmManager& manager) {
    int count = 0;
    for (u32 bucket = 0; bucket < 33; ++bucket)
        for (const auto* vm : manager.registry.layer(bucket))
            if (vm && (vm->source_script == 17 || vm->source_script == 16)) ++count;
    return count;
}
}  // namespace

TEST(menu_replay_launch_prayer_spawns_eight_times_then_retires) {
    GameRandom random;
    AnmEnvironment environment;
    environment.resolution_scale = 1;
    environment.raster_scale = 1;
    AnmManager manager{random, environment};
    const auto bytes = asset("ascii.anm");
    CHECK(!bytes.empty());
    CHECK(manager.load(2, bytes.data(), u32(bytes.size())));
    CHECK(manager.sprite_fallback(2));

    TitleState state;
    state.screen = TitleScreen::Replay;
    state.substate = 4;  // 选关屏：age>14 + Z 确认 → case3
    state.age.set(15);
    PrayerHost host;
    TitleReplayMenu menu{state, manager, host, TitleSceneResources{}};
    menu.play_sound = [&host](i32) { ++host.sounds; return true; };

    const u32 before = random.index();
    CHECK(menu.update(0x01, 0));  // Z：确认关 1 → case3
    CHECK_EQ(state.substate, 3);
    CHECK_EQ(random.index(), before);  // 进入 case3 当帧不建树（age 尚未到 2）

    const float rate = 1.f;
    // 生产帧序：TitleFrame update(6) → age.tick → 副链(9) → 主链(34)。
    const auto frame = [&](u32 pressed = 0) {
        CHECK(menu.update(pressed, 0));
        state.age.tick(&rate);
        CHECK(manager.update(true));
        CHECK(manager.update(false));
    };

    // 进入 case3 当帧 age=0。i=0/i=1 分别看到 age=0/1，仍未建根。
    frame();
    frame();
    CHECK_EQ(random.index(), before);
    CHECK_EQ(prayer_vms(manager), 0);
    // i=2：age==2 建根（create 只做根首 tick，不抽签），同帧主链走到 t=1
    // 出生 script16 → 12 次变量读 = 24 units。
    frame();
    CHECK_EQ(random.index() - before, 24u);
    CHECK(prayer_vms(manager) > 0);

    // 继续推进：出生节拍 i=2,6,10,...,30 共 8 次（每次 24 units）；i=32
    // （age=32 > 31）排队启动并在同一 update 内 retire 整树。
    for (int i = 3; i <= 32; ++i) frame();
    CHECK_EQ(host.started_file, 0);
    CHECK_EQ(host.started_stage, 0);
    CHECK_EQ(random.index() - before, 192u);   // 8 spawns × 24 units（原生 192）
    CHECK_EQ(prayer_vms(manager), 0);          // 启动排队后 caption 树已卸载
    CHECK(menu.error.empty());
}

int main() { return th13test::run_all("b_menu prayer rng contracts"); }
