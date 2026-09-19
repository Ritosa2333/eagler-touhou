#pragma once
#include "ScoreFile.hpp"
namespace th08 {
struct GameGlobals {
    u32 display_score=0;
    i32 graze_stage=0;
    u32 score=0;
    i32 graze=0,score_increment=0;
    u32 high_score=0;
    u8 high_score_retries=0,padding19[3]{};
    i32 captured_spells=0;
    i16 gauge_copy=0,gauge=0;
    i32 point_value=0;
    i8 clock_time=0;
    u8 retries=0,padding2a[2]{};
    i32 points_stage=0,points=0;
    u32 point_extends=0;
    i32 next_point_extend=0,time_orbs=0,last_spell_requirement=0,total_time_orbs=0;
    i32 rng1[7]{};
    float deaths=0,deaths_stage=0,rng2[2]{},lives=0,rng3[2]{},bombs=0,bombs_used=0,bombs_used_stage=0,rng4[3]{},power=0,rng5[2]{};
    i32 rng6=0,rng7[8]{};
    u32 integrity_value=0;
    i32 integrity_checksum=0,rng8[5]{};
};
static_assert(sizeof(GameGlobals)==0xe4 && offsetof(GameGlobals,lives)==0x74 && offsetof(GameGlobals,integrity_value)==0xc8);
// Native game values and their original integrity/random-stream side effects.
// There is no emulated memory, Windows state or instruction dispatch here.
class GameValues {
public:
    GameValues(GameGlobals& globals,GameConfiguration& game_config,GameConfiguration& display_config,HighScore& high_score,Rng& random)
      :globals(globals),game_config(game_config),display_config(display_config),high_score(high_score),random(random){}
    float expected_checksum=0;
    void initialize_integrity();
    void randomize_integrity();
    void update_integrity();
    void refresh_integrity(){store_checksum();}
    bool tampered()const;
    i32 checksum();
    i32 checksum_bytes(const u8* data,i32 size);
    void set_lives(i32 value);
    void set_bombs(i32 value);
    void set_power(i32 value);
    void set_deaths_stage(i32 value);
    void set_bombs_stage(i32 value);
    bool add_lives(i32 value);
    bool add_bombs(i32 value);
    bool add_power(i32 value);
    bool add_deaths(i32 value);
    bool count_bombs(i32 value);
    void add_time_orbs(i32 value);
    void add_score(i32 value){globals.score+=u32(value/10);}
    void add_clock(i8 amount){globals.clock_time=i8(u8(globals.clock_time)+u8(amount));}
private:
    GameGlobals& globals;
    GameConfiguration &game_config,&display_config;
    HighScore& high_score;
    Rng& random;
    i32 random_integer();
    float random_float();
    void store_checksum(bool separate_casts=false);
};
struct SpellMusic {i32 last_spell,song;const char* path;i32 name_sprite;bool pause_in_practice;};
const SpellMusic& spell_music(i32 current_spell)noexcept;
}
