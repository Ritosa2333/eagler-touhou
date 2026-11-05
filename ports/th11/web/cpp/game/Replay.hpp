#pragma once
#include "Types.hpp"
#include <array>
#include <string>
#include <vector>

namespace th11 {
struct GameEconomy;struct Rng;struct PlayerMotionState;
struct ReplayStageState {
    u32 seed=0;i32 score=0,power=0,points=0,lives=0,fragments=0,rank=0;
    i32 x=0,y=0,continues=0,extends=0,focused=0,graze=0,weapon=0;
    std::array<u32,20> spell_times{};
    void read(const u8* header);
    void restore(GameEconomy&,Rng&,bool first)const;
    void restore_position(PlayerMotionState&)const;
};
// TH11 1.00a t11r v4. Entries are six bytes PER TICK, not timestamped
// change records: held, pressed and released keys, each a u16. Playback
// restores the stored edges; it must not derive them from the previous tick.
struct ReplayInput {
    u32 held=0,previous=0,pressed=0,released=0;
    u32 fps=0;bool end_marker=false;
    i32 touch_mode=0;float touch_x=0,touch_y=0;
};
struct ReplayTouch {u32 frame=0;i32 mode=0;float x=0,y=0;};
struct ReplayStage {
    u32 number=0,offset=0,frames=0,payload_size=0;
};
class Replay {
public:
    bool open(const u8*,u32);
    bool select(u32 stage);
    ReplayInput tick(bool active=true);
    const ReplayStage* stage(u32 number) const;
    const u8* header(u32 number) const;
    const std::vector<u8>& decoded() const {return bytes;}
    u32 stage_count() const {return count;}
    u32 character() const;u32 subtype() const;u32 difficulty() const;
    u32 frame() const {return cursor;}
    u32 selected_stage()const{return selected;}
    const std::string& error() const {return failure;}
    bool uses_touch()const;
    void retain_metadata();
private:
    std::vector<u8> bytes;std::array<ReplayStage,8> stages{};
    std::array<std::vector<ReplayTouch>,8> touches{};u32 touch_cursor=0;
    u32 count=0,selected=0,cursor=0;ReplayInput input;std::string failure;
    bool metadata_only=false;
};
}
