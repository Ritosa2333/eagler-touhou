#pragma once
#include <array>
#include <cstdint>
#include <optional>
#include <vector>
#include <stdexcept>
namespace soew {
// ReC98/th02/main/player/player.cpp:33..39,64..112,125..245.
// Source has two authoritative page positions AND a separately cached position.
// Keep both: miss animation changes pages AFTER refreshing the cached position.
struct PlayerLife {
    struct Point {int x=208,y=336;};
    std::array<Point,2> positions{};
    Point current,draw;
    std::optional<unsigned> pattern;
    std::uint8_t invincibility=0,miss_frame=0,lives=2,bombs=3,start_bombs=3,power=1;
    bool hit=false,miss_active=false,bomb_invincible=false,game_over=false;
    int playperf=0;
    int respawn_x=208;
    static int playperf_max(unsigned difficulty){return difficulty==0?4:16;}
    void advance_playperf(std::uint32_t stage_frame,unsigned difficulty){if(stage_frame%1800==1500&&playperf<playperf_max(difficulty))++playperf;}
    unsigned stage_misses=0,total_misses=0;
    enum EffectKind {Sound,ScoreCommit,Sparks,MissItems,LivesChanged,BombsChanged,PowerChanged};
    struct Effect {EffectKind kind;int x=0,y=0,value=0,count=0;bool gameover=false,as_sprite=false;};
    std::vector<Effect> effects;
    explicit PlayerLife(unsigned spare=2,unsigned stock=3){if(spare>255||stock>253)throw std::runtime_error("Invalid life stock");lives=static_cast<std::uint8_t>(spare);bombs=start_bombs=static_cast<std::uint8_t>(stock);}
    void negate(){if(bomb_invincible||(invincibility&&!miss_active))hit=false;}
    void begin_frame(unsigned back){if(back>1)throw std::runtime_error("Invalid life page");positions[back]=positions[back^1];pattern.reset();}
    bool input_allowed(){negate();return !hit&&!game_over;}
    void moved(unsigned back,int x,int y){positions[back]={x,y};current={x,y};}
    void register_hit(){if(!game_over)hit=true;}
    // Called before shots_update, matching original negate/decrement order.
    void player_phase_begin(){negate();if(invincibility)--invincibility;}
    void sparks(unsigned back,int count){effects.push_back({Sparks,positions[back].x+16,positions[back].y+24,128,count,false,true});}
    void player_phase_end(unsigned back,unsigned normal_pattern,bool infinite_lives=false,bool keep_power=false,bool lock_rank=false,bool protect_last=false){
        if(game_over)return;
        if(!hit){if((invincibility&3)<3){draw=current;pattern=normal_pattern;}return;}
        if(miss_frame==0){effects.push_back({Sound,0,0,2});miss_active=true;++stage_misses;++total_misses;effects.push_back({ScoreCommit});sparks(back,16);}
        current=positions[back];++miss_frame;
        if(miss_frame<24){draw=positions[back];pattern=4+(miss_frame>>2);}
        else if(miss_frame==24){
            invincibility=220;if(!lock_rank){playperf=playperf>2?0:playperf-2;if(playperf<-6)playperf=-6;}
            sparks(back,32);
            if(lives==0&&!infinite_lives){miss_frame=0;effects.push_back({MissItems,positions[back].x,positions[back].y,0,0,true});game_over=true;hit=false;}
            else{if(!keep_power){power=1;effects.push_back({PowerChanged,0,0,1});}effects.push_back({MissItems,positions[back].x,positions[back].y,0,0,false});}
        }else if(miss_frame<43)positions[back].y+=4;
        else if(miss_frame==43){
            if(!infinite_lives||(protect_last&&lives>0)){--lives;effects.push_back({LivesChanged,0,0,lives});}bombs=static_cast<std::uint8_t>(start_bombs+(lives==0?2:0));effects.push_back({BombsChanged,0,0,bombs});sparks(back,16);
            positions[0]=positions[1]={respawn_x,366};
        }else if(miss_frame<68){positions[back].y-=2;draw=positions[back];pattern=0;}
        else{miss_frame=0;miss_active=false;hit=false;}
    }
};
}
