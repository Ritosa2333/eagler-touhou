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

u32 held_at(unsigned seat, unsigned frame) {
    return seat == 0 ? (frame % 5 == 0 ? 0x010u : 0x001u) : (frame % 7 == 0 ? 0x801u : 0x001u);
}

}

TEST(replay_roundtrip_seek) {
    const auto setup = replaySetup();
    ReplayArchive recorder;
    CHECK(recorder.Begin(setup, 0x1122'3344'5566'7788ull));
    Netplay::FrameInput inputs[2];
    for (unsigned frame = 0; frame < 40; ++frame) {
        CHECK(InputLanes::Capture(held_at(0, frame), false, inputs[0]));
        CHECK(InputLanes::Capture(held_at(1, frame), false, inputs[1]));
        CHECK(recorder.Append(frame, 1, inputs, 2));
    }
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
    CHECK(!recorder.AppendStageEntry(1, entryBytes));
    std::vector<u8> wire;
    CHECK(recorder.Encode(wire, "roundtrip", 123456, true));
    CHECK(ReplayArchive::HasSignature(wire.data(), wire.size()));
    ReplayArchive player;
    CHECK(player.Load(wire.data(), wire.size()));
    CHECK(!player.UsesLegacyStagePositions());
    CHECK(!player.UsesLegacyPickupRewards());
    CHECK(!player.UsesLegacyBossMitigation());
    auto legacy = wire;
    legacy[4] = ReplayArchive::LegacyVersion;
    ReplayArchive old_player;
    CHECK(old_player.Load(legacy.data(), legacy.size()));
    CHECK(old_player.UsesLegacyStagePositions());
    CHECK(old_player.UsesLegacyPickupRewards());
    CHECK(old_player.UsesLegacyBossMitigation());
    legacy[4] = 3;
    CHECK(old_player.Load(legacy.data(), legacy.size()));
    CHECK(!old_player.UsesLegacyBossMitigation());
    CHECK(old_player.UsesLegacyPickupRewards());
    CHECK_EQ(old_player.FrameCount(), player.FrameCount());
    auto quarters = wire;
    quarters[4] = 2;
    ReplayArchive quarter_player;
    CHECK(quarter_player.Load(quarters.data(), quarters.size()));
    CHECK(!quarter_player.UsesLegacyStagePositions());
    CHECK(quarter_player.UsesLegacyBossMitigation());
    CHECK_EQ(quarter_player.FrameCount(), player.FrameCount());
    CHECK(player.Setup().session_id == setup.session_id);
    CHECK(player.Setup().seed == setup.seed);
    CHECK(player.Setup().selections == setup.selections);
    CHECK(player.Timestamp() == 0x1122'3344'5566'7788ull);
    CHECK(player.Score() == 123456);
    CHECK(player.Completed());
    CHECK(player.Name() == "roundtrip");
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
    const auto* chapters = player.Chapters();
    for (std::uint32_t index = 0; index < player.ChapterCount(); ++index) {
        const auto* anchor = player.FrameAt(chapters[index].firstFrame);
        CHECK(anchor != nullptr);
    }
    CHECK(!player.Load(wire.data(), wire.size() - 1));
    std::vector<u8> bad = wire;
    bad[0] ^= 0xff;
    CHECK(!player.Load(bad.data(), bad.size()));
    bad = wire;
    bad[4] = 9;
    CHECK(!player.Load(bad.data(), bad.size()));
}

TEST(stage_entry_carry) {
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
    StageEntry carry;
    carry.Capture(continuous.battle, continuous.progress, continuous.score,
                  47932, continuous.battle.sim.random.index(), 0x1234,
                  continuous.visual.index());
    step(continuous, 30);
    WorldState expected;
    CHECK(expected.capture(continuous.battle, continuous.visual));

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

    StageEntry rejected;
    CHECK(!StageEntry::Decode(bytes.data(), bytes.size() - 1, rejected));
    std::vector<u8> bad = bytes;
    bad[0] = 9;
    CHECK(!StageEntry::Decode(bad.data(), bad.size(), rejected));
    bad = bytes;
    bad[12] = 4;
    CHECK(!StageEntry::Decode(bad.data(), bad.size(), rejected));
}

TEST(spectator_confirmed_latejoin) {
    const auto setup = replaySetup();
    SpectatorJournal publisher;
    CHECK(publisher.BeginPublish(setup));
    Netplay::FrameInput inputs[2];
    StageEntry entry;
    entry.stage = 1;
    entry.seat_count = 2;
    const auto entryBytes = entry.Encode();
    for (unsigned frame = 0; frame < 30; ++frame) {
        CHECK(InputLanes::Capture(held_at(0, frame), false, inputs[0]));
        CHECK(InputLanes::Capture(held_at(1, frame), false, inputs[1]));
        const auto need = publisher.NeedsCheckpoint(frame, 1);
        CHECK(need == Netplay::SpectatorStagePublisher::CheckpointRequirement::Required ||
              need == Netplay::SpectatorStagePublisher::CheckpointRequirement::NotRequired);
        CHECK(publisher.RecordFrame(frame, 1, inputs, 2,
                                    frame == 0 ? &entryBytes : nullptr));
    }
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
    CHECK(publisher.Published() == 26);
    SpectatorJournal viewer;
    CHECK(viewer.BeginViewing(setup));
    for (const auto& packet : wire) {
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
    CHECK(viewer.Received() == 26);
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
    ReplayArchive archive;
    CHECK(viewer.ArchiveFromHistory(archive));
    CHECK(archive.FrameCount() == 26);
    CHECK(archive.Setup().session_id == setup.session_id);
}

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
    CHECK(publisher.Publish(89, 89, inputsAt, send));
    CHECK(publisher.Published() == 90);
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
    CHECK(viewer.Received() <= 11);
    CHECK(publisher.Published() == 90);
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

TEST(run_generation_isolation) {
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
    entry.trance_generation = 7;
    CHECK(old.AppendStageEntry(1, entry.Encode()));
    std::vector<u8> wire;
    CHECK(old.Encode(wire, "old-run", 999, false));

    auto next = setup;
    next.session_id = 0x7777'7777'7777'7777ull;
    ReplayArchive fresh;
    CHECK(fresh.Begin(next, 2000));
    CHECK(fresh.Setup().session_id != old.Setup().session_id);
    CHECK(GameplayAbi(fresh.Setup()) == GameplayAbi(old.Setup()));
    ReplayArchive loaded;
    CHECK(loaded.Load(wire.data(), wire.size()));
    Fixture world;
    CHECK(world.init());
    CHECK(world.battle.enable_seats(world.provider, 2, {0, 0}));
    StageEntry freshEntry;
    freshEntry.Capture(world.battle, world.progress, world.score, 1, 0, 1, 0);
    CHECK(freshEntry.trance_generation == world.battle.trance_generation);
    CHECK(freshEntry.trance_generation != 7 || world.battle.trance_generation == 7);
}

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
    CHECK(loadedDone.FrameCount() == recorder.FrameCount());
}

TEST(disconnect_confirmed_save_return) {
    const auto setup = replaySetup();
    ReplayArchive live;
    CHECK(live.Begin(setup, 7));
    Netplay::FrameInput inputs[2];
    for (unsigned frame = 0; frame < 30; ++frame) {
        CHECK(InputLanes::Capture(held_at(0, frame), false, inputs[0]));
        CHECK(InputLanes::Capture(held_at(1, frame), false, inputs[1]));
        CHECK(live.Append(frame, 1, inputs, 2));
    }
    std::vector<u8> wire;
    CHECK(live.Encode(wire, "disconnect", 500, false));
    ReplayArchive saved;
    CHECK(saved.Load(wire.data(), wire.size()));
    CHECK(saved.FrameCount() == 30);
    for (unsigned frame = 0; frame < 21; ++frame) {
        const auto* decoded = saved.FrameAt(frame);
        CHECK(decoded != nullptr);
        Netplay::FrameInput expected[2];
        CHECK(InputLanes::Capture(held_at(0, frame), false, expected[0]));
        CHECK(InputLanes::Capture(held_at(1, frame), false, expected[1]));
        CHECK(decoded->at(0) == expected[0]);
        CHECK(decoded->at(1) == expected[1]);
    }
    for (unsigned frame = 0; frame < 30; ++frame)
        CHECK(saved.FrameAt(frame) != nullptr);
}

int main() { return th13test::run_all("m9.replay"); }
