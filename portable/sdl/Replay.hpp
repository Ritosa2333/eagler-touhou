// TH12 1.00b replays (.rpy), the original format: port of upstream
// src/replay.mjs (reading, native 43c424 / 4639d0) and replay-recorder.mjs
// (writing, 43ae80 / 43c590 / 43b7e0 / 43bc10). Files are interchangeable
// with the PC game's replay/ folder.
#pragma once
#include "Platform.hpp"
#include <array>
#include <optional>
#include <string>

namespace th12host {
// The 17 words a stage starts from (th12_start_replay's order is x, y, power,
// lives, bombs, score, point value, life/bomb fragments, UFO colours, rank,
// native globals).
struct ReplayInitial {
    i32 score=0,power=0,point_value=0,lives=0,life_fragments=0,bombs=0,bomb_fragments=0,rank=0,x=0,y=0;
    std::array<i32,3> ufo{};std::array<i32,4> globals{};
    std::array<i32,17> words()const{return {x,y,power,lives,bombs,score,point_value,life_fragments,bomb_fragments,ufo[0],ufo[1],ufo[2],rank,globals[0],globals[1],globals[2],globals[3]};}
};
struct ReplayInput {u16 held=0,pressed=0,released=0;bool end()const{return held==0xffff&&pressed==0xffff&&released==0xffff;}};
struct ReplayStage {
    i32 number=1,seed=0;u32 frames=0,payload=0,input_offset=0,fps_offset=0,fps_count=0;
    ReplayInitial initial;std::array<i32,21> spell_times{};
};
// Header fields, enough for the Replay and save lists.
struct ReplayInfo {
    std::string name;i64 timestamp=0;i32 score_stored=0;float slowdown=0;
    i32 character=0,shot=0,difficulty=0,final_stage=1,continues=0;bool practice=false;
};
struct Replay {
    ReplayInfo info;std::vector<ReplayStage> stages;std::vector<u8> data;
    const ReplayStage* stage(i32 number)const{for(const auto& s:stages)if(s.number==number)return &s;return nullptr;}
    const ReplayStage* after(i32 number)const{const ReplayStage* best=nullptr;for(const auto& s:stages)if(s.number>number&&(!best||s.number<best->number))best=&s;return best;}
    // nullopt past the stage's last record (an ended stream).
    std::optional<ReplayInput> input(i32 stage,u32 frame)const;
};
bool parse_replay(const std::vector<u8>& file,Replay& out,std::string& error);
// Only the 112-byte header (the list pages decode no further).
bool read_replay_info(const std::vector<u8>& file,ReplayInfo& out);

// 20 marker words and the fullscreen flag (native 43b080).
std::array<i32,21> default_replay_spell_times();
// th12.cfg's 60 bytes, recorded in every replay header.
using ReplayConfig=std::array<u8,60>;

class ReplayRecorder {
public:
    ReplayRecorder(i32 character,i32 shot,i32 difficulty,bool practice,const ReplayConfig& config)
        :character(character),shot(shot),difficulty(difficulty),practice(practice),config(config){}
    void begin_stage(i32 number,i32 seed,const ReplayInitial& initial);
    void push(u32 held,u32 pressed,u32 released,float fps);
    void set_spell_time(i32 index,i32 encoded){if(active&&index>=0&&index<20)active->spell_times[size_t(index)]=encoded;}
    void finish_stage(bool end=false);
    bool recording()const{return bool(active);}
    // The finished file; the last open stage is closed with an end marker.
    std::vector<u8> encode(const std::string& name,i64 timestamp,i32 score_stored,i32 continues,i32 final_stage,float slowdown);
private:
    struct Stage {i32 number=1,seed=0;ReplayInitial initial;std::array<i32,21> spell_times{};std::vector<ReplayInput> inputs;std::vector<u8> fps;};
    i32 character,shot,difficulty;bool practice;ReplayConfig config;
    std::vector<Stage> stages;std::optional<Stage> active;
};

// The replay folder: th12_01..25.rpy (fixed slots) and th12_ud????.rpy
// (user files) after them, as the original lists them.
struct ReplayEntry {std::string file;std::optional<ReplayInfo> info;};
std::vector<ReplayEntry> list_replays(const std::string& folder);
}
