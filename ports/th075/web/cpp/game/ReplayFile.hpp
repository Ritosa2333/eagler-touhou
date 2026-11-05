#pragma once
#include "BattleGeometry.hpp"
namespace th075 {
struct ReplayRound {
    std::array<u8,6> opponentOptions{};u8 stage=0;i16 initialHealth=0;u8 initialLives=0;u32 initialScore=0;
    std::vector<u8> playerInputs;std::vector<i16> randomValues;std::vector<u8> auxiliaryInputs;std::vector<u32> events;
};
class ReplayFile {
public:
    // Preserve the original name/date/configuration header, including reserved bytes.
    std::array<u8,60> header{};std::vector<ReplayRound> rounds;std::string error;
    bool load(const void*,std::size_t);
    bool encode(std::vector<u8>&)const;
};
enum class ReplayMode:u8 {Live,Recording,Playback};
struct ReplayStreams {
    ReplayMode mode=ReplayMode::Live;ReplayRound* round=nullptr;ReplayRandom random;
    u32 inputCursor=0,randomCursor=0,auxiliaryCursor=0,eventCursor=0;bool exhausted=false;
    void bind(ReplayMode,ReplayRound*,u32 randomSeed);
    u8 input(u8 live);u8 auxiliary(u8 live);i32 range(i32 low,i32 high);
    i16 control_timer(i16 live);
    void append_event(u32 frame,u32 event);
};
}
