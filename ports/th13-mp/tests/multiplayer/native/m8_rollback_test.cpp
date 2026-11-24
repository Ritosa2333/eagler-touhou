#include "../../ecl/ecl_test_fixtures.hpp"
#include "game/StageBattle.hpp"
#include "multiplayer/Session.hpp"
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <set>
#include <string>
#include <vector>
#include "multiplayer/WorldState.hpp"
#include "sdl/AudioDevice.hpp"

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
        if (name == "mp6.ecl") {
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
        if (!battle.load(provider, "mp6.ecl", 2, host, progress, player, score)) {
            std::printf("  battle.load failed: %s\n", battle.error().c_str());
            return false;
        }
        return true;
    }
};

uint64_t advance(Fixture& fixture, int frames, u32 held_a, u32 held_b) {
    for (int frame = 0; frame < frames; ++frame) {
        fixture.battle.seat_inputs[0].update(held_a);
        fixture.battle.seat_inputs[1].update(held_b);
        CHECK(fixture.battle.step(GameInput{}));
    }
    WorldState snapshot;
    CHECK(snapshot.capture(fixture.battle, fixture.visual));
    return snapshot.digest_core();
}

}

TEST(rollback_one_three_twelve) {
    for (const int error_frames : {1, 3, 12}) {
        Fixture baseline;
        CHECK(baseline.init());
        CHECK(baseline.battle.enable_seats(baseline.provider, 2, {0, 0}));
        const uint64_t expected = advance(baseline, 40, 0x01, 0x01);

        Fixture rollback;
        CHECK(rollback.init());
        CHECK(rollback.battle.enable_seats(rollback.provider, 2, {0, 0}));
        advance(rollback, 20, 0x01, 0x01);
        WorldState checkpoint;
        CHECK(checkpoint.capture(rollback.battle, rollback.visual));
        advance(rollback, error_frames, 0x3f, 0x3f);
        CHECK(checkpoint.restore(rollback.battle, rollback.visual));
        advance(rollback, 20, 0x01, 0x01);
        WorldState verify;
        CHECK(verify.capture(rollback.battle, rollback.visual));
        CHECK_EQ(verify.digest_core(), expected);
    }
}

namespace {

using Observation = th13::sdl::AudioDevice::RequestObservation;

SessionSetup p2_setup();
bool p2_begin(Fixture& world, const SessionSetup& setup);

const char* audio_asset_root() {
    const char* root = std::getenv("TH13_NATIVE_ASSET_ROOT");
    return root && *root ? root : "";
}

const char* audio_repo_root() {
    const char* root = std::getenv("TH13_NATIVE_REPO_ROOT");
    return root && *root ? root : "../../..";
}

void script_audio(th13::sdl::AudioDevice& device, std::uint32_t frame) {
    device.enqueue(8, float(frame % 3) * 60.f - 60.f);
    device.enqueue(9);
    if (frame % 4 == 0) device.stop(9);
    device.observe_voice_pan(8, float(frame % 5) * 48.f - 96.f);
    if (frame == 0) device.queue_music(1, 0, "th13_01.wav");
    if (frame == 3) device.queue_music(5, 1, "");
    if (frame == 6) device.queue_music(1, 1, "th13_03.wav");
}

struct AudioArm {
    th13::sdl::AudioDevice device;
    struct Applied {
        std::uint32_t seq = 0;
        std::uint32_t frame = 0;
        Observation observation;
    };
    std::vector<Applied> applied;
    std::uint32_t seq = 0;
    std::uint32_t committed_seen = 0;
    std::vector<std::uint32_t> commit_seq;

    bool init(std::uint32_t frames) {
        if (!*audio_asset_root()) {
            std::printf("  失败 audio: TH13_NATIVE_ASSET_ROOT 未注入\n");
            return false;
        }
        if (!device.initialize_for_test(
                audio_asset_root(),
                std::string(audio_repo_root()) + "/games/th13/thbgmogg.dat")) {
            std::printf("  失败 audio initialize_for_test: %s\n", device.error.c_str());
            return false;
        }
        device.defer_output = true;
        commit_seq.assign(frames, 0xFFFFFFFFu);
        device.request_observer = [this](const Observation& observation) {
            auto& session = multiplayer::session;
            if (session.RecordingAudio()) session.ObserveAudio(observation);
            else device.apply_request(observation);
        };
        return true;
    }
    bool drain() {
        auto& session = multiplayer::session;
        const auto apply = [this, &session](const std::vector<Session::AudioRequest>& list) {
            std::size_t count = 0;
            for (; count < list.size(); ++count) {
                const auto& request = list[count];
                if (!device.apply_request(request.observation)) break;
                applied.push_back({seq++, request.frame, request.observation});
                session.NoteApplied(request.frame, request.observation);
            }
            return count;
        };
        std::size_t count = apply(session.AudioOut());
        session.ConsumeAudioOut(count);
        if (!device.error.empty()) return false;
        count = apply(session.VoiceOut());
        session.ConsumeVoiceOut(count);
        return device.error.empty();
    }
    void note_commits() {
        auto& session = multiplayer::session;
        while (committed_seen < session.Committed() && committed_seen < commit_seq.size()) {
            commit_seq[committed_seen] = seq++;
            ++committed_seen;
        }
    }
};

struct AudioArmResult {
    std::vector<AudioArm::Applied> applied;
    std::vector<std::uint32_t> commit_seq;
    std::uint32_t restores = 0, max_distance = 0;
    std::uint64_t suppressed = 0, music_deferred = 0, music_dropped = 0;
    std::uint64_t voice_captured = 0, music_captured = 0;
    std::uint64_t voice_applied = 0, music_applied = 0;
};

bool drive_audio_arm(Fixture& world, AudioArm& arm, bool rollback,
                     std::uint32_t total, std::uint32_t held, AudioArmResult& out) {
    Session& session = multiplayer::session;
    if (!p2_begin(world, p2_setup())) return false;
    const auto submit = [&](std::uint32_t frame) {
        Netplay::FrameInput input;
        if (!InputLanes::Capture(held, false, input)) return false;
        const auto result = session.SubmitRemote(1, frame, input);
        return result == Netplay::RemoteInputResult::Accepted ||
               result == Netplay::RemoteInputResult::Duplicate ||
               result == Netplay::RemoteInputResult::RollbackRequired ||
               result == Netplay::RemoteInputResult::PredictionCorrect;
    };
    std::uint64_t now = 9000;
    bool released = !rollback;
    for (unsigned guard = 0; guard < 512; ++guard) {
        if (!session.Pump(now++)) return false;
        if (rollback && !released && session.SimFrame() >= 12) {
            for (std::uint32_t frame = 0; frame < total; ++frame)
                if (!submit(frame)) return false;
            released = true;
            continue;
        }
        if (!rollback && session.SimFrame() < total &&
            !session.Correcting()) {
            if (!submit(session.SimFrame())) return false;
        }
        if (session.SimFrame() >= total && !session.Correcting()) break;
        if (!session.CanSimulate()) continue;
        if (!session.Correcting() &&
            session.CaptureFrame() == session.SimFrame()) {
            if (!session.CaptureLocal(held, now)) return false;
            session.AdvanceCapturePhase();
        }
        const std::uint32_t frame = session.SimFrame();
        const GameInput* local = session.LocalDelayedInput();
        if (!local) return false;
        if (!world.battle.step(*local)) return false;
        script_audio(arm.device, frame);
        if (!arm.drain()) {
            std::printf("  失败 audio drain @frame %u: %s\n", frame,
                        arm.device.error.c_str());
            return false;
        }
        session.OnFrameSimulated();
        arm.note_commits();
        if (session.SimFrame() == frame) return false;
    }
    if (!arm.drain()) return false;
    out.applied = arm.applied;
    out.commit_seq = arm.commit_seq;
    out.restores = session.Restores();
    out.max_distance = session.MaxRollbackDistance();
    out.suppressed = session.SfxSuppressed();
    out.music_deferred = session.MusicDeferred();
    out.music_dropped = session.MusicDropped();
    out.voice_captured = session.VoiceCaptured();
    out.music_captured = session.MusicCaptured();
    out.voice_applied = session.VoiceApplied();
    out.music_applied = session.MusicApplied();
    session.Stop();
    return true;
}

std::map<std::uint32_t, std::multiset<std::string>> grouped_applies(
    const AudioArmResult& arm) {
    std::map<std::uint32_t, std::multiset<std::string>> grouped;
    for (const auto& item : arm.applied) {
        char buffer[512];
        std::snprintf(buffer, sizeof buffer, "k%d|id%d|v%d|p%d|x%.9g|%s",
                      int(item.observation.kind), item.observation.id,
                      item.observation.value, item.observation.positional ? 1 : 0,
                      double(item.observation.x), item.observation.text.c_str());
        grouped[item.frame].insert(buffer);
    }
    return grouped;
}

}

