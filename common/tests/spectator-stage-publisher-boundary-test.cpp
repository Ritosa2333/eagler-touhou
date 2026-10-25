#include <eagler/netplay/SpectatorStagePublisher.hpp>
#include <eagler/netplay/SpectatorPublicationTail.hpp>
#include <eagler/netplay/SpectatorStageHistory.hpp>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <memory>
#include <vector>

using namespace Netplay;
using Requirement = SpectatorStagePublisher::CheckpointRequirement;
using PublishFailure = SpectatorStagePublisher::PublishFailure;

namespace {

constexpr std::uint64_t Session = 0x5345474d454e5431ull;
constexpr std::uint32_t Abi = 0x07050001u;

SpectatorStagePublisher::Frame make_frame(std::uint32_t frame, std::uint32_t stage,
                                           std::uint8_t players,
                                           std::vector<std::uint8_t> initializer = {}) {
    SpectatorStagePublisher::Frame result;
    result.frame = frame;
    result.stage = stage;
    result.schema = initializer.empty() ? 0u : 5u;
    result.initializer = std::move(initializer);
    result.hasInputs = true;
    for (std::uint8_t player = 0; player < players; ++player)
        result.inputs[player] = FrameInput(static_cast<std::uint16_t>((frame + player) & 0xffffu));
    return result;
}

bool publish_one(SpectatorStagePublisher &publisher, std::uint32_t frame,
                 const std::vector<std::uint8_t> &initializer,
                 std::uint32_t stage, std::uint8_t players,
                 const SpectatorStagePublisher::Frame *recordOverride = nullptr) {
    auto record = recordOverride ? *recordOverride : make_frame(frame, stage, players, initializer);
    if (!publisher.Record(std::move(record))) return false;
    return publisher.Publish(frame, frame,
        [](std::uint32_t, std::array<FrameInput, MAX_PLAYERS> &) { return false; },
        [](const std::vector<std::uint8_t> &) { return true; });
}

struct BoundaryWireAudit {
    std::vector<std::vector<std::uint8_t>> stageTwoInitializers;
    std::vector<std::uint32_t> boundaryInputFrames;
    bool operator()(const std::vector<std::uint8_t> &wire) {
        if (IsSpectatorStageEvent(wire.data(), wire.size())) {
            SpectatorStageEvent event;
            assert(DecodeSpectatorStageEvent(wire.data(), wire.size(), &event));
            if (event.kind == SpectatorEventKind::Checkpoint && event.stage == 2)
                stageTwoInitializers.push_back(event.state);
            return true;
        }
        SpectatorFramePacket packet;
        assert(DecodeSpectatorFramePacket(wire.data(), wire.size(), &packet));
        if (packet.frame == 6746) boundaryInputFrames.push_back(packet.frame);
        return true;
    }
};

void stage_boundary_replay(std::uint8_t players) {
    SpectatorStagePublisher publisher;
    assert(publisher.Reset('7', players, Session + players, Abi, 0));
    BoundaryWireAudit audit;

    assert(publisher.NeedsCheckpoint(0, 1) == Requirement::Required);
    auto first = make_frame(0, 1, players, {0x10, 0x11});
    assert(publisher.Record(first));
    assert(publisher.Publish(0, 0,
        [](std::uint32_t, std::array<FrameInput, MAX_PLAYERS> &) { return false; },
        [&](const std::vector<std::uint8_t> &wire) { return audit(wire); }));

    // Published prefixes are idempotent for unchanged inputs and reject a
    // revised input. They never request a replacement initializer.
    assert(publisher.NeedsCheckpoint(0, 1) == Requirement::AlreadyPublished);
    assert(publisher.Record(first));
    auto changedPublishedInput = first;
    changedPublishedInput.inputs[0].buttons ^= 0x4000u;
    assert(!publisher.Record(changedPublishedInput));

    for (std::uint32_t frame = 1; frame < 6746; ++frame) {
        assert(publisher.NeedsCheckpoint(frame, 1) == Requirement::NotRequired);
        assert(publish_one(publisher, frame, {}, 1, players));
    }

    // First speculative Stage 2 execution captures a provisional initializer.
    assert(publisher.NeedsCheckpoint(6746, 2) == Requirement::Required);
    assert(publisher.Record(make_frame(6746, 2, players, {0xa1, 0xa2})));
    // RestoreTo(6746) replays that exact logical frame. The query must remain
    // stage-boundary aware so the title recaptures, rather than retaining the
    // provisional CP or overwriting it with a no-CP record.
    assert(publisher.NeedsCheckpoint(6746, 2) == Requirement::Required);
    assert(publisher.Record(make_frame(6746, 2, players, {0xb1, 0xb2, 0xb3})));
    assert(publisher.Publish(6746, 6746,
        [](std::uint32_t, std::array<FrameInput, MAX_PLAYERS> &) { return false; },
        [&](const std::vector<std::uint8_t> &wire) { return audit(wire); }));
    assert(audit.stageTwoInitializers.size() == 1);
    assert((audit.stageTwoInitializers.front() == std::vector<std::uint8_t>{0xb1, 0xb2, 0xb3}));
    assert(audit.boundaryInputFrames.size() == 1 && audit.boundaryInputFrames.front() == 6746);

    assert(publisher.NeedsCheckpoint(6747, 2) == Requirement::NotRequired);
    assert(publish_one(publisher, 6747, {}, 2, players));
    assert(audit.stageTwoInitializers.size() == 1);
}

void missing_initializer_is_rejected() {
    SpectatorStagePublisher publisher;
    assert(publisher.Reset('7', 2, Session + 20, Abi));
    assert(publish_one(publisher, 0, {0x01}, 1, 2));
    for (std::uint32_t frame = 1; frame < 6746; ++frame)
        assert(publish_one(publisher, frame, {}, 1, 2));
    assert(publisher.NeedsCheckpoint(6746, 2) == Requirement::Required);
    // This is the old-cache failure mode: a stale title-side stage value
    // suppresses the initializer, so the real publisher must reject the
    // stage change instead of silently emitting an invalid stream.
    assert(publisher.Record(make_frame(6746, 2, 2)));
    PublishFailure failure = PublishFailure::None;
    assert(!publisher.Publish(6746, 6746,
        [](std::uint32_t, std::array<FrameInput, MAX_PLAYERS> &) { return false; },
        [](const std::vector<std::uint8_t> &) { return true; }, &failure));
    assert(failure == PublishFailure::StageCheckpointMissing);
}

void checkpoint_send_failure_is_retryable() {
    SpectatorStagePublisher publisher;
    assert(publisher.Reset('7', 2, Session + 50, Abi));
    assert(publisher.Record(make_frame(0, 1, 2, {0x51})));
    unsigned checkpointAttempts = 0;
    unsigned checkpointSuccesses = 0;
    unsigned inputSuccesses = 0;
    bool failFirstCheckpoint = true;
    PublishFailure failure = PublishFailure::None;
    const auto send = [&](const std::vector<std::uint8_t> &wire) {
        if (IsSpectatorStageEvent(wire.data(), wire.size())) {
            SpectatorStageEvent event;
            assert(DecodeSpectatorStageEvent(wire.data(), wire.size(), &event));
            if (event.kind == SpectatorEventKind::Checkpoint) {
                ++checkpointAttempts;
                if (failFirstCheckpoint) {
                    failFirstCheckpoint = false;
                    return false;
                }
                ++checkpointSuccesses;
            }
        } else {
            ++inputSuccesses;
        }
        return true;
    };
    const auto inputs = [](std::uint32_t, std::array<FrameInput, MAX_PLAYERS> &) { return false; };
    assert(!publisher.Publish(0, 0, inputs, send, &failure));
    assert(failure == PublishFailure::CheckpointSend);
    assert(publisher.Serial() == 0 && publisher.Published() == 0);
    assert(publisher.Publish(0, 0, inputs, send, &failure));
    assert(failure == PublishFailure::None);
    assert(checkpointAttempts == 2 && checkpointSuccesses == 1 && inputSuccesses == 1);
}

void input_send_failure_retries_without_duplicate_checkpoint() {
    SpectatorStagePublisher publisher;
    assert(publisher.Reset('7', 2, Session + 60, Abi));
    assert(publisher.Record(make_frame(0, 1, 2, {0x61})));
    unsigned checkpoints = 0;
    unsigned inputAttempts = 0;
    unsigned inputSuccesses = 0;
    bool failFirstInput = true;
    PublishFailure failure = PublishFailure::None;
    const auto send = [&](const std::vector<std::uint8_t> &wire) {
        if (IsSpectatorStageEvent(wire.data(), wire.size())) {
            ++checkpoints;
            return true;
        }
        ++inputAttempts;
        if (failFirstInput) {
            failFirstInput = false;
            return false;
        }
        ++inputSuccesses;
        return true;
    };
    const auto inputs = [](std::uint32_t, std::array<FrameInput, MAX_PLAYERS> &) { return false; };
    assert(!publisher.Publish(0, 0, inputs, send, &failure));
    assert(failure == PublishFailure::InputSend);
    assert(publisher.Serial() == 1 && publisher.Published() == 0);
    assert(publisher.Publish(0, 0, inputs, send, &failure));
    assert(failure == PublishFailure::None);
    assert(checkpoints == 1 && inputAttempts == 2 && inputSuccesses == 1);
}

void confirmed_input_unavailable_is_structural() {
    SpectatorStagePublisher publisher;
    assert(publisher.Reset('7', 2, Session + 70, Abi));
    auto frame = make_frame(0, 1, 2, {0x71});
    frame.hasInputs = false;
    assert(publisher.Record(std::move(frame)));
    PublishFailure failure = PublishFailure::None;
    assert(!publisher.Publish(0, 0,
        [](std::uint32_t, std::array<FrameInput, MAX_PLAYERS> &) { return false; },
        [](const std::vector<std::uint8_t> &) { return true; }, &failure));
    assert(failure == PublishFailure::InputUnavailable);
    assert(!SpectatorStagePublisher::Retryable(failure));
    // The checkpoint send has succeeded, so only the missing confirmed input
    // remains. Production treats that missing input as a permanent lane error.
    assert(publisher.Serial() == 1 && publisher.Published() == 0);
}

void upload_wait_preserves_order_and_navigation(unsigned blockedPhase, std::uint8_t players) {
    SpectatorStagePublisher publisher;
    assert(publisher.Reset('7', players, Session + 80 + blockedPhase + players * 10, Abi));
    bool blocked = true;
    unsigned refused = 0;
    std::vector<std::uint32_t> frames, checkpoints, bossFrames, serials;
    const auto inputs = [](std::uint32_t, std::array<FrameInput, MAX_PLAYERS>&) { return false; };
    const auto send = [&](const std::vector<std::uint8_t>& wire) {
        if (IsSpectatorStageEvent(wire.data(), wire.size())) {
            SpectatorStageEvent event;
            assert(DecodeSpectatorStageEvent(wire.data(), wire.size(), &event));
            const unsigned phase = event.kind == SpectatorEventKind::Checkpoint ? 0 : 2;
            if (blocked && phase == blockedPhase) {++refused; return false;}
            serials.push_back(event.serial);
            if (event.kind == SpectatorEventKind::Checkpoint) checkpoints.push_back(event.frame);
            if (event.kind == SpectatorEventKind::Node) bossFrames.push_back(event.frame);
        } else {
            if (blocked && blockedPhase == 1) {++refused; return false;}
            SpectatorFramePacket packet;
            assert(DecodeSpectatorFramePacket(wire.data(), wire.size(), &packet));
            assert(packet.playerCount == players);
            frames.push_back(packet.frame);
        }
        return true;
    };
    // Nine normal simulation ticks accumulate while one publication phase is
    // blocked. A stage boundary enters the same backlog as ordinary input.
    for (unsigned frame = 0; frame < 12; ++frame) {
        auto record = make_frame(frame, frame < 6 ? 1 : 2, players,
            frame == 0 || frame == 6 ? std::vector<std::uint8_t>{std::uint8_t(frame + 1)} : std::vector<std::uint8_t>{});
        if (frame == 0) record.bossSlots = 3;
        assert(publisher.Record(std::move(record)));
        blocked = frame < 9;
        PublishFailure failure{};
        if (!publisher.Publish(frame, frame, inputs, send, &failure)) {
            assert(blocked);
            assert(SpectatorStagePublisher::Retryable(failure));
        }
    }
    assert(refused == 9);
    assert(publisher.Published() == 12);
    assert(frames.size() == 12);
    for (unsigned frame = 0; frame < frames.size(); ++frame) assert(frames[frame] == frame);
    assert((checkpoints == std::vector<std::uint32_t>{0, 6}));
    assert((bossFrames == std::vector<std::uint32_t>{1}));
    assert((serials == std::vector<std::uint32_t>{1, 2, 3}));
    assert(publisher.Finish(12, send));
    assert((serials == std::vector<std::uint32_t>{1, 2, 3, 4}));
}

void retired_world_tail_survives_upload_wait(std::uint8_t players, unsigned blockedPacket) {
    SpectatorStageHistory received;
    SpectatorPublicationTail tail;
    unsigned packet = 0;
    bool blocked = true;
    const auto send = [&](const std::vector<std::uint8_t>& wire) {
        if (blocked && packet == blockedPacket) return false;
        ++packet;
        if (IsSpectatorStageEvent(wire.data(), wire.size())) {
            SpectatorStageEvent event;
            assert(DecodeSpectatorStageEvent(wire.data(), wire.size(), &event));
            assert(received.Accept(event));
        } else {
            SpectatorFramePacket input;
            assert(DecodeSpectatorFramePacket(wire.data(), wire.size(), &input));
            assert(received.Accept(input));
        }
        return true;
    };
    {
        SpectatorStagePublisher publisher;
        assert(publisher.Reset('7', players, Session + 200 + players, Abi));
        for (unsigned frame = 0; frame < 16; ++frame) {
            auto record = make_frame(frame, 1, players, frame == 0 ? std::vector<std::uint8_t>{1} : std::vector<std::uint8_t>{});
            if (frame == 0) record.bossSlots = 1;
            assert(publisher.Record(std::move(record)));
        }
        // Capture the unsent suffix, even if a checkpoint, input or Boss node
        // was already accepted before retirement. Include the end packet.
        publisher.Publish(15, 15, [](auto, auto&) {return false;}, send);
        assert(tail.Freeze(publisher, 15, 15, [](auto, auto&) {return false;}, 100));
    } // Original world, publisher and input ring are now gone.
    using Result = SpectatorPublicationTail::Result;
    assert(tail.Drain(101, false, send, [] {return 0ull;}) == Result::Waiting);
    assert(tail.Pending());
    blocked = false;
    assert(tail.Drain(250, false, send, [] {return 0ull;}) == Result::Complete);
    assert(!tail.Pending());
    assert(received.Ended() && received.Received() == 16);
    assert(packet == 19); // initializer + 16 inputs + Boss navigation + End
    assert(tail.Drain(251, false, send, [] {return 0ull;}) == Result::Idle);
    assert(packet == 19);
}

void retirement_tail_budget_and_expiry() {
    using Result = SpectatorPublicationTail::Result;
    const auto make = [] {
        auto publisher = std::make_unique<SpectatorStagePublisher>();
        assert(publisher->Reset('7', 2, Session + 300, Abi));
        for (unsigned frame = 0; frame < 64; ++frame)
            assert(publisher->Record(make_frame(frame, 1, 2,
                frame == 0 ? std::vector<std::uint8_t>{1} : std::vector<std::uint8_t>{})));
        return publisher;
    };
    SpectatorPublicationTail tail;
    auto publisher = make();
    assert(tail.Freeze(*publisher, 63, 63, [](auto, auto&) {return false;}, 100));
    unsigned sent = 0;
    const auto send = [&](const auto&) {++sent; return true;};
    assert(tail.Drain(101, false, send, [] {return 0ull;}) == Result::Waiting);
    assert(sent == 32);
    std::uint64_t clock = 0;
    assert(tail.Drain(102, false, send, [&] {return clock++ * 1000;}) == Result::Waiting);
    assert(sent == 34);
    assert(tail.Drain(15'100, false, send, [] {return 0ull;}) == Result::Failed);
    assert(sent == 34 && !tail.Pending());
    publisher = make();
    assert(tail.Freeze(*publisher, 63, 63, [](auto, auto&) {return false;}, 200));
    assert(tail.Drain(201, true, send, [] {return 0ull;}) == Result::Failed);
    assert(sent == 34 && !tail.Pending());
}

void nonzero_start_and_missing_history() {
    SpectatorStagePublisher publisher;
    assert(publisher.Reset('7', 2, Session + 30, Abi, 100));
    assert(publisher.NeedsCheckpoint(100, 4) == Requirement::Required);
    assert(publish_one(publisher, 100, {0x40}, 4, 2));
    assert(publisher.NeedsCheckpoint(101, 4) == Requirement::NotRequired);
    assert(publisher.NeedsCheckpoint(99, 4) == Requirement::MissingPredecessor);

    // A cached future frame cannot stand in for a missing contiguous prefix.
    assert(publisher.Record(make_frame(103, 5, 2)));
    assert(publisher.NeedsCheckpoint(104, 5) == Requirement::MissingPredecessor);
}

void overwritten_predecessor_is_explicit() {
    SpectatorStagePublisher publisher;
    assert(publisher.Reset('7', 2, Session + 40, Abi));
    for (std::uint32_t frame = 0; frame <= 512; ++frame)
        assert(publish_one(publisher, frame, frame == 0 ? std::vector<std::uint8_t>{0x01}
                                                        : std::vector<std::uint8_t>{},
                           1, 2));
    // 768 aliases frame 512 in the bounded ring; once overwritten, the query
    // must report missing history rather than guessing from any newer frame.
    assert(publisher.Record(make_frame(768, 2, 2)));
    assert(publisher.NeedsCheckpoint(513, 2) == Requirement::MissingPredecessor);
}

} // namespace

int main() {
    stage_boundary_replay(2);
    stage_boundary_replay(3);
    missing_initializer_is_rejected();
    checkpoint_send_failure_is_retryable();
    input_send_failure_retries_without_duplicate_checkpoint();
    confirmed_input_unavailable_is_structural();
    for (const auto players : {2, 3})
        for (unsigned phase = 0; phase < 3; ++phase)
            upload_wait_preserves_order_and_navigation(phase, std::uint8_t(players));
    for (const auto players : {2, 3})
        for (const auto packet : {0u, 1u, 2u, 18u})
            retired_world_tail_survives_upload_wait(std::uint8_t(players), packet);
    retirement_tail_budget_and_expiry();
    nonzero_start_and_missing_history();
    overwritten_predecessor_is_explicit();
    std::cout << "PASS publisher derives and replays stage boundary from contiguous records (2P/3P, firstFrame=100)\n";
}
