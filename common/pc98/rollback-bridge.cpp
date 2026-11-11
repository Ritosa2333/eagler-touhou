#include <eagler/netplay/NetplayCore.hpp>
#include <cstdint>

using namespace Netplay;

struct Pc98Rollback {
    RollbackCore core;
    FrameDecision prepared{};
    std::uint32_t preparedFrame = INVALID_FRAME;
};

extern "C" {
Pc98Rollback *pc98_rollback_create(unsigned players, unsigned local, unsigned limit,
                                    unsigned held, unsigned directions, unsigned directionFrames) {
    auto *state = new Pc98Rollback;
    CoreConfig config{};
    config.playerCount = static_cast<std::uint8_t>(players);
    config.localPlayer = static_cast<std::uint8_t>(local);
    config.maxRollbackFrames = static_cast<std::uint8_t>(limit);
    config.predictableButtons = static_cast<std::uint16_t>(held);
    config.directionButtons = static_cast<std::uint16_t>(directions);
    config.maxDirectionPredictionFrames = static_cast<std::uint8_t>(directionFrames);
    config.predictAnalog = false;
    if (!state->core.Reset(config)) { delete state; return nullptr; }
    return state;
}
void pc98_rollback_destroy(Pc98Rollback *state) { delete state; }
static FrameInput input(unsigned buttons, unsigned mode, float x, float y, unsigned flags) {
    FrameInput value{};
    value.buttons = static_cast<std::uint16_t>(buttons);
    value.analogMode = static_cast<AnalogMode>(mode);
    value.x = x; value.y = y;
    value.unlimited = (flags & 1) != 0;
    value.touchUsed = (flags & 2) != 0;
    value.touchBomb = (flags & 4) != 0;
    return value;
}
int pc98_rollback_local(Pc98Rollback *state, unsigned frame, unsigned buttons,
                        unsigned mode, float x, float y, unsigned flags) {
    return state->core.ScheduleLocalInput(frame, input(buttons, mode, x, y, flags));
}
int pc98_rollback_remote(Pc98Rollback *state, unsigned slot, unsigned frame, unsigned buttons,
                         unsigned mode, float x, float y, unsigned flags) {
    return static_cast<int>(state->core.SubmitRemoteInput(static_cast<std::uint8_t>(slot), frame,
                                                         input(buttons, mode, x, y, flags)));
}
int pc98_rollback_prepare(Pc98Rollback *state, unsigned frame) {
    state->prepared = state->core.PrepareFrame(frame);
    state->preparedFrame = frame;
    return state->prepared.canAdvance;
}
unsigned pc98_rollback_buttons(Pc98Rollback *state, unsigned slot) { return state->prepared.inputs[slot].buttons; }
unsigned pc98_rollback_mode(Pc98Rollback *state, unsigned slot) { return static_cast<unsigned>(state->prepared.inputs[slot].analogMode); }
float pc98_rollback_x(Pc98Rollback *state, unsigned slot) { return state->prepared.inputs[slot].x; }
float pc98_rollback_y(Pc98Rollback *state, unsigned slot) { return state->prepared.inputs[slot].y; }
unsigned pc98_rollback_flags(Pc98Rollback *state, unsigned slot) {
    const auto &value = state->prepared.inputs[slot];
    return unsigned(value.unlimited) | (unsigned(value.touchUsed) << 1) | (unsigned(value.touchBomb) << 2);
}
unsigned pc98_rollback_predicted(Pc98Rollback *state) { return state->prepared.predictedMask; }
int pc98_rollback_commit(Pc98Rollback *state, unsigned frame) {
    if (state->preparedFrame != frame) return 0;
    return state->core.MarkSimulated(frame, state->prepared);
}
unsigned pc98_rollback_dirty(Pc98Rollback *state) { return state->core.RollbackFrame(); }
int pc98_rollback_rewind(Pc98Rollback *state, unsigned frame) { return state->core.RewindSimulationTo(frame); }
unsigned pc98_rollback_confirmed(Pc98Rollback *state, unsigned slot) { return state->core.ConfirmedThrough(static_cast<std::uint8_t>(slot)); }
int pc98_rollback_retire(Pc98Rollback *state, unsigned slot, unsigned frame) { return state->core.RetireRemoteFrom(static_cast<std::uint8_t>(slot),frame); }
void pc98_rollback_refresh(Pc98Rollback *state) { state->core.RefreshPredictions(); }
void pc98_rollback_discard(Pc98Rollback *state, unsigned frame) { state->core.DiscardSimulatedBefore(frame); }
void pc98_rollback_set_decision(Pc98Rollback *state, unsigned slot, unsigned buttons,
                                 unsigned mode, float x, float y, unsigned flags, unsigned predicted) {
    state->prepared.inputs[slot] = input(buttons,mode,x,y,flags);
    if(predicted) state->prepared.predictedMask |= static_cast<std::uint8_t>(1u<<slot);
    else state->prepared.predictedMask &= static_cast<std::uint8_t>(~(1u<<slot));
}
}