TEST(audio_bgm_once) {
    Fixture fixture;
    CHECK(fixture.init());
    CHECK(fixture.battle.enable_seats(fixture.provider, 2, {0, 0}));
    Session& session = multiplayer::session;
    session.Stop();
    th13::sdl::AudioDevice::RequestObservation event;
    event.kind = th13::sdl::AudioDevice::RequestObservation::Kind::Voice;
    event.id = 42;
    session.ObserveAudio(event);
    CHECK(session.AudioOut().empty());
    session.Stop();

    const std::uint32_t total = 13;
    const std::uint32_t held = 0x001;
    Fixture rolled_world;
    CHECK(rolled_world.init());
    CHECK(rolled_world.battle.enable_seats(rolled_world.provider, 2, {0, 0}));
    AudioArm rolled_arm;
    CHECK(rolled_arm.init(total));
    AudioArmResult rolled;
    CHECK(drive_audio_arm(rolled_world, rolled_arm, true, total, held, rolled));

    Fixture continuous_world;
    CHECK(continuous_world.init());
    CHECK(continuous_world.battle.enable_seats(continuous_world.provider, 2, {0, 0}));
    AudioArm continuous_arm;
    CHECK(continuous_arm.init(total));
    AudioArmResult continuous;
    CHECK(drive_audio_arm(continuous_world, continuous_arm, false, total, held,
                          continuous));

    CHECK_EQ(continuous.restores, 0u);
    CHECK(rolled.restores >= 1u);
    CHECK(rolled.max_distance >= 12u);
    CHECK(rolled.suppressed > 0);
    CHECK(rolled.music_deferred > 0);
    std::printf("# audio_bgm_once: restores=%u maxDistance=%u suppressed=%llu "
                "music_deferred=%llu music_dropped=%llu voice=%llu/%llu/%llu "
                "music=%llu/%llu/%llu\n",
                rolled.restores, rolled.max_distance,
                (unsigned long long)rolled.suppressed,
                (unsigned long long)rolled.music_deferred,
                (unsigned long long)rolled.music_dropped,
                (unsigned long long)rolled.voice_captured,
                (unsigned long long)rolled.voice_applied,
                (unsigned long long)rolled.suppressed,
                (unsigned long long)rolled.music_captured,
                (unsigned long long)rolled.music_applied,
                (unsigned long long)rolled.music_dropped);

    CHECK_EQ(rolled.voice_applied + rolled.suppressed, rolled.voice_captured);
    CHECK_EQ(rolled.music_applied + rolled.music_dropped, rolled.music_captured);
    CHECK_EQ(continuous.suppressed, 0ull);
    CHECK_EQ(continuous.music_dropped, 0ull);
    CHECK_EQ(continuous.voice_applied, continuous.voice_captured);
    CHECK_EQ(continuous.music_applied, continuous.music_captured);

    const auto rolled_groups = grouped_applies(rolled);
    const auto continuous_groups = grouped_applies(continuous);
    CHECK_EQ(rolled_groups.size(), continuous_groups.size());
    for (const auto& [frame, events] : continuous_groups) {
        const auto found = rolled_groups.find(frame);
        CHECK(found != rolled_groups.end());
        if (found == rolled_groups.end()) continue;
        CHECK_EQ(found->second.size(), events.size());
        CHECK(found->second == events);
        if (found->second != events) {
            for (const auto& e : events)
                std::printf("  continuous f%u: %s\n", frame, e.c_str());
            for (const auto& e : found->second)
                std::printf("  rolled     f%u: %s\n", frame, e.c_str());
        }
    }

    std::map<std::string, unsigned> music_counts;
    std::size_t voice_checks = 0, music_checks = 0;
    for (const auto& item : rolled.applied) {
        CHECK(item.frame < rolled.commit_seq.size());
        if (item.frame >= rolled.commit_seq.size()) continue;
        const std::uint32_t commit = rolled.commit_seq[item.frame];
        CHECK(commit != 0xFFFFFFFFu);
        if (item.observation.kind == Observation::Kind::Voice) {
            CHECK(item.seq < commit);
            ++voice_checks;
        } else if (item.observation.kind == Observation::Kind::Music) {
            CHECK(item.seq > commit);
            char key[512];
            std::snprintf(key, sizeof key, "f%u|%d|%d|%s", item.frame,
                          item.observation.id, item.observation.value,
                          item.observation.text.c_str());
            ++music_counts[key];
            ++music_checks;
        }
    }
    CHECK(voice_checks > 0);
    CHECK(music_checks > 0);
    for (const auto& [key, count] : music_counts) {
        CHECK_EQ(count, 1u);
        if (count != 1u) std::printf("  music duplicate %s x%u\n", key.c_str(), count);
    }
}

