#pragma once
#include "Replay.hpp"
namespace th11 {
struct ReplayMenuEntry {
    std::string path;Replay replay;std::vector<u8> file;
#ifdef TH11_MULTIPLAYER
    // Presentation metadata for an all-seat archive. Keep this separate from
    // retail Replay bytes: an MP archive has no retail stage-score snapshots.
    struct MultiplayerPreview {
        std::string name;
        u64 timestamp=0;
        u32 score=0,selection=0,difficulty=0,player_count=0,recorded_player=0,last_stage=0;
        bool completed=false;
        std::array<u32,8> stage_frames{};
        std::array<bool,8> stages{};
    } multiplayer;
    bool is_multiplayer=false;
#endif
    bool has_stage(u32 number)const {
#ifdef TH11_MULTIPLAYER
        if(is_multiplayer)return number<multiplayer.stages.size()&&multiplayer.stages[number];
#endif
        return replay.stage(number)!=nullptr;
    }
    u32 character()const {
#ifdef TH11_MULTIPLAYER
        if(is_multiplayer)return multiplayer.selection/3;
#endif
        return replay.character();
    }
    u32 subtype()const {
#ifdef TH11_MULTIPLAYER
        if(is_multiplayer)return multiplayer.selection%3;
#endif
        return replay.subtype();
    }
    u32 difficulty()const {
#ifdef TH11_MULTIPLAYER
        if(is_multiplayer)return multiplayer.difficulty;
#endif
        return replay.difficulty();
    }
};
}
