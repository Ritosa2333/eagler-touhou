#include <eagler/netplay/BrowserPeerTransport.hpp>
#include <eagler/netplay/SessionChannel.hpp>
#include <eagler/netplay/SpectatorStagePublisher.hpp>
#include <eagler/netplay/SpectatorStageView.hpp>
#include <netplay/NetplayProtocolConfig.hpp>
#include <emscripten/emscripten.h>

#include "ThibpInput.hpp"

#include <array>
#include <cmath>
#include <cstdint>

namespace {
constexpr std::uint32_t gameId = 0x50424954; // TIBP in little-endian wire order.
constexpr unsigned maxDelay = 8;
constexpr unsigned viewBatch = 20;
static_assert(thibp::maxPlayers == Netplay::MAX_PLAYERS);

Netplay::BrowserPeerTransport transport;
Netplay::SessionGate gate;
Netplay::RollbackCore core;
Netplay::SessionChannel channel(transport);
Netplay::FrameDecision prepared;
thibp::InputFrame inputs;
std::uint32_t nextFrame = 0;
std::uint32_t nextCapture = 0;

Netplay::SpectatorStagePublisher publisher;
std::uint32_t streamGeneration = Netplay::INVALID_FRAME;
std::uint32_t streamOrigin = 0, streamStage = 0, nextPublished = 0;
std::uint32_t checkpointFrame = Netplay::INVALID_FRAME;
std::vector<std::uint8_t> stageInitializer;
bool publishing = false;

Netplay::SpectatorStagePlayback playback(Netplay::SpectatorStagePlayback::RestoreMode::Run, true);
Netplay::SpectatorStageView playbackView;
std::uint64_t viewSession = 0;
std::uint32_t viewSeed = 0, viewAbi = 0, viewPlayers = 0;
std::array<std::uint32_t, 12 + viewBatch * Netplay::MAX_PLAYERS> viewSteps{};

bool sendSpectator(const std::vector<std::uint8_t>& packet)
{
    return transport.SendSpectator(packet.data(), packet.size());
}
Netplay::FrameInput decodeInput(std::uint32_t value)
{
    Netplay::FrameInput input(static_cast<std::uint16_t>(value & thibp::validButtons));
    if (thibp::hasTouchPosition(value)) {
        input.analogMode = Netplay::AnalogMode::DirectTouch;
        input.x = thibp::touchX(value);
        input.y = thibp::touchY(value);
        input.unlimited = input.touchUsed = true;
    }
    return input;
}

bool encodeInput(const Netplay::FrameInput& input, std::uint32_t& value)
{
    if (input.buttons > thibp::validButtons || input.touchBomb) return false;
    if (input.analogMode == Netplay::AnalogMode::None) {
        value = input.buttons;
        return !input.unlimited && !input.touchUsed && input.x == 0 && input.y == 0;
    }
    if (input.analogMode != Netplay::AnalogMode::DirectTouch || !input.unlimited || !input.touchUsed ||
        !(input.x >= 0 && input.x <= thibp::maxTouchX && input.x == std::floor(input.x)) ||
        !(input.y >= 0 && input.y <= thibp::maxTouchY && input.y == std::floor(input.y))) return false;
    value = thibp::withTouchPosition(input.buttons, static_cast<unsigned>(input.x), static_cast<unsigned>(input.y));
    return true;
}

std::uint64_t nowMs()
{
    return static_cast<std::uint64_t>(emscripten_get_now());
}
}