TEST(trance_death_resimulation) {
    Fixture baseline;
    CHECK(baseline.init());
    CHECK(baseline.battle.enable_seats(baseline.provider, 2, {0, 0}));
    const uint64_t expected = advance(baseline, 60, 0x01, 0x801);

    Fixture rollback;
    CHECK(rollback.init());
    CHECK(rollback.battle.enable_seats(rollback.provider, 2, {0, 0}));
    advance(rollback, 30, 0x01, 0x801);
    WorldState checkpoint;
    CHECK(checkpoint.capture(rollback.battle, rollback.visual));
    advance(rollback, 12, 0x3f, 0xff);
    CHECK(checkpoint.restore(rollback.battle, rollback.visual));
    advance(rollback, 30, 0x01, 0x801);
    WorldState verify;
    CHECK(verify.capture(rollback.battle, rollback.visual));
    CHECK_EQ(verify.digest_core(), expected);
}

TEST(resim_budget_new_progress) {
    Fixture baseline;
    CHECK(baseline.init());
    CHECK(baseline.battle.enable_seats(baseline.provider, 2, {0, 0}));
    const uint64_t expected = advance(baseline, 53, 0x01, 0x01);

    Fixture rollback;
    CHECK(rollback.init());
    CHECK(rollback.battle.enable_seats(rollback.provider, 2, {0, 0}));
    advance(rollback, 40, 0x01, 0x01);
    WorldState checkpoint;
    CHECK(checkpoint.capture(rollback.battle, rollback.visual));
    advance(rollback, 12, 0x3f, 0x3f);
    CHECK(checkpoint.restore(rollback.battle, rollback.visual));
    advance(rollback, 13, 0x01, 0x01);
    WorldState verify;
    CHECK(verify.capture(rollback.battle, rollback.visual));
    CHECK_EQ(verify.digest_core(), expected);
    advance(baseline, 1, 0x01, 0x01);
    advance(rollback, 1, 0x01, 0x01);
    WorldState verify2, baseline2;
    CHECK(verify2.capture(rollback.battle, rollback.visual));
    CHECK(baseline2.capture(baseline.battle, baseline.visual));
    CHECK_EQ(verify2.digest_core(), baseline2.digest_core());
}

TEST(delay_zero_two_nine) {
    for (const unsigned delay : {0u, 2u, 9u}) {
        Netplay::RollbackCore core;
        Netplay::CoreConfig config;
        config.sessionId = 0x1234;
        config.playerCount = 2;
        config.localPlayer = 0;
        config.inputDelay = uint8_t(delay);
        config.maxRollbackFrames = 12;
        config.allowPrediction = false;
        CHECK(core.Reset(config));
        for (unsigned capture = 0; capture < 20; ++capture) {
            const unsigned bits = capture % 2 ? 0x001 : 0x000;
            CHECK(core.ScheduleLocalInput(capture, Netplay::FrameInput(bits)));
        }
        for (unsigned frame = 0; frame < 25; ++frame) {
            const unsigned bits = frame % 2 ? 0x001 : 0x000;
            const auto result = core.SubmitRemoteInput(1, frame, Netplay::FrameInput(bits));
            CHECK(result == Netplay::RemoteInputResult::Accepted ||
                  result == Netplay::RemoteInputResult::Duplicate);
        }
        for (unsigned frame = delay; frame < delay + 20; ++frame) {
            std::array<Netplay::FrameInput, 3> inputs{};
            if (!core.ConfirmedInputs(frame, &inputs)) continue;
            const unsigned bits = (frame - delay) % 2 ? 0x001 : 0x000;
            CHECK(inputs[0].buttons == uint16_t(bits));
        }
        CHECK(core.ConfirmedThroughAllRemotes() >= 19);
    }
}

TEST(jitter_loss_reorder_duplicate) {
    Netplay::RollbackCore core;
    Netplay::CoreConfig config;
    config.sessionId = 0x5678;
    config.playerCount = 2;
    config.localPlayer = 0;
    config.inputDelay = 0;
    config.maxRollbackFrames = 12;
    config.allowPrediction = true;
    config.predictableButtons = 0x0f8 | 0x001;
    config.directionButtons = 0x0f0;
    CHECK(core.Reset(config));
    for (unsigned capture = 0; capture < 10; ++capture)
        CHECK(core.ScheduleLocalInput(capture, Netplay::FrameInput(0x001)));
    const unsigned order[] = {5, 3, 4, 2, 7, 6, 8, 9, 0, 1};
    for (const unsigned frame : order) {
        const auto result = core.SubmitRemoteInput(1, frame, Netplay::FrameInput(0x002));
        CHECK(result == Netplay::RemoteInputResult::Accepted ||
              result == Netplay::RemoteInputResult::Duplicate ||
              result == Netplay::RemoteInputResult::RollbackRequired ||
              result == Netplay::RemoteInputResult::PredictionCorrect);
    }
    CHECK(core.SubmitRemoteInput(1, 5, Netplay::FrameInput(0x002)) ==
          Netplay::RemoteInputResult::Duplicate);
    CHECK(core.SubmitRemoteInput(1, 5, Netplay::FrameInput(0x003)) ==
          Netplay::RemoteInputResult::ConflictingConfirmedInput);
    CHECK(core.ConfirmedThroughAllRemotes() == 9);
    CHECK(core.SubmitRemoteInput(1, 10, Netplay::FrameInput(0x002)) ==
          Netplay::RemoteInputResult::Accepted);
    CHECK(core.ConfirmedThroughAllRemotes() == 10);
}

