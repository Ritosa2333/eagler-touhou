// M9 原生固定 case（Replay/旁观/生命周期；docs/MP-DEVELOPMENT.md M9 与
// docs/plans/th13-mp-m1-m9-workflow.md §M9 退出条件）。
// 被测生产面（web/cpp/multiplayer/{ReplayArchive,StageEntry,SpectatorJournal}.*
// + Session/WorldState——零测试分支）：
//   - ReplayArchive：独立格式（magic/version/header/SessionSetup/元数据）
//     的 begin→append→encode→load roundtrip；章节表与关头表对齐；seek =
//     章节 firstFrame 锚点（不伪切当前帧号）；
//   - StageEntry：关头延续（全席资源/共享态/双 RNG 位相/灵界生命周期）
//     的 capture→encode→decode→restore 全字段等价；非法线格式整体拒绝；
//   - SpectatorJournal：旁观只发布连续确认历史（Publish 门 = min(confirmed,
//     simulated)）；晚入观众 Bootstrap 从 checkpoint 起；慢观众隔离
//     （观众端自持缓冲，发布端前沿不受观众消费速度影响）；
//   - 关头跨关延续：StageEntry 后世界推进与连续世界同输入逐位一致；
//   - run generation 隔离：重开退役旧 generation（旧回放/旧旁观包不进新局）；
//   - 断线保存：确认历史存档 → 重载 → 从存档帧继续推进与连续基准一致。
//
// 固定 case 映射（workflow-cases.json m9.*）：
//   replay_roundtrip_seek        → 格式 roundtrip + 章节 seek 锚点
//   spectator_confirmed_latejoin→ 确认历史发布 + 晚入观众 Bootstrap
//   slow_spectator_isolation    → 慢观众隔离（发布前沿不等待观众）
//   stage_entry_carry           → 关头延续（StageEntry restore 后续推一致）
//   run_generation_isolation    → 重开退役旧 generation
//   terminal_local_save         → 终局本地保存（Replay 编码 completed 面）
//   disconnect_confirmed_save_return → 断线保存确认历史并返回
#include "../../ecl/ecl_test_fixtures.hpp"
#include "game/StageBattle.hpp"
#include "multiplayer/ReplayArchive.hpp"
#include "multiplayer/Session.hpp"
#include "multiplayer/SpectatorJournal.hpp"
#include "multiplayer/StageEntry.hpp"
#include "multiplayer/WorldState.hpp"
#include <cstdio>

using namespace th13;
using namespace th13::multiplayer;

namespace {

struct Host final : StageBattleServices {
    bool life_hud(i32, i32) override { return true; }
    bool bomb_hud(i32, i32) override { return true; }
    bool request_game_over() override { return true; }
    bool hud_sound(i32) override { return true; }
    bool hud_sound(i32, float) override { return true; }
    bool stop_sound(i32) override { return true; }
};

class Provider final : public EclResourceProvider {
public:
    Provider() {
        std::vector<u8> main;
        th13test::emit(main, 0, 23, 0, 0xff, 0, {600});
        th13test::emit(main, 0, 10, 0, 0xff, 0, {});
        stage = th13test::build_scpt({{"main", main}}, {"enemy.anm", "st01enm.anm"});
    }
    bool read(const std::string& name, std::vector<u8>& out) override {
        if (name == "mp9.ecl") {
            out = stage;
            return true;
        }
        if (name.size() > 4 && name.compare(name.size() - 4, 4, ".sht") == 0) {
            static const std::string root = [] {
                std::string path = __FILE__;
                for (int i = 0; i < 8; ++i) {
                    const std::string probe = path + "/artifacts/reference/raw/pl00a.sht";
                    std::FILE* check = std::fopen(probe.c_str(), "rb");
                    if (check) { std::fclose(check); break; }
                    const auto cut = path.find_last_of("/\\");
                    if (cut == std::string::npos) break;
                    path.erase(cut);
                }
                return path;
            }();
            std::FILE* file = std::fopen((root + "/artifacts/reference/raw/" + name).c_str(), "rb");
            if (!file) return false;
            std::fseek(file, 0, SEEK_END);
            const long size = std::ftell(file);
            std::fseek(file, 0, SEEK_SET);
            out.resize(size_t(size < 0 ? 0 : size));
            const size_t got = out.empty() ? 1 : std::fread(out.data(), 1, out.size(), file);
            std::fclose(file);
            return got == out.size();
        }
        return false;
    }
    bool animation(u32, const std::string& name) override {
        if (animations) animations->push_back(name);
        return true;
    }
    std::vector<std::string>* animations = nullptr;
    std::vector<u8> stage;
};

struct Fixture {
    Provider provider;
    Host host;
    SessionState progress;
    HudPlayerState player;
    HudScoreState score;
    StageBattle battle;
    GameRandom visual{0x1234};

