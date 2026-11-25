#pragma once
#include "enemy_math.hpp"
#include "core_contract.hpp"
#include <array>
#include <cstdint>
#include <vector>
#include <functional>
namespace soew {
// A/B/C level0..9 source branches + option projectiles, source asm
// sub_F1D8:5676..5784,shot_a/b/c:2681..3928,sub_D38F,shots_update.
// Boss target contexts and original startup state remain caller obligations.
// No Bomb. Portable slots explicitly retain original narrow-field behavior.
struct PlayerShots {
    struct Slot {
        std::uint8_t flag=0,type=0,pattern=52,special=0,age=0,render_pattern=52;
        std::array<std::array<std::uint16_t,2>,2> position{};
        int vx=0,vy=0;
        unsigned motion_generation=0;
    };
    using BoundaryExit=std::function<void(const Slot&,const std::array<std::uint16_t,2>&)>;
    std::array<Slot,38> slots{};
    std::uint8_t burst_a=255,burst_b=255,cool_a=0,cool_b=0;
    struct HitSpark {int left=0,top=0;unsigned speed=30,count=1;bool as_sprite=false;};
    std::vector<HitSpark> hit_sparks;
    std::vector<std::uint16_t> hit_score_lowword_adds;
    std::uint64_t fired=0,hit_events=0,damage_events=0,sound_requests=0;
    static int pixel(std::uint16_t raw){const int v=enemy_s16(raw);return v>=0?v/16:-((-v+15)/16);}
    static int sar3(int v){return v>=0?v/8:-((-v+7)/8);}
    void begin_frame(unsigned back){
        if(back>1)throw std::runtime_error("Invalid shot page");
        unsigned active_options=0;
        for(auto &s:slots){if(s.flag==2){s.flag=0;continue;}if(s.flag==1){s.position[back]=s.position[back^1];if(s.special)++active_options;}}
        if(!active_options)option_lock=0;
    }
    unsigned level=0,shot_type=0;
    RandomRing *random=nullptr;
    std::uint8_t spread=0,option_lock=0,counter_c=0;
    int option_left=192,option_top=352,target_x=-1,target_y=-1,last_target_x=-1;
    bool supports_level()const{return shot_type<=2&&level<=9;}
    unsigned draw_random(unsigned mask){if(!random)throw std::runtime_error("Missing shared shot random ring");return random->masked(static_cast<std::uint8_t>(mask));}
    bool emit(unsigned back,int player_x,int player_y){
        if(back>1||shot_type>2||!supports_level())throw std::runtime_error("Weapon level not yet implemented");
        if(shot_type==2)++counter_c;
        if(shot_type!=0&&level>=2)return emit_bc(back,player_x,player_y);
        // shot_a's slot search increments group index ONLY after allocation;
        // every random angle is drawn after finding a free slot, not before.
        unsigned index=0;bool allocated=false;
        constexpr std::array<unsigned,10> counts={1,1,2,2,3,3,4,4,5,6};
        for(auto &s:slots){if(s.flag)continue;
            unsigned angle=192,pattern=level==9?48:52;int offset=8;
            switch(level){
            case 0:break;
            case 1:if(shot_type!=2)angle=190+draw_random(3);break;
            case 2:offset=index==0?0:16;angle=188+draw_random(7);break;
            case 3:offset=index==0?0:16;break;
            case 4:case 5:angle=index==0?192:index==1?(level==4?187:185):(level==4?197:199);break;
            case 6:if(index<2)offset=index==0?0:16;else angle=177+draw_random(31);break;
            case 7:if(index<2)offset=index==0?0:16;else angle=index==2?184:200;break;
            case 8:{constexpr std::array<unsigned,5> angles={192,184,200,176,208};angle=angles[index];break;}
            case 9:
                if(index<2)offset=index==0?0:16;
                else if(index==2){pattern=52;angle=179;}
                else if(index==3){pattern=52;angle=205;}
                else {pattern=54;offset=index==4?-24:40;angle=index==4?unsigned(192-int(spread)):192+spread;}
                break;
            }
            s.motion_generation=unsigned(++fired);s.flag=1;s.type=0;s.pattern=static_cast<std::uint8_t>(pattern);s.special=0;
            s.position[back]={static_cast<std::uint16_t>((player_x+offset)*16),static_cast<std::uint16_t>((player_y+32)*16)};
            const auto velocity=enemy_vector(angle,192);s.vx=velocity[0];s.vy=velocity[1];allocated=true;
            if(++index>=counts[level])break;
        }
        return allocated;
    }
    bool emit_bc(unsigned back,int px,int py){
        unsigned slot=0,index=0;bool any=false;
        const unsigned main_count=shot_type==1?(level==2?1:level<=5?2:level<=7?3:4):(level==9&&counter_c%3==0?5:1);
        const unsigned main_pattern=shot_type==2?124:52;
        for(;slot<38;++slot){auto &s=slots[slot];if(s.flag)continue;unsigned angle=192,pattern=main_pattern;int offset=8,y=py+32;
            if(shot_type==1){
                if(level==2)angle=188+draw_random(7);
                else if(level==3){offset=index?16:0;angle=188+draw_random(7);}
                else if(level==4)offset=index?16:0;
                else if(level==5){offset=index?16:0;angle=index?194:190;}
                else if(level==6||level==7)angle=index==0?192:index==1?(level==6?187:185):(level==6?197:199);
                else if(index<2)offset=index?16:0;
                else if(level==8)angle=177+draw_random(31);
                else {pattern=50;offset=index==2?0:16;if(target_y>0)angle=(enemy_atan2(enemy_s16(unsigned(target_y-py-32)),enemy_s16(unsigned(target_x-px-(index==3?16:0))))+draw_random(7)-3)&255;else angle=index==2?183:201;}
            }else if(level>=3&&level<=5)angle=190+draw_random(3);
            else if(level>=6&&level<=8)angle=188+draw_random(7);
            else if(level==9&&index){pattern=49;offset=(index==1||index==3)?-16:32;if(index>=3)y-=16;}
            s.motion_generation=unsigned(++fired);s.flag=1;s.type=0;s.special=0;s.pattern=static_cast<std::uint8_t>(pattern);s.position[back]={static_cast<std::uint16_t>((px+offset)*16),static_cast<std::uint16_t>(y*16)};auto v=enemy_vector(angle,192);s.vx=v[0];s.vy=v[1];any=true;
            if(++index>=main_count){++slot;break;}
        }
        if(index<main_count||level<2)return any;
        bool has_option_slot=false;for(unsigned i=slot;i<38;++i)if(slots[i].flag==0){has_option_slot=true;break;}
        if(!has_option_slot)return any;
        if(shot_type==1){last_target_x=level==9?target_x:-1;if(option_lock)return any;}
        else {const bool due=level==2?!option_lock:level==3?counter_c%5==0:level<=5?(counter_c&3)==0:level<=8?counter_c%3==0:(counter_c&1)==0;if(!due)return any;}
        struct Spec{int x,vx,vy;};
        std::array<Spec,6> spec{};unsigned count=0;
        if(shot_type==1){
            if(level<=4){count=2;spec={Spec{0,0,-4},Spec{48,0,-4}};}
            else if(level<=7){count=4;spec={Spec{0,0,-16},Spec{48,0,-16},Spec{-8,-3,-6},Spec{56,3,-6}};}
            else if(level==8){count=6;spec={Spec{0,0,-34},Spec{48,0,-34},Spec{-8,-5,-28},Spec{56,5,-28},Spec{-16,-10,-24},Spec{64,10,-24}};}
            else {count=6;spec={Spec{0,0,-54},Spec{48,0,-54},Spec{-8,-16,-50},Spec{56,16,-50},Spec{-16,-32,-40},Spec{64,32,-40}};}
        }else{
            if(level<=3){count=2;spec={Spec{0,0,-4},Spec{48,0,-4}};}
            else if(level==4){count=2;spec={Spec{0,0,-16},Spec{48,0,-16}};}
            else if(level<=7){count=4;const int dx=level==5?4:6,vy=level==5?-16:level==6?-24:-26;spec={Spec{0,dx,vy},Spec{48,-dx,vy},Spec{-8,0,vy},Spec{56,0,vy}};}
            else if(level==8){count=4;spec={Spec{0,10,-48},Spec{48,-10,-48},Spec{-8,-4,-48},Spec{56,4,-48}};}
            else {count=(counter_c&3)==0?6:4;spec={Spec{0,-10,-160},Spec{48,10,-160},Spec{-8,20,-140},Spec{56,-20,-140},Spec{-16,-7,-120},Spec{64,7,-120}};}
        }
        index=0;
        for(;slot<38;++slot){auto &s=slots[slot];if(s.flag)continue;auto a=spec[index];
            // Level8 B's source CF83 block falls directly through CFD6 and
            // overwrites the same slot; preserve final state, not both shots.
            if(shot_type==1&&level==8){constexpr std::array<Spec,6> overwritten={Spec{0,0,-54},Spec{48,0,-54},Spec{-8,-16,-50},Spec{56,16,-50},Spec{-16,-32,-40},Spec{64,32,-40}};a=overwritten[index];}
            s.motion_generation=unsigned(++fired);s.flag=1;s.type=1;s.special=1;s.pattern=static_cast<std::uint8_t>(level==9?(shot_type==1?55:59):63);s.position[back]={static_cast<std::uint16_t>((option_left+a.x)*16),static_cast<std::uint16_t>(option_top*16)};s.vx=a.vx;s.vy=a.vy;any=true;
            if(++index>=count){option_lock=1;break;}
        }
        return any;
    }
    // Byte comparisons below are unsigned, including the -1/255 sentinels.
    void input(bool held,unsigned back,int player_x,int player_y,std::uint16_t directions=0){
        if(held){if(burst_a==255){burst_a=0;cool_a=0;}else if(burst_b==255){burst_b=0;cool_b=0;}}
        else if(burst_a>=3){burst_a=255;burst_b=4;}else if(burst_b>=2)burst_b=255;
        if(burst_a!=255){if(cool_a==0){emit(back,player_x,player_y);++sound_requests;cool_a=8;++burst_a;return;}--cool_a;}
        if(burst_b<2){if(cool_b==0){if(level==9){if(directions&(Down|DownLeft|DownRight))spread=static_cast<std::uint8_t>(spread+5)>26?26:static_cast<std::uint8_t>(spread+5);else if(!(directions&(15|0xf000)))spread=spread<5?0:static_cast<std::uint8_t>(spread-5);}emit(back,player_x,player_y);cool_b=8;++burst_b;return;}++sound_requests;--cool_b;}
    }
    // Enemy hit query runs BEFORE this frame's shot movement and only scans
    // slots0..34, despite38 slots being updated/rendered in original.
    unsigned damage(unsigned back,int left,int top,int width=32,int height=32,const std::function<void(const HitSpark&)> &spark={}){
        if(level>9)throw std::runtime_error("Invalid damage level");
        constexpr std::array<unsigned,10> main_bonus={5,5,4,4,3,3,3,2,2,1},large_bonus={0,0,8,8,7,7,6,5,4,3},option_damage={2,2,2,2,2,1,1,1,1,1};
        unsigned total=0;
        for(unsigned i=0;i<35;++i){auto &s=slots[i];if(s.flag!=1||(!s.special&&s.type!=0))continue;
            const int x=pixel(s.position[back][0]),y=pixel(s.position[back][1]);
            if(x<=left-8||x>=left+width-(s.special?8:0)||y<=top-8||y>=top-8+height)continue;
            ++hit_events;
            if(s.special){total+=option_damage[level];if(s.type==1){s.type=2;hit_sparks.push_back({x,y,40,1,false});if(spark)spark(hit_sparks.back());}continue;}
            ++total;
            if(s.pattern>=50){s.vx=sar3(s.vx);s.vy=sar3(s.vy);if(s.pattern>=120){s.type=1;s.position[back][0]=static_cast<std::uint16_t>(s.position[back][0]-128);s.position[back][1]=static_cast<std::uint16_t>(s.position[back][1]-128);total+=large_bonus[level];}else{s.type=2;total+=main_bonus[level];}}
            hit_sparks.push_back({x,pixel(s.position[back][1]),30,1,false});
            if(spark)spark(hit_sparks.back());
        }
        hit_score_lowword_adds.push_back(static_cast<std::uint16_t>(total));
        damage_events+=total;return total;
    }
    void update(unsigned back,std::uint32_t stage_frame,const BoundaryExit &on_exit={}){
        if(back>1)throw std::runtime_error("Invalid shot page");
        for(auto &s:slots){if(s.flag!=1)continue;
            const auto old=s.position[back];
            s.position[back][0]=static_cast<std::uint16_t>(s.position[back][0]+s.vx);
            s.position[back][1]=static_cast<std::uint16_t>(s.position[back][1]+s.vy);
            const int raw_x=enemy_s16(s.position[back][0]),y=pixel(s.position[back][1]);
            const bool left=raw_x<=256,right=raw_x>=6688,top=y<8,bottom=y>384;
            if(left||right||top||bottom){
                if(on_exit&&s.type==unsigned(s.special!=0)&&((left&&s.vx<0)||(right&&s.vx>0)||(top&&s.vy<0)||(bottom&&s.vy>0)))on_exit(s,old);
                s.flag=2;continue;
            }
            if(s.special){
                if(s.type>1){const int interval=shot_type==2?3:4;if(stage_frame%unsigned(interval)==0){++s.type;if(s.type>4){s.flag=2;continue;}}if(stage_frame&1)s.vy=s.vy>=0?s.vy/2:-((-s.vy+1)/2);}
                else if(s.pattern!=59)s.vy=enemy_s16(unsigned(s.vy-4));
                s.render_pattern=static_cast<std::uint8_t>(s.pattern+s.type);
            }else{
                ++s.age;
                constexpr std::array<unsigned,6> large_hit={39,39,39,40,41,47}; // Original DS:0A94.
                s.render_pattern=static_cast<std::uint8_t>(s.type?(s.pattern>=124?large_hit.at(s.type>5?5:s.type):74+s.type):s.pattern+(s.pattern>=50?(s.age&1):0));
                if(s.type){if(s.type>5){s.flag=2;continue;}if((stage_frame&3)==0)++s.type;}
            }
        }
    }
    unsigned pattern_for(const Slot &s)const{return s.render_pattern;}
};
}