TEST(window_stall_conflict) {
    Netplay::RollbackCore core;
    Netplay::CoreConfig config;
    config.sessionId = 0x9abc;
    config.playerCount = 2;
    config.localPlayer = 0;
    config.inputDelay = 0;
    config.maxRollbackFrames = 12;
    config.allowPrediction = true;
    config.predictableButtons = 0x0f8;
    config.directionButtons = 0x0f0;
    CHECK(core.Reset(config));
    for (unsigned capture = 0; capture < 30; ++capture)
        CHECK(core.ScheduleLocalInput(capture, Netplay::FrameInput(0x001)));
    for (unsigned frame = 0; frame <= 5; ++frame)
        CHECK(core.SubmitRemoteInput(1, frame, Netplay::FrameInput(0)) ==
              Netplay::RemoteInputResult::Accepted);
    for (unsigned frame = 0; frame < 18; ++frame) {
        const auto decision = core.PrepareFrame(frame);
        CHECK(decision.canAdvance);
        CHECK(core.MarkSimulated(frame, decision));
    }
    const auto stalled = core.PrepareFrame(18);
    CHECK(!stalled.canAdvance);
    for (unsigned frame = 6; frame < 18; ++frame)
        CHECK(core.SubmitRemoteInput(1, frame, Netplay::FrameInput(0x002)) !=
              Netplay::RemoteInputResult::InvalidPlayer);
    const auto resumed = core.PrepareFrame(18);
    CHECK(resumed.canAdvance);
}