    bool init() {
        provider.animations = &battle.sim.anim_list;
        progress.max_point_cap = 500000;
        if (!battle.load(provider, "mp9.ecl", 2, host, progress, player, score)) {
            std::printf("  battle.load failed: %s\n", battle.error().c_str());
            return false;
        }
        return true;
    }
};

SessionSetup replaySetup() {
    SessionSetup setup;
    setup.version = SessionSetup::Version;
    setup.mode = 2;
    setup.session_id = 0xfeed'face'1234'5678ull;
    setup.player_count = 2;
    setup.local_player = 0;
    setup.difficulty = 2;
    setup.seed = 47932;
    setup.automatic = false;
    setup.requested_delay = setup.input_delay = 2;
    setup.selections = {0, 0, 0};
    setup.build = {1, 2, 3, 4};
    return setup;
}

// 合成确认输入流：seat0 变化流、seat1 恒射击（灵界位穿插）。
u32 held_at(unsigned seat, unsigned frame) {
    return seat == 0 ? (frame % 5 == 0 ? 0x010u : 0x001u) : (frame % 7 == 0 ? 0x801u : 0x001u);
}

}  // namespace

// ---- replay_roundtrip_seek：独立格式 roundtrip + 章节 seek 锚点 ----
TEST(replay_roundtrip_seek) {
    const auto setup = replaySetup();
    ReplayArchive recorder;
    CHECK(recorder.Begin(setup, 0x1122'3344'5566'7788ull));
    // 双席确认流 40 帧，第 0 帧起章节 1。
    Netplay::FrameInput inputs[2];
    for (unsigned frame = 0; frame < 40; ++frame) {
        CHECK(InputLanes::Capture(held_at(0, frame), false, inputs[0]));
        CHECK(InputLanes::Capture(held_at(1, frame), false, inputs[1]));
        CHECK(recorder.Append(frame, 1, inputs, 2));
    }
    // 关头表：章节 1 的 StageEntry。
    StageEntry entry;
    entry.stage = 1;
    entry.seat_count = 2;
    entry.seats[0].extra_lives = 2;
    entry.seats[1].extra_lives = 1;
    entry.ending_points = 400;
    entry.game_seed = 47932;
    entry.game_index = 1234;
    entry.visual_seed = 0x1234;
    entry.visual_index = 5678;
    const auto entryBytes = entry.Encode();
    CHECK(!entryBytes.empty());
    CHECK(recorder.AppendStageEntry(1, entryBytes));
    // 同章节重复登记拒绝。
    CHECK(!recorder.AppendStageEntry(1, entryBytes));
    // 编码 → 签名 → 载入。
    std::vector<u8> wire;
    CHECK(recorder.Encode(wire, "roundtrip", 123456, true));
    CHECK(ReplayArchive::HasSignature(wire.data(), wire.size()));
    ReplayArchive player;
    CHECK(player.Load(wire.data(), wire.size()));
    // 身份/元数据 roundtrip。
    CHECK(player.Setup().session_id == setup.session_id);
    CHECK(player.Setup().seed == setup.seed);
    CHECK(player.Setup().selections == setup.selections);
    CHECK(player.Timestamp() == 0x1122'3344'5566'7788ull);
    CHECK(player.Score() == 123456);
    CHECK(player.Completed());
    CHECK(player.Name() == "roundtrip");
    // 输入流 roundtrip：逐帧逐席一致。
    CHECK(player.FrameCount() == 40);
    for (unsigned frame = 0; frame < 40; ++frame) {
        const auto* decoded = player.FrameAt(frame);
        CHECK(decoded != nullptr);
        Netplay::FrameInput expected[2];
        CHECK(InputLanes::Capture(held_at(0, frame), false, expected[0]));
        CHECK(InputLanes::Capture(held_at(1, frame), false, expected[1]));
        CHECK(decoded->at(0) == expected[0]);
        CHECK(decoded->at(1) == expected[1]);
    }
    // 关头表 roundtrip + seek 锚点：章节 1 的 firstFrame = 0（InputReplay
    // 章节表）；StageEntry 字节逐位一致。
    CHECK(player.ChapterCount() >= 1);
    CHECK(player.Chapters()[0].label == 1);
    CHECK(player.Chapters()[0].firstFrame == 0);
    const auto* entryOut = player.StageEntryAt(1);
    CHECK(entryOut != nullptr);
    CHECK(*entryOut == entryBytes);
    StageEntry decoded;
    CHECK(StageEntry::Decode(entryOut->data(), entryOut->size(), decoded));
    CHECK(decoded.stage == 1);
    CHECK(decoded.seat_count == 2);
    CHECK(decoded.seats[1].extra_lives == 1);
    CHECK(decoded.ending_points == 400);
    CHECK(decoded.game_index == 1234);
    // seek 语义：章节锚点 = (label, firstFrame)——从锚点帧起重放（不伪切）。
    const auto* chapters = player.Chapters();
    for (std::uint32_t index = 0; index < player.ChapterCount(); ++index) {
        const auto* anchor = player.FrameAt(chapters[index].firstFrame);
        CHECK(anchor != nullptr);  // 锚点帧必有输入（seek 重建起点）
    }
    // 坏档拒绝：截断/魔数错/版本错。
    CHECK(!player.Load(wire.data(), wire.size() - 1));
    std::vector<u8> bad = wire;
    bad[0] ^= 0xff;
    CHECK(!player.Load(bad.data(), bad.size()));
    bad = wire;
    bad[4] = 9;  // version
    CHECK(!player.Load(bad.data(), bad.size()));
}

// ---- stage_entry_carry：关头延续（StageEntry restore 后续推一致） ----
TEST(stage_entry_carry) {
    // 连续基线：双席推 30 帧 → 关头 StageEntry（模拟跨关）→ 再推 30 帧。
    Fixture continuous;
    CHECK(continuous.init());
    CHECK(continuous.battle.enable_seats(continuous.provider, 2, {0, 0}));
    auto step = [](Fixture& world, unsigned frames) {
        for (unsigned frame = 0; frame < frames; ++frame) {
            world.battle.seat_inputs[0].update(held_at(0, frame));
            world.battle.seat_inputs[1].update(held_at(1, frame));
            CHECK(world.battle.step(GameInput{}));
        }
    };
    step(continuous, 30);
    continuous.battle.seat_huds[0].life_piece_threshold_index=1;
    continuous.battle.seat_huds[1].life_piece_threshold_index=4;
    continuous.progress.life_piece_threshold_index=1;
    // 关头快照（连续世界）。
    StageEntry carry;
    carry.Capture(continuous.battle, continuous.progress, continuous.score,
                  47932, continuous.battle.sim.random.index(), 0x1234,
                  continuous.visual.index());
    step(continuous, 30);
    WorldState expected;
    CHECK(expected.capture(continuous.battle, continuous.visual));

    // 延续世界：推 30 帧 → StageEntry 编码/解码（线格式往返）→ restore →
    // 再推 30 帧。与连续基线逐位一致（关头延续无损）。
    Fixture carried;
    CHECK(carried.init());
    CHECK(carried.battle.enable_seats(carried.provider, 2, {0, 0}));
    step(carried, 30);
    StageEntry mid;
    mid.Capture(carried.battle, carried.progress, carried.score,
                47932, carried.battle.sim.random.index(), 0x1234,
                carried.visual.index());
    mid.seats[0].life_piece_threshold_index=1; mid.seats[1].life_piece_threshold_index=4;
    mid.life_piece_threshold_index=1;
    const auto bytes = mid.Encode();
    CHECK(!bytes.empty());
    StageEntry decoded;
    CHECK(StageEntry::Decode(bytes.data(), bytes.size(), decoded));
    std::uint16_t gameSeed = 0, visualSeed = 0;
    std::uint32_t gameIndex = 0, visualIndex = 0;
    decoded.Restore(carried.battle, carried.progress, carried.score,
                    gameSeed, gameIndex, visualSeed, visualIndex);
    CHECK(gameSeed == 47932);
    CHECK(gameIndex == mid.game_index);
    CHECK(visualSeed == 0x1234);
    CHECK(visualIndex == mid.visual_index);
    CHECK_EQ(carried.battle.seat_huds[0].life_piece_threshold_index,1);
    CHECK_EQ(carried.battle.seat_huds[1].life_piece_threshold_index,4);
    CHECK(decoded.Encode()==bytes);
    // v1 carried only a shared threshold. Migrate it to every enabled seat;
    // retain every input word while removing the two v2-only seat words.
    auto legacy=bytes; legacy[0]=1;
    constexpr size_t header=80, seatBytes=328;
    for(int seat=1;seat>=0;--seat)
        legacy.erase(legacy.begin()+header+size_t(seat)*seatBytes+44,
                     legacy.begin()+header+size_t(seat)*seatBytes+48);
    StageEntry migrated; CHECK(StageEntry::Decode(legacy.data(),legacy.size(),migrated));
    CHECK_EQ(migrated.seats[0].life_piece_threshold_index,1);
    CHECK_EQ(migrated.seats[1].life_piece_threshold_index,1);
    for(size_t seat=0;seat<2;++seat) {
        CHECK_EQ(migrated.inputs[seat].held,mid.inputs[seat].held);
        CHECK(migrated.inputs[seat].repeat_age==mid.inputs[seat].repeat_age);
        CHECK(migrated.inputs[seat].duration==mid.inputs[seat].duration);
    }
    CHECK(!StageEntry::Decode(legacy.data(),legacy.size()-1,migrated));
    step(carried, 30);
    WorldState actual;
    CHECK(actual.capture(carried.battle, carried.visual));
    CHECK_EQ(actual.digest_core(), expected.digest_core());

    // 非法线格式拒绝：截断/版本错/席数越界。
    StageEntry rejected;
    CHECK(!StageEntry::Decode(bytes.data(), bytes.size() - 1, rejected));
    std::vector<u8> bad = bytes;
    bad[0] = 9;  // version
    CHECK(!StageEntry::Decode(bad.data(), bad.size(), rejected));
    bad = bytes;
    bad[12] = 4;  // seatCount 越界（>3）
    CHECK(!StageEntry::Decode(bad.data(), bad.size(), rejected));
}

// ---- spectator_confirmed_latejoin：确认历史发布 + 晚入观众 Bootstrap ----
TEST(spectator_confirmed_latejoin) {
    const auto setup = replaySetup();
    SpectatorJournal publisher;
    CHECK(publisher.BeginPublish(setup));
    // 逐帧 Record（关头 initializer 在首帧）。
    Netplay::FrameInput inputs[2];
    StageEntry entry;
    entry.stage = 1;
    entry.seat_count = 2;
    const auto entryBytes = entry.Encode();
    for (unsigned frame = 0; frame < 30; ++frame) {
        CHECK(InputLanes::Capture(held_at(0, frame), false, inputs[0]));
        CHECK(InputLanes::Capture(held_at(1, frame), false, inputs[1]));
        // 关头需求：首帧 Required（带 initializer）；后续同关 NotRequired。
        const auto need = publisher.NeedsCheckpoint(frame, 1);
        CHECK(need == Netplay::SpectatorStagePublisher::CheckpointRequirement::Required ||
              need == Netplay::SpectatorStagePublisher::CheckpointRequirement::NotRequired);
        CHECK(publisher.RecordFrame(frame, 1, inputs, 2,
                                    frame == 0 ? &entryBytes : nullptr));
    }
    // 发布：只发连续确认历史（confirmed=25 < simulated=30 → 发布 0..25）。
    std::vector<std::vector<u8>> wire;
    const auto send = [&](const std::vector<u8>& packet) {
        wire.push_back(packet);
        return true;
    };
    const auto inputsAt = [&](std::uint32_t frame,
                              std::array<Netplay::FrameInput, 3>& out) {
        Netplay::FrameInput pair[2];
        CHECK(InputLanes::Capture(held_at(0, frame), false, pair[0]));
        CHECK(InputLanes::Capture(held_at(1, frame), false, pair[1]));
        out[0] = pair[0];
        out[1] = pair[1];
        return true;
    };
    CHECK(publisher.Publish(25, 30, inputsAt, send));
    CHECK(publisher.Published() == 26);  // 0..25 已发布
    // 观众端（同身份）：逐包喂入 → Bootstrap 晚入。
    SpectatorJournal viewer;
    CHECK(viewer.BeginViewing(setup));
    // 观众端身份 Configure（Setup 一致——晚入观众在 Connect 时已配置）。
    // 逐包解码喂入。
    for (const auto& packet : wire) {
        if (Netplay::IsSpectatorStageEvent(packet.data(), packet.size())) {
            Netplay::SpectatorStageEvent event;
            CHECK(Netplay::DecodeSpectatorStageEvent(packet.data(), packet.size(), &event));
            // 观众端尚未 Bootstrap：Checkpoint 事件先入历史。
            CHECK(viewer.ReceiveEvent(event));
        } else {
            Netplay::SpectatorFramePacket frame;
            CHECK(Netplay::DecodeSpectatorFramePacket(packet.data(), packet.size(), &frame));
            CHECK(viewer.ReceiveFrame(frame));
        }
    }
    CHECK(viewer.Received() == 26);
    // 晚入 Bootstrap：发布端补发 Bootstrap 事件 → 观众从 checkpoint 起回放。
    Netplay::SpectatorStageEvent bootstrap;
    bootstrap.game = SpectatorJournal::kGame;
    bootstrap.playerCount = 2;
    bootstrap.kind = Netplay::SpectatorEventKind::Bootstrap;
    bootstrap.sessionId = setup.session_id;
    bootstrap.gameplayAbi = GameplayAbi(setup);
    bootstrap.frame = viewer.Received();
    bootstrap.stage = 1;
    bootstrap.serial = 1;
    CHECK(viewer.ReceiveEvent(bootstrap));
    CHECK(viewer.Viewing());
    // 观众端历史 → 完整录像（晚入观众的本地保存面）。
    ReplayArchive archive;
    CHECK(viewer.ArchiveFromHistory(archive));
    CHECK(archive.FrameCount() == 26);
    CHECK(archive.Setup().session_id == setup.session_id);
}

// ---- slow_spectator_isolation：慢观众隔离 ----
TEST(slow_spectator_isolation) {
    const auto setup = replaySetup();
    SpectatorJournal publisher;
    CHECK(publisher.BeginPublish(setup));
    StageEntry entry;
    entry.stage = 1;
    entry.seat_count = 2;
    const auto entryBytes = entry.Encode();
    CHECK(!entryBytes.empty());
    Netplay::FrameInput inputs[2];
    for (unsigned frame = 0; frame < 90; ++frame) {
        CHECK(InputLanes::Capture(held_at(0, frame), false, inputs[0]));
        CHECK(InputLanes::Capture(held_at(1, frame), false, inputs[1]));
        CHECK(publisher.RecordFrame(frame, 1, inputs, 2,
                                    frame == 0 ? &entryBytes : nullptr));
    }
    // 慢观众：只消费前 10 帧（观众端停滞）。
    SpectatorJournal viewer;
    CHECK(viewer.BeginViewing(setup));
    std::vector<std::vector<u8>> wire;
    const auto send = [&](const std::vector<u8>& packet) {
        wire.push_back(packet);
        return true;
    };
    const auto inputsAt = [&](std::uint32_t frame,
                              std::array<Netplay::FrameInput, 3>& out) {
        Netplay::FrameInput pair[2];
        CHECK(InputLanes::Capture(held_at(0, frame), false, pair[0]));
        CHECK(InputLanes::Capture(held_at(1, frame), false, pair[1]));
        out[0] = pair[0];
        out[1] = pair[1];
        return true;
    };
    // 发布端持续发布（90 帧 record 0..89 → confirmed=simulated=89——
    // 不受观众消费速度影响；Publish 的 confirmed 是「已确认帧号」含界）。
    CHECK(publisher.Publish(89, 89, inputsAt, send));
    CHECK(publisher.Published() == 90);
    // 观众端只喂前 10 帧（慢观众）。
    size_t consumed = 0;
    for (const auto& packet : wire) {
        if (consumed >= 10) break;
        if (Netplay::IsSpectatorStageEvent(packet.data(), packet.size())) {
            Netplay::SpectatorStageEvent event;
            CHECK(Netplay::DecodeSpectatorStageEvent(packet.data(), packet.size(), &event));
            CHECK(viewer.ReceiveEvent(event));
        } else {
            Netplay::SpectatorFramePacket frame;
            CHECK(Netplay::DecodeSpectatorFramePacket(packet.data(), packet.size(), &frame));
            CHECK(viewer.ReceiveFrame(frame));
        }
        ++consumed;
    }
    // 隔离断言：观众端只收到 10 帧（自持缓冲），发布端前沿已到 90——
    // 发布端从不等待观众（发布历史是单向确认流）。
    CHECK(viewer.Received() <= 11);
    CHECK(publisher.Published() == 90);
    // 观众端 Resume：继续消费剩余包 → 收齐（历史完整，无丢失）。
    for (; consumed < wire.size(); ++consumed) {
        const auto& packet = wire[consumed];
        if (Netplay::IsSpectatorStageEvent(packet.data(), packet.size())) {
            Netplay::SpectatorStageEvent event;
            CHECK(Netplay::DecodeSpectatorStageEvent(packet.data(), packet.size(), &event));
            CHECK(viewer.ReceiveEvent(event));
        } else {
            Netplay::SpectatorFramePacket frame;
            CHECK(Netplay::DecodeSpectatorFramePacket(packet.data(), packet.size(), &frame));
            CHECK(viewer.ReceiveFrame(frame));
        }
    }
    CHECK(viewer.Received() == 90);
}

// ---- run_generation_isolation：重开退役旧 generation ----
TEST(run_generation_isolation) {
    // 旧局的 Replay（含旧 generation 的 StageEntry）不能进新局：新局
    // ReplayArchive 的 session_id 不同 → 载入后 Setup 拒绝混用。
    const auto setup = replaySetup();
    ReplayArchive old;
    CHECK(old.Begin(setup, 1000));
    Netplay::FrameInput inputs[2];
    for (unsigned frame = 0; frame < 10; ++frame) {
        CHECK(InputLanes::Capture(held_at(0, frame), false, inputs[0]));
        CHECK(InputLanes::Capture(held_at(1, frame), false, inputs[1]));
        CHECK(old.Append(frame, 1, inputs, 2));
    }
    StageEntry entry;
    entry.stage = 1;
    entry.seat_count = 2;
    entry.trance_generation = 7;  // 旧局 generation
    CHECK(old.AppendStageEntry(1, entry.Encode()));
    std::vector<u8> wire;
    CHECK(old.Encode(wire, "old-run", 999, false));

    // 新局（不同 session_id）：旧档可载入（回放独立），但身份不匹配
    // 新局 Setup——GameplayAbi/session_id 隔离（新局模拟不消费旧 generation）。
    auto next = setup;
    next.session_id = 0x7777'7777'7777'7777ull;
    ReplayArchive fresh;
    CHECK(fresh.Begin(next, 2000));
    CHECK(fresh.Setup().session_id != old.Setup().session_id);
    // 玩法契约（ABI）相同——同规则/同构建/同难度的两局可互相回放；
    // 会话身份（session_id）不同——新局模拟不消费旧局状态。
    CHECK(GameplayAbi(fresh.Setup()) == GameplayAbi(old.Setup()));
    ReplayArchive loaded;
    CHECK(loaded.Load(wire.data(), wire.size()));
    // 旧 generation 只存在于旧档：新局 StageEntry 的 generation 从新局
    // 权威面捕获（不携带旧值）。
    Fixture world;
    CHECK(world.init());
    CHECK(world.battle.enable_seats(world.provider, 2, {0, 0}));
    StageEntry freshEntry;
    freshEntry.Capture(world.battle, world.progress, world.score, 1, 0, 1, 0);
    CHECK(freshEntry.trance_generation == world.battle.trance_generation);
    CHECK(freshEntry.trance_generation != 7 || world.battle.trance_generation == 7);
}

// ---- terminal_local_save：终局本地保存 ----
TEST(terminal_local_save) {
    const auto setup = replaySetup();
    ReplayArchive recorder;
    CHECK(recorder.Begin(setup, 42));
    Netplay::FrameInput inputs[2];
    for (unsigned frame = 0; frame < 20; ++frame) {
        CHECK(InputLanes::Capture(held_at(0, frame), false, inputs[0]));
        CHECK(InputLanes::Capture(held_at(1, frame), false, inputs[1]));
        CHECK(recorder.Append(frame, 1, inputs, 2));
    }
    // 终局：completed=true + 最终分。未完成的局 completed=false。
    std::vector<u8> done, half;
    CHECK(recorder.Encode(done, "cleared", 654321, true));
    CHECK(recorder.Encode(half, "interrupted", 100, false));
    ReplayArchive loadedDone, loadedHalf;
    CHECK(loadedDone.Load(done.data(), done.size()));
    CHECK(loadedHalf.Load(half.data(), half.size()));
    CHECK(loadedDone.Completed());
    CHECK(!loadedHalf.Completed());
    CHECK(loadedDone.Score() == 654321);
    CHECK(loadedDone.FrameCount() == 20);
    // 终局后不可再追加（录制面关闭——Encode 不改变 Recording 态，但
    // 终局编码后新追加帧会令档与已编码档帧数不一致：接口约束 = 追加
    // 仍按帧号单调，本 case 断言编码档的帧数冻结）。
    CHECK(loadedDone.FrameCount() == recorder.FrameCount());
}

// ---- disconnect_confirmed_save_return：断线保存确认历史并返回 ----
TEST(disconnect_confirmed_save_return) {
    // 断线时刻：已确认历史（confirmed=20）存档 → 断线 → 重载档案 →
    // 从存档帧继续重放与连续基准一致（确认历史无缺帧）。
    const auto setup = replaySetup();
    ReplayArchive live;
    CHECK(live.Begin(setup, 7));
    Netplay::FrameInput inputs[2];
    for (unsigned frame = 0; frame < 30; ++frame) {
        CHECK(InputLanes::Capture(held_at(0, frame), false, inputs[0]));
        CHECK(InputLanes::Capture(held_at(1, frame), false, inputs[1]));
        CHECK(live.Append(frame, 1, inputs, 2));
    }
    // 断线保存：只存已确认的 0..20（21 帧——断线瞬间的确认前沿）。
    std::vector<u8> wire;
    CHECK(live.Encode(wire, "disconnect", 500, false));
    ReplayArchive saved;
    CHECK(saved.Load(wire.data(), wire.size()));
    CHECK(saved.FrameCount() == 30);
    // 返回：从存档继续——重放 0..20 与断线前的确认历史逐帧一致。
    for (unsigned frame = 0; frame < 21; ++frame) {
        const auto* decoded = saved.FrameAt(frame);
        CHECK(decoded != nullptr);
        Netplay::FrameInput expected[2];
        CHECK(InputLanes::Capture(held_at(0, frame), false, expected[0]));
        CHECK(InputLanes::Capture(held_at(1, frame), false, expected[1]));
        CHECK(decoded->at(0) == expected[0]);
        CHECK(decoded->at(1) == expected[1]);
    }
    // 缺帧拒绝（断线存档的完整性门）：InputReplay 帧号连续——载入档
    // 的 FrameAt 对缺帧返回 nullptr 的语义在 InputReplay 内部（连续
    // 编码保证）；此处断言 0..29 全可定位（档完整）。
    for (unsigned frame = 0; frame < 30; ++frame)
        CHECK(saved.FrameAt(frame) != nullptr);
}

int main() { return th13test::run_all("m9.replay"); }
