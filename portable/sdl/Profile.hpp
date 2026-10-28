// Saves in the original formats, so they are interchangeable with the PC
// game: scoreth12.dat (rankings, clears, Practice unlocks, spell history,
// music flags) and th12.cfg (volumes, pad buttons). Port of upstream
// src/native-profile.mjs (formats reconstructed from 43cf90/43d140/43d2e0,
// 423250/42feb0) and the rules of src/game-profile.mjs.
#pragma once
#include "Platform.hpp"
#include <array>
#include <string>

namespace th12host {
// The 18 Music Room tracks in score-file order (status +0x26).
extern const std::array<const char*,18> native_music_ids;
class Profile {
public:
    // th12.cfg
    i32 bgm_volume()const{return config[0x20];}
    i32 se_volume()const{return config[0x21];}
    void set_volumes(i32 bgm,i32 se);
    const std::array<u8,60>& config_bytes()const{return config;} // recorded in replays
    // Pad buttons for shoot, bomb, focus, pause, skip (0..31, -1 = none).
    std::array<i32,5> joy_buttons()const;
    void set_joy_buttons(const std::array<i32,5>& buttons);
    // scoreth12.dat
    bool can_extra(i32 character,i32 shot)const; // no-continue clear on Normal or above
    bool any_extra()const;
    bool cleared(i32 character,i32 shot,i32 difficulty)const;
    bool can_practice(i32 character,i32 shot,i32 difficulty,i32 stage)const;
    i32 practice_score(i32 character,i32 shot,i32 difficulty,i32 stage)const; // stored (score / 10)
    bool music_heard(const std::string& id)const;
    std::string last_name()const;
    void unlock_music(const std::string& id);
    void record_play_start(i32 character,i32 shot);
    void record_play_frame(i32 character,i32 shot);
    void record_stage_clear(i32 character,i32 shot,i32 difficulty,i32 stage,i64 score);
    void record_clear(i32 character,i32 shot,i32 difficulty,i32 continues,i32 stage);
    // The core's spell history: 128 cards x 7 players x (attempts, captures).
    std::vector<i32> spell_history()const;
    void set_spell_history(const std::vector<i32>& words);
    // Score Ranking: ten 28-byte rows per player and difficulty (+0x10).
    struct RankingRow {i32 score_stored=0,final_stage=1,continues=0;std::string name;i64 timestamp=0;float slowdown=0;
        i64 score()const{return i64(score_stored)*10+continues;}};
    std::array<RankingRow,10> rankings(i32 character,i32 shot,i32 difficulty)const;
    // The rank (0..9) a score enters at, or -1; an equal score goes ahead.
    i32 ranking_place(i32 character,i32 shot,i32 difficulty,i64 score)const;
    void insert_ranking(i32 character,i32 shot,i32 difficulty,i32 rank,const RankingRow& row);
    void set_last_name(const std::string& name);
    // Player Data totals: play count, play time (60 Hz frames), clears.
    struct Totals {u32 play_count=0,play_frames=0,clear_count=0;};
    Totals totals(i32 character,i32 shot,i32 difficulty)const;
    // A card's title as the game stored it (CP932), per player record (6 = all).
    std::string spell_title(i32 card,i32 player)const;
    void set_spell_title(i32 card,i32 player,const std::string& cp932);
    bool ending_seen(i32 ending)const{return ending>=0&&ending<12&&(status[size_t(0x16+ending)]&1);}
    void record_ending_seen(i32 ending){if(ending>=0&&ending<12)status[size_t(0x16+ending)]|=1;}
    // A Practice run's score on its stage (best kept).
    void record_practice_score(i32 character,i32 shot,i32 difficulty,i32 stage,i64 score);
    // Files
    bool load(const std::string& folder);
    bool save();
    std::string error;
    // Encoded files, for the host tests.
    std::vector<u8> encode_score()const;
    bool decode_score(const std::vector<u8>& bytes);
private:
    std::string folder;std::array<u8,60> config{};
    std::array<std::vector<u8>,7> records;std::vector<u8> status;std::array<u8,24> header{};
    bool score_read_only=false,config_read_only=false;
    void fresh();
    u8* practice_entry(i32 player,i32 difficulty,i32 stage);
    const u8* practice_entry(i32 player,i32 difficulty,i32 stage)const;
};
}