namespace {
struct SessionDriveResult {
    uint64_t digest = 0;
    std::uint32_t sim_frames = 0, restores = 0;
    std::string domains;
    uint64_t capture_us_sum = 0, capture_us_max = 0, capture_us_count = 0;
    uint64_t restore_us_sum = 0, restore_us_max = 0;
};

SessionDriveResult drive_session_world(Fixture& world, const SessionSetup& setup,
                                       unsigned total_frames, unsigned remote_lag,
                                       std::vector<uint64_t>* trace = nullptr) {
    Session& session = multiplayer::session;
    session.Stop();
    const auto words = EncodeSessionSetup(setup);
    CHECK(session.Configure(words.data(), words.size()));
    session.BindWorld(&world.battle, &world.visual, nullptr);
    world.battle.mp_local_seat = int(setup.local_player);
    CHECK(session.BeginLocalSimulation());
    const auto held_at = [](unsigned seat, unsigned frame) -> u32 {
        return seat == 0 ? (frame % 5 == 0 ? 0x010u : 0x001u) : 0x801u;
    };
    const unsigned remote_seat = setup.local_player == 0 ? 1u : 0u;
    const auto remote_held_at = [&](unsigned frame) -> u32 {
        return frame >= setup.input_delay
                   ? held_at(remote_seat, frame - setup.input_delay)
                   : 0u;
    };
    for (unsigned frame = 0; frame < total_frames; ++frame) {
        CHECK(session.CaptureLocal(held_at(setup.local_player, frame), frame));
        session.AdvanceCapturePhase();
        if (frame >= remote_lag) {
            const unsigned remote_frame = frame - remote_lag;
            Netplay::FrameInput remote_input;
            CHECK(InputLanes::Capture(remote_held_at(remote_frame), false, remote_input));
            const auto result = session.SubmitRemote(remote_seat, remote_frame, remote_input);
            if (result == Netplay::RemoteInputResult::RollbackRequired)
                std::printf("  rb-req @frame %u remote_frame %u sim=%u next=%u\n",
                            frame, remote_frame, session.SimFrame(), 0);
            CHECK(result == Netplay::RemoteInputResult::Accepted ||
                  result == Netplay::RemoteInputResult::Duplicate ||
                  result == Netplay::RemoteInputResult::RollbackRequired ||
                  result == Netplay::RemoteInputResult::PredictionCorrect);
        }
        CHECK(session.Pump(frame));
        if (session.SimFrame() < total_frames && session.CanSimulate()) {
            const GameInput* local = session.LocalDelayedInput();
            CHECK(local != nullptr);
            CHECK(world.battle.step(*local));
            session.OnFrameSimulated();
            if (trace && session.SimFrame() > 0 && session.SimFrame() <= 3) {
                std::printf("  step: outer=%u sim_after=%u battle_frame=%d\n",
                            frame, session.SimFrame(), world.battle.sim.frame);
            }
            if (trace && session.SimFrame() > 0) {
                WorldState per_frame;
                CHECK(per_frame.capture(world.battle, world.visual));
                const size_t at = size_t(session.SimFrame()) - 1;
                if (trace->size() <= at) trace->resize(at + 1);
                (*trace)[at] = per_frame.digest_core();
                if (at < 2) {
                    char buffer[512];
                    per_frame.digest_domains(buffer, sizeof buffer);
                    std::printf("  world=%s frame=%zu domains=%s\n",
                                 remote_lag ? "rolled" : "continuous", at, buffer);
                }
            }
        }
    }
    for (unsigned frame = total_frames - remote_lag; frame < total_frames; ++frame) {
        Netplay::FrameInput remote_input;
        CHECK(InputLanes::Capture(remote_held_at(frame), false, remote_input));
        const auto result = session.SubmitRemote(remote_seat, frame, remote_input);
        CHECK(result == Netplay::RemoteInputResult::Accepted ||
              result == Netplay::RemoteInputResult::Duplicate ||
              result == Netplay::RemoteInputResult::RollbackRequired ||
              result == Netplay::RemoteInputResult::PredictionCorrect);
    }
    int guard = 0;
    while (guard++ < 256) {
        CHECK(session.Pump(total_frames));
        if (!(session.SimFrame() < total_frames && session.CanSimulate())) break;
        const GameInput* local = session.LocalDelayedInput();
        CHECK(local != nullptr);
        CHECK(world.battle.step(*local));
        session.OnFrameSimulated();
        if (trace && session.SimFrame() > 0) {
            WorldState per_frame;
            CHECK(per_frame.capture(world.battle, world.visual));
            const size_t at = size_t(session.SimFrame()) - 1;
            if (trace->size() <= at) trace->resize(at + 1);
            (*trace)[at] = per_frame.digest_core();
        }
    }
    SessionDriveResult out;
    out.sim_frames = session.SimFrame();
    out.restores = session.Restores();
    out.capture_us_sum = session.CaptureUsSum();
    out.capture_us_count = session.CaptureUsCount();
    out.capture_us_max = session.CaptureUsMax();
    out.restore_us_sum = session.RestoreUsSum();
    out.restore_us_max = session.RestoreUsMax();
    WorldState snapshot;
    CHECK(snapshot.capture(world.battle, world.visual));
    out.digest = snapshot.digest_core();
    char buffer[512];
    snapshot.digest_domains(buffer, sizeof buffer);
    out.domains = buffer;
    session.Stop();
    return out;
}

struct TickOutcome {
    bool ok = true;
    unsigned captures = 0;
    unsigned resimulated = 0;
    unsigned progressed = 0;
    std::uint32_t local_held = 0;
};

SessionSetup p2_setup() {
    SessionSetup setup;
    setup.version = SessionSetup::Version;
    setup.mode = 2;
    setup.session_id = 0x9135'0000'0000'0002ull;
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

bool p2_begin(Fixture& world, const SessionSetup& setup) {
    Session& session = multiplayer::session;
    session.Stop();
    const auto words = EncodeSessionSetup(setup);
    if (!session.Configure(words.data(), words.size())) return false;
    session.BindWorld(&world.battle, &world.visual, nullptr);
    world.battle.mp_local_seat = int(setup.local_player);
    return session.BeginLocalSimulation();
}

TickOutcome p2_tick(Fixture& world, std::uint32_t held, std::uint64_t now_ms,
                    double& accumulator, bool frozen = false) {
    TickOutcome outcome;
    Session& session = multiplayer::session;
    if (!session.Pump(now_ms)) { outcome.ok = false; return outcome; }
    if (!session.Simulating()) return outcome;
    accumulator += (1.0 / 60.0) / session.IntervalScale();
    while (session.Correcting()) {
        if (!session.CanSimulate()) break;
        const auto before = session.SimFrame();
        if (!frozen) {
            const GameInput* local = session.LocalDelayedInput();
            if (!local) { outcome.ok = false; return outcome; }
            outcome.local_held = local->held;
            if (!world.battle.step(*local)) { outcome.ok = false; return outcome; }
        }
        session.OnFrameSimulated();
        if (session.SimFrame() == before) { outcome.ok = false; return outcome; }
        ++outcome.resimulated;
    }
    unsigned steps = 0;
    while (accumulator >= (1.0 / 60.0) - 1e-9 && steps < 2 && !session.Correcting()) {
        if (session.CaptureFrame() == session.SimFrame()) {
            if (!session.CaptureLocal(held, now_ms)) { outcome.ok = false; return outcome; }
            session.AdvanceCapturePhase();
            ++outcome.captures;
        }
        if (!session.CanSimulate()) {
            accumulator = std::min(accumulator, 1.0 / 60.0);
            break;
        }
        const auto before = session.SimFrame();
        if (!frozen) {
            const GameInput* local = session.LocalDelayedInput();
            if (!local) { outcome.ok = false; return outcome; }
            outcome.local_held = local->held;
            if (!world.battle.step(*local)) { outcome.ok = false; return outcome; }
        }
        session.OnFrameSimulated();
        if (session.SimFrame() == before) { outcome.ok = false; return outcome; }
        accumulator -= 1.0 / 60.0;
        ++steps;
        ++outcome.progressed;
    }
    return outcome;
}

}

TEST(session_driven_rollback_equivalence) {
    SessionSetup setup;
    setup.version = SessionSetup::Version;
    setup.mode = 2;
    setup.session_id = 0xbeef'cafe'1234'5678ull;
    setup.player_count = 2;
    setup.local_player = 0;
    setup.difficulty = 2;
    setup.seed = 47932;
    setup.automatic = false;
    setup.requested_delay = setup.input_delay = 2;
    setup.selections = {0, 0, 0};
    setup.build = {1, 2, 3, 4};
    const unsigned total_frames = 40;

    Fixture baseline;
    CHECK(baseline.init());
    CHECK(baseline.battle.enable_seats(baseline.provider, 2, {0, 0}));
    std::vector<uint64_t> trace_continuous;
    const auto continuous = drive_session_world(baseline, setup, total_frames, 0,
                                                &trace_continuous);
    CHECK(continuous.sim_frames == total_frames);
    CHECK(continuous.restores == 0);

    Fixture rolled;
    CHECK(rolled.init());
    CHECK(rolled.battle.enable_seats(rolled.provider, 2, {0, 0}));
    auto rolled_setup = setup;
    rolled_setup.local_player = 1;
    std::vector<uint64_t> trace_corrected;
    const auto corrected = drive_session_world(rolled, rolled_setup, total_frames, 6,
                                                &trace_corrected);
    CHECK(corrected.sim_frames == total_frames);
    CHECK(corrected.restores > 0);

    if (corrected.digest != continuous.digest) {
        Fixture baseline_world, rolled_world;
        std::printf("  corrected=%llu continuous=%llu\n  C: %s\n  N: %s\n",
                    (unsigned long long)corrected.digest,
                    (unsigned long long)continuous.digest,
                    corrected.domains.c_str(), continuous.domains.c_str());
        for (size_t i = 0; i < trace_continuous.size() && i < trace_corrected.size(); ++i) {
            if (trace_continuous[i] != trace_corrected[i]) {
                std::printf("  首差 trace[%zu]: continuous=%llu corrected=%llu\n",
                            i, (unsigned long long)trace_continuous[i],
                            (unsigned long long)trace_corrected[i]);
                break;
            }
        }
        CHECK(false);
    }
}

TEST(same_tick_resim) {
    const SessionSetup setup = p2_setup();
    const std::uint32_t held = 0x001;
    Netplay::FrameInput remote;
    CHECK(InputLanes::Capture(held, false, remote));

    Fixture rolled;
    CHECK(rolled.init());
    CHECK(rolled.battle.enable_seats(rolled.provider, 2, {0, 0}));
    CHECK(p2_begin(rolled, setup));
    double acc = 0;
    for (unsigned frame = 0; frame < 12; ++frame) {
        const auto tick = p2_tick(rolled, held, 1000 + frame, acc);
        CHECK(tick.ok);
        CHECK_EQ(tick.captures, 1u);
        CHECK_EQ(tick.resimulated, 0u);
        CHECK_EQ(tick.progressed, 1u);
        CHECK_EQ(multiplayer::session.SimFrame(), frame + 1);
    }
    CHECK_EQ(multiplayer::session.SimFrame(), 12u);
    CHECK_EQ(multiplayer::session.CaptureFrame(), 12u);
    CHECK_EQ(multiplayer::session.Restores(), 0u);
    CHECK_EQ(multiplayer::session.Checkpoints(), 12u);

    for (unsigned frame = 0; frame < 12; ++frame) {
        const auto result = multiplayer::session.SubmitRemote(1, frame, remote);
        CHECK(result == Netplay::RemoteInputResult::Accepted ||
              result == Netplay::RemoteInputResult::Duplicate ||
              result == Netplay::RemoteInputResult::RollbackRequired ||
              result == Netplay::RemoteInputResult::PredictionCorrect);
    }
    const std::uint32_t before_frame = multiplayer::session.SimFrame();
    const auto tick = p2_tick(rolled, held, 2000, acc);
    CHECK(tick.ok);
    CHECK_EQ(tick.resimulated, 12u);
    CHECK_EQ(tick.progressed, 1u);
    CHECK_EQ(tick.captures, 1u);
    CHECK_EQ(multiplayer::session.SimFrame(), before_frame + 1);
    CHECK_EQ(multiplayer::session.Restores(), 1u);
    CHECK_EQ(multiplayer::session.Resimulated(), 12u);
    CHECK_EQ(multiplayer::session.CaptureFrame(), before_frame + 1);
    WorldState rolled_state;
    CHECK(rolled_state.capture(rolled.battle, rolled.visual));
    const std::uint64_t rolled_digest = rolled_state.digest_core();
    const std::uint32_t rolled_restores = multiplayer::session.Restores();
    const std::uint32_t rolled_resimulated = multiplayer::session.Resimulated();
    const std::uint32_t rolled_checkpoints = multiplayer::session.Checkpoints();

    Fixture baseline;
    CHECK(baseline.init());
    CHECK(baseline.battle.enable_seats(baseline.provider, 2, {0, 0}));
    CHECK(p2_begin(baseline, setup));
    double baseline_acc = 0;
    for (unsigned frame = 0; frame < 13; ++frame) {
        const auto result = multiplayer::session.SubmitRemote(1, frame, remote);
        CHECK(result == Netplay::RemoteInputResult::Accepted ||
              result == Netplay::RemoteInputResult::Duplicate ||
              result == Netplay::RemoteInputResult::RollbackRequired ||
              result == Netplay::RemoteInputResult::PredictionCorrect);
        const auto base_tick = p2_tick(baseline, held, 3000 + frame, baseline_acc);
        CHECK(base_tick.ok);
        CHECK_EQ(base_tick.captures, 1u);
        CHECK_EQ(base_tick.resimulated, 0u);
        CHECK_EQ(base_tick.progressed, 1u);
        CHECK_EQ(multiplayer::session.SimFrame(), frame + 1);
    }
    CHECK_EQ(multiplayer::session.Restores(), 0u);
    CHECK_EQ(multiplayer::session.Resimulated(), 0u);
    WorldState baseline_state;
    CHECK(baseline_state.capture(baseline.battle, baseline.visual));
    CHECK_EQ(rolled_digest, baseline_state.digest_core());
    std::printf("# same_tick_resim: pre_rollback_sim=%u post_tick_sim=%u restores=%u "
                "resimulated=%u checkpoints=%u digest=%llx\n",
                before_frame, before_frame + 1,
                rolled_restores, rolled_resimulated, rolled_checkpoints,
                (unsigned long long)rolled_digest);
    multiplayer::session.Stop();
}

TEST(capture_phase_lockstep) {
    const SessionSetup setup = p2_setup();
    const unsigned delay = setup.input_delay;
    const auto held_for = [](unsigned frame) -> std::uint32_t {
        return frame % 5 == 0 ? 0x010u : 0x001u;
    };
    Fixture world;
    CHECK(world.init());
    CHECK(world.battle.enable_seats(world.provider, 2, {0, 0}));
    CHECK(p2_begin(world, setup));
    double acc = 0;
    const auto submit_remote = [](unsigned frame, std::uint32_t bits) {
        Netplay::FrameInput input;
        CHECK(InputLanes::Capture(bits, false, input));
        const auto result = multiplayer::session.SubmitRemote(1, frame, input);
        CHECK(result == Netplay::RemoteInputResult::Accepted ||
              result == Netplay::RemoteInputResult::Duplicate ||
              result == Netplay::RemoteInputResult::RollbackRequired ||
              result == Netplay::RemoteInputResult::PredictionCorrect);
    };
    for (unsigned frame = 0; frame < 300; ++frame) {
        submit_remote(frame, held_for(frame));
        const auto tick = p2_tick(world, held_for(frame), 5000 + frame, acc);
        CHECK(tick.ok);
        CHECK_EQ(tick.captures, 1u);
        CHECK_EQ(tick.resimulated, 0u);
        CHECK_EQ(tick.progressed, 1u);
        CHECK_EQ(multiplayer::session.SimFrame(), frame + 1);
        CHECK_EQ(multiplayer::session.CaptureFrame(), multiplayer::session.SimFrame());
        CHECK_EQ(tick.local_held, frame >= delay ? held_for(frame - delay) : 0u);
    }
    CHECK_EQ(multiplayer::session.Restores(), 0u);
    CHECK_EQ(multiplayer::session.Resimulated(), 0u);
    const std::uint32_t stage_frame_before_pause = world.battle.sim.frame;
    const float player_x_before_pause = world.battle.player_x;
    const float player_y_before_pause = world.battle.player_y;
    const std::uint32_t sim_before_pause = multiplayer::session.SimFrame();
    const std::uint32_t capture_before_pause = multiplayer::session.CaptureFrame();

    const std::uint32_t pause_held = 0x002;
    for (unsigned frame = 0; frame < 30; ++frame) {
        submit_remote(sim_before_pause + frame, pause_held);
        const auto tick = p2_tick(world, pause_held, 6000 + frame, acc, true);
        CHECK(tick.ok);
        CHECK_EQ(tick.captures, 1u);
        CHECK_EQ(tick.resimulated, 0u);
        CHECK_EQ(tick.progressed, 1u);
        CHECK_EQ(multiplayer::session.SimFrame(), sim_before_pause + frame + 1);
        CHECK_EQ(multiplayer::session.CaptureFrame(), multiplayer::session.SimFrame());
    }
    CHECK_EQ(world.battle.sim.frame, stage_frame_before_pause);
    CHECK(world.battle.player_x == player_x_before_pause);
    CHECK(world.battle.player_y == player_y_before_pause);
    CHECK_EQ(multiplayer::session.CaptureFrame(), capture_before_pause + 30);
    CHECK_EQ(multiplayer::session.CaptureFrame() - multiplayer::session.SimFrame(), 0u);

    for (unsigned frame = 0; frame < 20; ++frame) {
        const unsigned sim = multiplayer::session.SimFrame();
        submit_remote(sim, held_for(sim));
        const auto tick = p2_tick(world, held_for(sim), 7000 + frame, acc);
        CHECK(tick.ok);
        CHECK_EQ(tick.captures, 1u);
        CHECK_EQ(tick.resimulated, 0u);
        CHECK_EQ(tick.progressed, 1u);
        if (frame < delay) CHECK_EQ(tick.local_held, pause_held);
        else CHECK_EQ(tick.local_held, held_for(sim - delay));
    }
    CHECK_EQ(multiplayer::session.Restores(), 0u);
    std::printf("# capture_phase_lockstep: advance_frames=300 pause_frames=30 "
                "sim=%u capture=%u stage_frame_before_pause=%u "
                "stage_frame_after_pause=%u pause_held=%u\n",
                multiplayer::session.SimFrame(), multiplayer::session.CaptureFrame(),
                stage_frame_before_pause, world.battle.sim.frame, pause_held);
    multiplayer::session.Stop();
}

TEST(barrier_no_deadlock) {
    const unsigned kFrames = 300;
    const auto held_for = [](unsigned frame) -> std::uint32_t {
        return frame % 5 == 0 ? 0x010u : 0x001u;
    };
    for (const unsigned delay : {0u, 2u}) {
        SessionSetup setup = p2_setup();
        setup.session_id = 0x9a3e'0000'0000'0000ull + delay;
        setup.requested_delay = setup.input_delay = delay;
        Fixture world;
        CHECK(world.init());
        CHECK(world.battle.enable_seats(world.provider, 2, {0, 0}));
        CHECK(p2_begin(world, setup));
        Session& session = multiplayer::session;
        const auto submit_remote = [](unsigned frame, std::uint32_t bits) {
            Netplay::FrameInput remote;
            CHECK(InputLanes::Capture(bits, false, remote));
            const auto result = multiplayer::session.SubmitRemote(1, frame, remote);
            CHECK(result == Netplay::RemoteInputResult::Accepted ||
                  result == Netplay::RemoteInputResult::Duplicate ||
                  result == Netplay::RemoteInputResult::PredictionCorrect);
        };
        session.SetPredictionBarrier(true);
        const std::uint64_t stalls_before = session.StallFrames();
        for (unsigned frame = 0; frame < kFrames; ++frame) {
            CHECK(session.Pump(1000 + frame));
            CHECK_EQ(session.CaptureFrame(), session.SimFrame());
            CHECK(session.CaptureLocal(held_for(frame), 1000 + frame));
            session.AdvanceCapturePhase();
            CHECK(session.PredictedFrame());
            CHECK(!session.CanSimulate());
            submit_remote(frame, held_for(frame));
            CHECK(!session.PredictedFrame());
            CHECK(session.CanSimulate());
            const GameInput* local = session.LocalDelayedInput();
            CHECK(local != nullptr);
            CHECK(world.battle.step(*local));
            session.OnFrameSimulated();
            CHECK_EQ(session.SimFrame(), frame + 1);
            CHECK_EQ(session.ConfirmedThrough(), frame);
        }
        CHECK_EQ(session.Checkpoints(), 0u);
        CHECK_EQ(session.Restores(), 0u);
        CHECK_EQ(session.Resimulated(), 0u);
        CHECK_EQ(session.StallFrames(), stalls_before);
        CHECK_EQ(session.Committed(), kFrames);
        session.SetPredictionBarrier(false);
        for (unsigned frame = kFrames; frame < kFrames + 2; ++frame) {
            CHECK_EQ(session.CaptureFrame(), session.SimFrame());
            CHECK(session.CaptureLocal(held_for(frame), 1000 + frame));
            session.AdvanceCapturePhase();
            CHECK(session.PredictedFrame());
            CHECK(session.CanSimulate());
            const GameInput* local = session.LocalDelayedInput();
            CHECK(local != nullptr);
            CHECK(world.battle.step(*local));
            session.OnFrameSimulated();
            CHECK_EQ(session.SimFrame(), frame + 1);
        }
        CHECK_EQ(session.Checkpoints(), 2u);
        std::printf("# barrier_no_deadlock: delay=%u frames=%u sim=%u capture=%u "
                    "checkpoints=%u restores=%u stalls=%llu\n",
                    delay, kFrames, session.SimFrame(), session.CaptureFrame(),
                    session.Checkpoints(), session.Restores(),
                    (unsigned long long)session.StallFrames());
        session.Stop();
    }
}

namespace {
struct WireDigestOutcome {
    bool a_mismatch = false, b_mismatch = false;
    std::uint32_t mismatch_frame = 0xFFFFFFFFu;
    std::uint64_t a_sent = 0, a_received = 0, a_desync = 0;
    std::uint64_t b_sent = 0, b_received = 0, b_desync = 0;
    std::string error;
};

WireDigestOutcome drive_wire_digest(const SessionSetup& setup,
                                    std::uint32_t total_frames,
                                    std::uint32_t tamper_frame) {
    NetplayRuntime a, b;
    CHECK(a.Reset(setup));
    SessionSetup setup_b = setup;
    setup_b.local_player = setup.local_player == 0 ? 1 : 0;
    CHECK(b.Reset(setup_b));
    const auto digest_at = [](std::uint32_t frame) {
        std::uint32_t value = 2166136261u;
        value = (value ^ frame) * 16777619u;
        value = (value ^ (frame >> 16)) * 16777619u;
        return value;
    };
    WireDigestOutcome out;
    for (std::uint32_t frame = 0; frame < total_frames;
         frame += NetplayRuntime::kDigestIntervalFrames) {
        const std::uint32_t da = digest_at(frame);
        std::uint32_t db = da;
        if (frame == tamper_frame) db ^= 1u;
        CHECK(a.SendDigestControl(frame, da));
        CHECK(b.SendDigestControl(frame, db));
        a.NoteRemote(frame, db);
        b.NoteRemote(frame, da);
        if (a.DigestMismatched() && !out.a_mismatch) {
            out.a_mismatch = true;
            out.mismatch_frame = a.DigestMismatchFrame();
        }
        if (b.DigestMismatched()) out.b_mismatch = true;
    }
    out.a_sent = a.DigestsSent();
    out.a_received = a.DigestsReceived();
    out.a_desync = a.DesyncDetected();
    out.b_sent = b.DigestsSent();
    out.b_received = b.DigestsReceived();
    out.b_desync = b.DesyncDetected();
    out.error = a.Error();
    return out;
}
}

TEST(wire_digest_detects) {
    const SessionSetup setup = p2_setup();
    constexpr std::uint32_t kTotalFrames = 3600;
    constexpr std::uint32_t kTamperFrame = 1800;
    constexpr std::uint64_t kSamples =
        kTotalFrames / NetplayRuntime::kDigestIntervalFrames;

    const auto control = drive_wire_digest(setup, kTotalFrames, 0xFFFFFFFFu);
    CHECK(!control.a_mismatch);
    CHECK(!control.b_mismatch);
    CHECK_EQ(control.a_desync, 0ull);
    CHECK_EQ(control.b_desync, 0ull);
    CHECK_EQ(control.a_sent, kSamples);
    CHECK_EQ(control.b_sent, kSamples);
    CHECK_EQ(control.a_received, kSamples);
    CHECK_EQ(control.b_received, kSamples);
    CHECK(control.error.find("TH13 desync") == std::string::npos);

    const auto tampered = drive_wire_digest(setup, kTotalFrames, kTamperFrame);
    CHECK(tampered.a_mismatch);
    CHECK(tampered.b_mismatch);
    CHECK(tampered.mismatch_frame >= kTamperFrame);
    CHECK(tampered.mismatch_frame <
          kTamperFrame + NetplayRuntime::kDigestIntervalFrames);
    CHECK(tampered.error.find("TH13 desync at frame " +
                              std::to_string(kTamperFrame)) != std::string::npos);
    CHECK_EQ(tampered.a_desync, 1ull);
    CHECK_EQ(tampered.b_desync, 1ull);
    std::printf("# wire_digest_detects: samples=%llu tamper_frame=%u "
                "detected_at=%u sent=%llu received=%llu error=%s\n",
                (unsigned long long)kSamples, kTamperFrame,
                tampered.mismatch_frame, (unsigned long long)tampered.a_sent,
                (unsigned long long)tampered.a_received, tampered.error.c_str());
}

TEST(snapshot_cost) {
    SessionSetup setup;
    setup.version = SessionSetup::Version;
    setup.mode = 2;
    setup.session_id = 0xc05c'05c0'1234'5678ull;
    setup.player_count = 2;
    setup.local_player = 0;
    setup.difficulty = 2;
    setup.seed = 47932;
    setup.automatic = false;
    setup.requested_delay = setup.input_delay = 2;
    setup.selections = {0, 0, 0};
    setup.build = {1, 2, 3, 4};
    Fixture rolled;
    CHECK(rolled.init());
    CHECK(rolled.battle.enable_seats(rolled.provider, 2, {0, 0}));
    const auto result = drive_session_world(rolled, setup, 300, 6);
    CHECK(result.sim_frames == 300);
    CHECK(result.restores > 0);
    const uint64_t capture_count = result.capture_us_count;
    CHECK(capture_count > 0);
    const double capture_avg =
        capture_count ? double(result.capture_us_sum) / double(capture_count) : 0;
    std::printf("  snapshot_cost: captures=%llu avg_us=%.1f max_us=%llu "
                "restore_sum_us=%llu restore_max_us=%llu\n",
                (unsigned long long)capture_count, capture_avg,
                (unsigned long long)result.capture_us_max,
                (unsigned long long)result.restore_us_sum,
                (unsigned long long)result.restore_us_max);
    Fixture second;
    CHECK(second.init());
    CHECK(second.battle.enable_seats(second.provider, 2, {0, 0}));
    const auto rerun = drive_session_world(second, setup, 300, 6);
    CHECK(rerun.sim_frames == 300);
    const double second_avg = rerun.capture_us_count
                                  ? double(rerun.capture_us_sum) / double(rerun.capture_us_count)
                                  : 0;
    std::printf("  snapshot_cost: second_avg_us=%.1f (first=%.1f)\n",
                second_avg, capture_avg);
    CHECK(second_avg <= capture_avg * 3.0 + 1.0);
}

TEST(unconsumed_prediction_grant_receives_actual_input) {
    struct Result {
        uint64_t core = 0, full = 0;
        std::string domains;
        std::vector<unsigned> confirmed_frames;
    };
    const auto run = [](bool interleave) {
        Result result;
        Fixture world;
        CHECK(world.init());
        CHECK(world.battle.enable_seats(world.provider, 2, {0, 0}));
        auto setup = p2_setup();
        setup.requested_delay = setup.input_delay = 0;
        CHECK(p2_begin(world, setup));
        auto& session = multiplayer::session;
        session.confirmed_sink = [&](unsigned frame, unsigned,
                const std::array<Netplay::FrameInput, 3>&) {
            result.confirmed_frames.push_back(frame);
        };
        const auto submit = [&](unsigned frame, unsigned held) {
            Netplay::FrameInput input;
            CHECK(InputLanes::Capture(held, false, input));
            CHECK(session.SubmitRemote(1, frame, input) ==
                  Netplay::RemoteInputResult::Accepted);
        };
        for (unsigned frame = 0; frame < 41; ++frame) {
            CHECK(session.CaptureLocal(0, frame));
            session.AdvanceCapturePhase();
            if (!interleave || frame != 17) submit(frame, frame >= 17 ? 1 : 0);
            CHECK(session.Pump(frame * 2));
            CHECK(session.CanSimulate());
            if (interleave && frame == 17) {
                CHECK_EQ(world.battle.seat_inputs[1].held, 0u);
                submit(frame, 1);
                CHECK(!session.HasRollbackRequest());
                CHECK(session.Pump(frame * 2 + 1));
                CHECK(session.CanSimulate());
                CHECK_EQ(world.battle.seat_inputs[1].held, 1u);
                CHECK_EQ(world.battle.seat_inputs[1].duration[0], 1u);
            }
            const auto* local = session.LocalDelayedInput();
            CHECK(local != nullptr);
            CHECK(world.battle.step(*local));
            session.OnFrameSimulated();
            CHECK_EQ(session.SimFrame(), frame + 1);
            CHECK_EQ(session.Committed(), frame + 1);
            CHECK_EQ(session.RecordCount(), size_t(0));
            CHECK(!session.RecordOverflow());
        }
        CHECK_EQ(result.confirmed_frames.size(), size_t(41));
        for (unsigned frame = 0; frame < result.confirmed_frames.size(); ++frame)
            CHECK_EQ(result.confirmed_frames[frame], frame);
        WorldState final;
        CHECK(final.capture(world.battle, world.visual));
        result.core = final.digest_core();
        result.full = final.digest();
        char domains[4096];
        final.digest_domains(domains, sizeof domains);
        result.domains = domains;
        session.confirmed_sink = nullptr;
        session.Stop();
        return result;
    };
    const auto baseline = run(false), interleaved = run(true);
    CHECK_EQ(interleaved.core, baseline.core);
    CHECK_EQ(interleaved.full, baseline.full);
    CHECK_EQ(interleaved.domains, baseline.domains);
    CHECK_EQ(interleaved.confirmed_frames, baseline.confirmed_frames);
}

int main() { return th13test::run_all("m8.rollback"); }