extern "C" {
EMSCRIPTEN_KEEPALIVE void thibp_room_close()
{
    channel.Clear();
    gate.Clear();
    core.Clear();
    transport.Close();
    prepared = {};
    inputs = {};
    nextFrame = nextCapture = 0;
    publisher = {};
    streamGeneration = checkpointFrame = Netplay::INVALID_FRAME;
    streamOrigin = streamStage = nextPublished = 0;
    stageInitializer.clear();
    publishing = false;
    playback.Clear();
    playbackView.Clear();
    viewSession = viewSeed = viewAbi = viewPlayers = 0;
}

EMSCRIPTEN_KEEPALIVE int thibp_room_open(const char* url, unsigned seat, unsigned seats,
                                        unsigned low, unsigned high, unsigned seed,
                                        unsigned gameplay, unsigned delay)
{
    if (!url || seats < 2 || seats > Netplay::MAX_PLAYERS || seat >= seats ||
        delay > maxDelay || (low == 0 && high == 0)) return 0;

    thibp_room_close();
    Netplay::SessionConfig session;
    session.sessionId = (std::uint64_t(high) << 32) | low;
    session.seed = seed;
    session.gameId = gameId;
    session.gameplayAbi = gameplay;
    session.playerCount = static_cast<std::uint8_t>(seats);
    session.localPlayer = static_cast<std::uint8_t>(seat);

    Netplay::CoreConfig input;
    input.sessionId = session.sessionId;
    input.playerCount = session.playerCount;
    input.localPlayer = session.localPlayer;
    input.inputDelay = static_cast<std::uint8_t>(delay);
    input.allowPrediction = true;
    input.maxRollbackFrames = 4;
    input.predictableButtons = 0x014f; // Movement, fire, and focus.
    input.directionButtons = 0x000f;
    input.prediction = Netplay::InputPrediction::ConfirmedHold;
    input.directTouchIsAbsolute = true;
    return gate.Reset(session) && core.Reset(input) &&
           transport.Connect(url, input.localPlayer, input.playerCount, true) &&
           channel.BeginSession(session, nowMs());
}

EMSCRIPTEN_KEEPALIVE const char* thibp_room_error()
{
    return channel.Error() == Netplay::SessionChannel::Failure::Transport
        ? transport.LastError().c_str() : channel.ErrorText();
}

EMSCRIPTEN_KEEPALIVE int thibp_room_pump()
{
    if (!channel.Pump(gate, core, nowMs(), true)) return -1;
    if (transport.Disconnected()) return -2;
    return gate.CanStart() ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE int thibp_room_capture(unsigned frame, unsigned value)
{
    if (!gate.CanStart() || frame != nextCapture || !thibp::validInput(value) ||
        !core.ScheduleLocalInput(frame, decodeInput(value))) return 0;
    ++nextCapture;
    return channel.LocalCaptured(core, frame, nowMs());
}

EMSCRIPTEN_KEEPALIVE std::uintptr_t thibp_room_prepare(unsigned frame)
{
    if (!gate.CanStart() || frame != nextFrame) return 0;
    const auto confirmed = core.ConfirmedThroughAllRemotes();
    if (confirmed == Netplay::INVALID_FRAME || confirmed < frame) return 0;
    prepared = core.PrepareFrame(frame);
    if (!prepared.canAdvance || prepared.predictedMask) return 0;
    for (unsigned seat = 0; seat < gate.Config().playerCount; ++seat) {
        if (!encodeInput(prepared.inputs[seat], inputs[seat])) return 1;
    }
    return reinterpret_cast<std::uintptr_t>(inputs.data());
}

EMSCRIPTEN_KEEPALIVE std::uintptr_t thibp_room_forecast(unsigned frame)
{
    if (!gate.CanStart() || frame < nextFrame || frame - nextFrame >= 4) return 0;
    const auto decision = core.PrepareFrame(frame);
    if (!decision.canAdvance) return 0;
    for (unsigned seat = 0; seat < gate.Config().playerCount; ++seat) {
        if (!encodeInput(decision.inputs[seat], inputs[seat])) return 0;
    }
    return reinterpret_cast<std::uintptr_t>(inputs.data());
}

EMSCRIPTEN_KEEPALIVE int thibp_room_commit(unsigned frame)
{
    if (frame != nextFrame || !prepared.canAdvance || !core.MarkSimulated(frame, prepared)) return 0;
    prepared = {};
    ++nextFrame;
    return 1;
}

EMSCRIPTEN_KEEPALIVE int thibp_stream_stage(unsigned generation, unsigned tick, unsigned stage,
    unsigned difficulty, unsigned types, unsigned challenge, unsigned one, unsigned two, unsigned three)
{
    const auto& config = gate.Config();
    if (!gate.CanStart() || config.localPlayer != 0 || stage < 1 || stage > 7 ||
        ((one | two | three) & ~thibp::validButtons) != 0) return 0;
    if (generation != streamGeneration) {
        const auto id = config.sessionId ^ (std::uint64_t(generation) * 0x9e3779b97f4a7c15ull);
        if (!publisher.Reset(Netplay::ProtocolConfig::MagicGame, config.playerCount,
                             id, config.gameplayAbi)) return 0;
        transport.BeginSpectatorSession(id, config.seed);
        streamGeneration = generation;
        streamOrigin = nextPublished = tick;
        publishing = true;
    }
    if (!publishing || tick < nextPublished) return 0;
    streamStage = stage;
    checkpointFrame = tick;
    stageInitializer = {static_cast<std::uint8_t>(difficulty), static_cast<std::uint8_t>(challenge)};
    stageInitializer.resize(10);
    using Netplay::SpectatorStageWire::Put;
    Put(stageInitializer, 2, types, 2);
    Put(stageInitializer, 4, one, 2);
    Put(stageInitializer, 6, two, 2);
    Put(stageInitializer, 8, three, 2);
    return 1;
}

EMSCRIPTEN_KEEPALIVE int thibp_stream_publish(unsigned consumed, int finished)
{
    if (!publishing || consumed < nextPublished) return 0;
    unsigned budget = 24;
    while (nextPublished < consumed && budget--) {
        Netplay::SpectatorStagePublisher::Frame frame;
        frame.frame = nextPublished - streamOrigin;
        if (checkpointFrame == nextPublished) {
            frame.stage = streamStage;
            frame.schema = 1;
            frame.initializer = stageInitializer;
        } else {
            frame.stage = checkpointFrame > nextPublished && checkpointFrame != Netplay::INVALID_FRAME
                ? streamStage - 1 : streamStage;
        }
        frame.hasInputs = core.ConfirmedInputs(nextPublished, &frame.inputs);
        if (!frame.hasInputs || !publisher.Record(std::move(frame))) return -1;
        Netplay::SpectatorStagePublisher::PublishFailure failure{};
        const unsigned relative = nextPublished - streamOrigin;
        const auto noAlternateInput = [](unsigned, auto&) { return false; };
        if (!publisher.Publish(relative, relative, noAlternateInput, sendSpectator, &failure)) {
            if (Netplay::SpectatorStagePublisher::Retryable(failure)) return 0;
            return -1;
        }
        ++nextPublished;
        if (checkpointFrame + 1 == nextPublished) checkpointFrame = Netplay::INVALID_FRAME;
    }
    if (finished && nextPublished == consumed && publisher.Finish(consumed - streamOrigin, sendSpectator)) {
        publishing = false;
        transport.StopSpectators(true);
        return 1;
    }
    return 0;
}

EMSCRIPTEN_KEEPALIVE int thibp_view_open(const char* url, const char* id, unsigned seats, unsigned abi)
{
    if (!url || !id || seats < 2 || seats > Netplay::MAX_PLAYERS || !abi) return 0;
    thibp_room_close();
    viewPlayers = seats;
    viewAbi = abi;
    return transport.ConnectSpectator(url, id, static_cast<std::uint8_t>(seats), true);
}

EMSCRIPTEN_KEEPALIVE int thibp_view_pump()
{
    if (!viewPlayers || transport.Failed()) return -1;
    std::uint64_t session = 0;
    std::uint32_t seed = 0;
    if (transport.TakeSpectatorSession(&session, &seed)) {
        viewSession = session;
        viewSeed = seed;
        playback.Clear();
        playbackView.Clear();
    }
    std::vector<std::uint8_t> packet;
    for (unsigned count = 0; count < 384 && transport.Poll(&packet); ++count) {
        Netplay::SpectatorStageEvent event;
        Netplay::SpectatorFramePacket input;
        if (Netplay::DecodeSpectatorStageEvent(packet.data(), packet.size(), &event)) {
            if (event.game != Netplay::ProtocolConfig::MagicGame || event.sessionId != viewSession ||
                event.gameplayAbi != viewAbi || event.playerCount != viewPlayers || !playback.Receive(event)) return -1;
        } else if (Netplay::DecodeSpectatorFramePacket(packet.data(), packet.size(), &input)) {
            if (!playback.Receive(input)) return -1;
        } else return -1;
    }
    return playback.Ready() ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE std::uintptr_t thibp_view_step(unsigned budget)
{
    viewSteps[0] = viewSteps[1] = 0;
    viewSteps[2] = playback.FastForwarding();
    viewSteps[3] = playback.Position();
    if (!playback.Step(
        [&](const Netplay::SpectatorStageEvent& event) {
            if (event.schema != 1 || event.state.size() != 10 || event.frame != 0) return false;
            viewSteps[0] = 1;
            viewSteps[3] = 0;
            viewSteps[4] = viewSeed;
            viewSteps[5] = event.state[0];
            viewSteps[6] = event.state[1];
            for (unsigned i = 0; i < 4; ++i)
                viewSteps[7 + i] = Netplay::SpectatorStageWire::Get(event.state.data(), 2 + i * 2, 2);
            return true;
        },
        [&](const Netplay::SpectatorFramePacket& input, bool) {
            auto offset = 12 + viewSteps[1]++ * Netplay::MAX_PLAYERS;
            for (unsigned seat = 0; seat < Netplay::MAX_PLAYERS; ++seat)
                if (!encodeInput(input.inputs[seat], viewSteps[offset + seat])) return false;
            return true;
        },
        [&] { return viewSteps[0] != 0; }, std::min(budget, viewBatch))) return 0;
    return reinterpret_cast<std::uintptr_t>(viewSteps.data());
}

EMSCRIPTEN_KEEPALIVE const char* thibp_view_state()
{
    return playbackView.State(playback);
}

EMSCRIPTEN_KEEPALIVE int thibp_view_control(const char* run, const char* action, unsigned frame)
{
    return playbackView.Control(playback, run, action, frame);
}
}
