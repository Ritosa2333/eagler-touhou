#include "SpectatorStageState.hpp"
#include "CooperativeRules.hpp"

#include <algorithm>

namespace th10::multiplayer {
namespace {
using Netplay::SpectatorScalars::Reader;
using Netplay::SpectatorScalars::Writer;

template<class Codec> bool rng(Codec& codec, Rng& value) {
    return codec.Values(value.seed, value.reserved, value.calls);
}

template<class Codec> bool input(Codec& codec, GameInput& value) {
    return codec.Values(value.raw, value.raw_previous, value.raw_repeat,
        value.raw_pressed, value.raw_released) && codec.Array(value.raw_held_frames) &&
        codec.Values(value.focus_hold, value.current, value.previous, value.repeat,
            value.pressed, value.released, value.reserved_036) && codec.Array(value.held_frames);
}

template<class Codec> bool point(Codec& codec, PlayerOption::FixedPoint& value) {
    return codec.Values(value.x, value.y);
}

template<class Codec> bool route(Codec& codec, SessionSetup& value) {
    if (!codec.Values(value.sessionId, value.playerCount, value.localPlayer, value.difficulty,
            value.seed, value.input_delay, value.version, value.adonis_mode,
            value.prediction_reserve, value.measured_prediction, value.input_delay_auto,
            value.challenge_mode)) return false;
    for (auto& word : value.build) if (!codec.Value(word)) return false;
    for (auto& loadout : value.loadouts)
        if (!codec.Values(loadout.character, loadout.shot)) return false;
    return codec.Values(value.configured, value.started);
}

template<class Codec> bool replay_stage(Codec& codec, ReplayStage& value) {
    if (!codec.Values(value.stage, value.seed, value.frames, value.stream_bytes, value.score,
            value.power, value.reserved_012, value.item_value, value.faith, value.lives, value.rank) ||
        !point(codec, value.position)) return false;
    for (auto& item : value.history) if (!point(codec, item)) return false;
    for (auto* list : {value.option_targets, value.option_positions, value.option_offsets,
                       value.option_focused_offsets})
        for (unsigned i = 0; i < 4; ++i) if (!point(codec, list[i])) return false;
    return codec.Values(value.score_units, value.extend_index, value.focused, value.flags);
}

template<class Codec> bool checkpoint(Codec& codec, ReplayCheckpoint& value) {
    if (!codec.Values(value.label, value.firstFrame) ||
        !rng(codec, value.scriptRandom) || !rng(codec, value.visualRandom) ||
        !rng(codec, value.activationScriptRandom) || !rng(codec, value.activationVisualRandom) ||
        !codec.Values(value.activationRandomValid, value.faithCursor, value.laserLastId,
                      value.reservedStage, value.retainedStateValid)) return false;
    for (auto& seat : value.inputSeats) if (!input(codec, seat)) return false;
    auto& coop = value.cooperation;
    if (!codec.Values(coop.seatCount, coop.retryPending, coop.wipeTicks)) return false;
    for (auto& seat : coop.seats) {
        if (!codec.Values(seat.lives, seat.power, seat.character, seat.shot, seat.lifeState,
                          seat.rescueTicks, seat.rescueTarget, seat.waitingForFocusRelease)) return false;
    }
    for (auto& pilot : value.pilots) if (!replay_stage(codec, pilot)) return false;
    for (auto& power : value.reservedPower) if (!codec.Value(power)) return false;
    for (auto& deaths : value.challengeDeaths) if (!codec.Value(deaths)) return false;
    return codec.Value(value.cheatMovementUsed);
}

bool valid(const ReplayCheckpoint& value) {
    const auto stage = value.label & 255u;
    if (stage < 1 || stage > 7 || value.firstFrame == Netplay::INVALID_FRAME ||
        value.cooperation.seatCount < 2 || value.cooperation.seatCount > Netplay::MAX_PLAYERS ||
        value.cooperation.retryPending > 1 || value.cooperation.wipeTicks != 0 ||
        !value.retainedStateValid || value.faithCursor >= 2048) return false;
    for (std::size_t i = 0; i < value.cooperation.seatCount; ++i) {
        const auto& seat = value.cooperation.seats[i];
        if (seat.lives < -1 || seat.lives > kMaxLives || seat.power < 0 || seat.power > kMaxPower ||
            seat.character > 1 || seat.shot > 2 || seat.lifeState > u8(LifeState::Eliminated) ||
            seat.rescueTarget < -1 || seat.rescueTarget >= std::int8_t(value.cooperation.seatCount) ||
            seat.waitingForFocusRelease > 1) return false;
        const auto& pilot = value.pilots[i];
        if (pilot.stage != std::int16_t(stage) || pilot.power < 0 || pilot.power > kMaxPower ||
            pilot.lives < -1 || pilot.lives > kMaxLives || pilot.score_units < 0 || pilot.score_units > 9 ||
            value.reservedPower[i] > kMaxPower || (pilot.focused != 0 && pilot.focused != 1)) return false;
    }
    return true;
}

bool valid(const SpectatorStageState& value) {
    const auto& cp=value.checkpoint;const auto& setup=value.route;
    if(!valid(cp)||!setup.configured||!setup.started||!setup.sessionId||
       setup.playerCount!=cp.cooperation.seatCount||setup.localPlayer>=setup.playerCount||
       setup.difficulty>4||setup.seed>65535||setup.input_delay>9||setup.adonis_mode>2)return false;
    for(unsigned i=0;i<setup.playerCount;++i){
        if(setup.loadouts[i].character>1||setup.loadouts[i].shot>2||
           cp.cooperation.seats[i].character!=setup.loadouts[i].character||
           cp.cooperation.seats[i].shot!=setup.loadouts[i].shot)return false;
    }
    return true;
}
}
bool EncodeSpectatorStageState(const SpectatorStageState& state,std::vector<u8>& output){
    if(!valid(state))return false;auto copy=state;Writer writer;
    if(!writer.Value(kSpectatorStageSchema)||!checkpoint(writer,copy.checkpoint)||
       !route(writer,copy.route)||!writer.Good())return false;
    output=writer.Bytes();return true;
}
bool DecodeSpectatorStageState(const std::vector<u8>& bytes,SpectatorStageState& output){
    if(bytes.empty()||bytes.size()>Netplay::SpectatorStageMaxStateBytes)return false;
    Reader reader(bytes);SpectatorStageState decoded;u32 schema=0;
    if(!reader.Value(schema)||schema!=kSpectatorStageSchema||!checkpoint(reader,decoded.checkpoint)||
       !route(reader,decoded.route)||!reader.Done()||!valid(decoded))return false;
    output=decoded;return true;

}

} // namespace th10::multiplayer
