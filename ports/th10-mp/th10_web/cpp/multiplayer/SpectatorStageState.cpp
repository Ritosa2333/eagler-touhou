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

template<class Codec> bool team(Codec& codec, TeamEconomy& value) {
    return codec.Values(value.high_score, value.score, value.item_value, value.enemy_activity,
        value.faith_timer.previous, value.faith_timer.current, value.faith_timer.fractional,
        value.faith_timer_flags, value.difficulty, value.reserved_038, value.stage,
        value.reserved_040, value.section, value.stage_frames, value.section_frames,
        value.score_units, value.high_score_units, value.rank, value.extend_index, value.flags);
}

template<class Codec> bool pilot_economy(Codec& codec, PilotEconomy& value) {
    return codec.Values(value.power, value.reserved_power, value.character, value.shot_type,
                        value.lives, value.challenge_deaths);
}

template<class Codec> bool frame_input(Codec& codec, Netplay::FrameInput& value) {
    return codec.Values(value.buttons, value.analogMode, value.x, value.y,
                        value.unlimited, value.touchUsed, value.touchBomb);
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
    if(!valid(cp)||!ValidateSpectatorStageScene(value.scene)||
       value.scene.present!=value.activation.present||
       (cp.firstFrame!=0&&(!value.scene.present||!value.activation.present))||
       (cp.firstFrame==0&&(value.scene.present||value.activation.present))||
       !setup.configured||!setup.started||!setup.sessionId||
       setup.playerCount!=cp.cooperation.seatCount||setup.localPlayer>=setup.playerCount||
       setup.difficulty>4||setup.seed>65535||setup.input_delay>9||setup.adonis_mode>2||
       setup.prediction_reserve>12||setup.measured_prediction>2||
       value.team.stage!=i32(cp.label&255u)||value.team.difficulty!=i32(setup.difficulty))return false;
    for(const auto& seat:value.optionSnapNext)for(const auto snap:seat)if(snap>1)return false;
    for(std::size_t i=0;i<setup.playerCount;++i){
        const auto& pilot=value.pilots[i];const auto& coop=cp.cooperation.seats[i];
        if(pilot.character!=setup.loadouts[i].character||pilot.shot_type!=setup.loadouts[i].shot||
           pilot.lives!=coop.lives||pilot.power!=coop.power||
           setup.loadouts[i].character>1||setup.loadouts[i].shot>2||
           !Netplay::IsValidFrameInput(value.inputs[i]))return false;
    }
    for(std::size_t i=setup.playerCount;i<Netplay::MAX_PLAYERS;++i)
        if(!Netplay::IsValidFrameInput(value.inputs[i]))return false;
    return true;
}
}

bool EncodeSpectatorStageState(const SpectatorStageState& checkpointValue,
                               std::vector<std::uint8_t>& output) {
    if (!valid(checkpointValue)) return false;
    Writer writer;
    auto copy = checkpointValue;
    if (!writer.Value(kSpectatorStageSchema) || !checkpoint(writer, copy.checkpoint)) return false;
    for(auto& seat:copy.optionSnapNext)for(auto& snap:seat)if(!writer.Value(snap))return false;
    if(!team(writer,copy.team))return false;
    for(auto& pilot:copy.pilots)if(!pilot_economy(writer,pilot))return false;
    for(auto& input:copy.inputs)if(!frame_input(writer,input))return false;
    if(!input(writer,copy.hostInput)||!writer.Value(copy.pauseRising)||!route(writer,copy.route)||
       !WriteSpectatorStageScene(writer,copy.scene)||!writer.Good())return false;
    if(!WriteSpectatorStageActivation(writer,copy.activation)||!writer.Good())return false;
    output = writer.Bytes();
    return !output.empty() && output.size() <= Netplay::SpectatorStageMaxStateBytes;
}

bool DecodeSpectatorStageState(const std::vector<std::uint8_t>& bytes,
                               SpectatorStageState& output) {
    if (bytes.empty() || bytes.size() > Netplay::SpectatorStageMaxStateBytes) return false;
    Reader reader(bytes);
    SpectatorStageState decoded{};
    std::uint32_t schema = 0;
    if (!reader.Value(schema) || schema != kSpectatorStageSchema ||
        !checkpoint(reader, decoded.checkpoint)) return false;
    for(auto& seat:decoded.optionSnapNext)for(auto& snap:seat)if(!reader.Value(snap))return false;
    if(!team(reader,decoded.team))return false;
    for(auto& pilot:decoded.pilots)if(!pilot_economy(reader,pilot))return false;
    for(auto& input:decoded.inputs)if(!frame_input(reader,input))return false;
    if(!input(reader,decoded.hostInput)||!reader.Value(decoded.pauseRising)||
       !route(reader,decoded.route)||!ReadSpectatorStageScene(reader,decoded.scene)||
       !ReadSpectatorStageActivation(reader,decoded.activation)||
       !reader.Done()||!valid(decoded))return false;
    output = decoded;
    return true;
}

} // namespace th10::multiplayer
